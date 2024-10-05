#include <zephyr/kernel.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/logging/log.h>
#include <zephyr/sys/printk.h>
#include <zephyr/sys/util.h>
#include <zephyr/device.h>

LOG_MODULE_REGISTER(i2cdev);

typedef struct {
    struct k_sem lock; //for managing access to i2c
    struct i2c_config config; //stores configuration of i2c port
    bool installed; //indicates whether i2c driver is installed for the port
} i2c_port_state_t;

static i2c_port_state_t states[I2C_NUM_MAX]; //track i2c port's state
static bool i2c_initialised = false; //to check if i2c subsystem has been initialised

#if CONFIG_I2CDEV_NOLOCK //to check if i2c is locked
#define SEMAPHORE_TAKE(port) do { } while (0)
#define SEMAPHORE_GIVE(port) do { } while (0)
#else
#define SEMAPHORE_TAKE(port) \
    do { \
        if (k_sem_take(&states[port].lock, K_MSEC(CONFIG_I2CDEV_TIMEOUT)) != 0) { \
            LOG_ERR("Could not take port semaphore %d", port); \
            return -ETIME; \
        } \
    } while (0)

#define SEMAPHORE_GIVE(port) \
    do { \
        if (k_sem_give(&states[port].lock) != 0) { \
            LOG_ERR("Could not give port semaphore %d", port); \
            return -EIO; \
        } \
    } while (0)
#endif

int i2cdev_init(void) //initialise i2c
{
    if (!i2c_initialised)
    {
        memset(states, 0, sizeof(states)); //clears the states array

#if !CONFIG_I2CDEV_NOLOCK
        for (int i = 0; i < I2C_NUM_MAX; i++)
        {
            k_sem_init(&states[i].lock, 1, 1); //initializes semaphore for each i2c port
        }
#endif
        i2c_initialised = true;
    }

    return 0;
}

int i2cdev_done(void) 
{ //Checks if the port is installed, takes the semaphore, unregisters the device if it's ready, and then updates the state.
    for (int i = 0; i < I2C_NUM_MAX; i++)
    {
        if (states[i].installed)
        {
            SEMAPHORE_TAKE(i);
            const struct device *dev = DEVICE_DT_GET(DT_NODELABEL(i));
            if (device_is_ready(dev)) {
                i2c_unregister(dev);
            }
            states[i].installed = false;
            SEMAPHORE_GIVE(i);
        }
#if !CONFIG_I2CDEV_NOLOCK
        k_sem_reset(&states[i].lock); //if locked, it resets the ports
#endif
    }
    return 0;
}

int i2c_dev_create_mutex(i2c_dev_t *dev)
{
#if !CONFIG_I2CDEV_NOLOCK
    if (!dev) return -EINVAL;

    LOG_DBG("[0x%02x at %d] creating mutex", dev->addr, dev->port);

    k_sem_init(&dev->mutex, 1, 1);
#endif

    return 0;
}

int i2c_dev_delete_mutex(i2c_dev_t *dev)
{
#if !CONFIG_I2CDEV_NOLOCK
    if (!dev) return -EINVAL;

    LOG_DBG("[0x%02x at %d] deleting mutex", dev->addr, dev->port);

    k_sem_reset(&dev->mutex);
#endif
    return 0;
}

int i2c_dev_take_mutex(i2c_dev_t *dev)
{
#if !CONFIG_I2CDEV_NOLOCK
    if (!dev) return -EINVAL;

    LOG_DBG("[0x%02x at %d] taking mutex", dev->addr, dev->port);

    if (k_sem_take(&dev->mutex, K_MSEC(CONFIG_I2CDEV_TIMEOUT)) != 0)
    {
        LOG_ERR("[0x%02x at %d] Could not take device mutex", dev->addr, dev->port);
        return -ETIME;
    }
#endif
    return 0;
}

int i2c_dev_give_mutex(i2c_dev_t *dev)
{
#if !CONFIG_I2CDEV_NOLOCK
    if (!dev) return -EINVAL;

    LOG_DBG("[0x%02x at %d] giving mutex", dev->addr, dev->port);

    if (k_sem_give(&dev->mutex) != 0)
    {
        LOG_ERR("[0x%02x at %d] Could not give device mutex", dev->addr, dev->port);
        return -EIO;
    }
#endif
    return 0;
}

inline static bool cfg_equal(const struct i2c_config *a, const struct i2c_config *b)
{
    return a->scl_pin == b->scl_pin
        && a->sda_pin == b->sda_pin
        && a->frequency == b->frequency
        && a->flags == b->flags;
}

static int i2c_setup_port(const i2c_dev_t *dev)
{
    if (dev->port >= I2C_NUM_MAX) return -EINVAL;

    int res;
    if (!cfg_equal(&dev->cfg, &states[dev->port].config))
    {
        LOG_DBG("Reconfiguring I2C driver on port %d", dev->port);
        struct i2c_config temp;
        memcpy(&temp, &dev->cfg, sizeof(struct i2c_config));

        // Driver reinstallation
        if (states[dev->port].installed)
        {
            const struct device *i2c_dev = DEVICE_DT_GET(DT_NODELABEL(dev->port));
            if (device_is_ready(i2c_dev)) {
                i2c_unregister(i2c_dev);
            }
        }
        
        const struct device *i2c_dev = DEVICE_DT_GET(DT_NODELABEL(dev->port));
        if (!device_is_ready(i2c_dev)) {
            return -ENODEV;
        }
        
        if ((res = i2c_configure(i2c_dev, &temp)) != 0)
            return res;

        states[dev->port].installed = true;
        memcpy(&states[dev->port].config, &temp, sizeof(struct i2c_config));
        LOG_DBG("I2C driver successfully reconfigured on port %d", dev->port);
    }

    return 0;
}

int i2c_dev_read(const i2c_dev_t *dev, const void *out_data, size_t out_size, void *in_data, size_t in_size)
{
    if (!dev || !in_data || !in_size) return -EINVAL;

    SEMAPHORE_TAKE(dev->port);

    int res = i2c_setup_port(dev);
    if (res == 0)
    {
        const struct device *i2c_dev = DEVICE_DT_GET(DT_NODELABEL(dev->port));
        if (!device_is_ready(i2c_dev)) {
            SEMAPHORE_GIVE(dev->port);
            return -ENODEV;
        }

        struct i2c_msg msgs[2] = {
            {
                .buf = (void *)out_data,
                .len = out_size,
                .flags = I2C_MSG_WRITE,
            },
            {
                .buf = in_data,
                .len = in_size,
                .flags = I2C_MSG_READ,
            }
        };

        res = i2c_transfer(i2c_dev, msgs, ARRAY_SIZE(msgs));
        if (res != 0)
            LOG_ERR("Could not read from device [0x%02x at %d]: %d", dev->addr, dev->port, res);
    }

    SEMAPHORE_GIVE(dev->port);
    return res;
}

int i2c_dev_write(const i2c_dev_t *dev, const void *out_reg, size_t out_reg_size, const void *out_data, size_t out_size)
{
    if (!dev || !out_data || !out_size) return -EINVAL;

    SEMAPHORE_TAKE(dev->port);

    int res = i2c_setup_port(dev);
    if (res == 0)
    {
        const struct device *i2c_dev = DEVICE_DT_GET(DT_NODELABEL(dev->port));
        if (!device_is_ready(i2c_dev)) {
            SEMAPHORE_GIVE(dev->port);
            return -ENODEV;
        }

        struct i2c_msg msgs[2] = {
            {
                .buf = (void *)out_reg,
                .len = out_reg_size,
                .flags = I2C_MSG_WRITE,
            },
            {
                .buf = (void *)out_data,
                .len = out_size,
                .flags = I2C_MSG_WRITE,
            }
        };

        res = i2c_transfer(i2c_dev, msgs, ARRAY_SIZE(msgs));
        if (res != 0)
            LOG_ERR("Could not write to device [0x%02x at %d]: %d", dev->addr, dev->port, res);
    }

    SEMAPHORE_GIVE(dev->port);
    return res;
}

int i2c_dev_read_reg(const i2c_dev_t *dev, uint8_t reg, void *in_data
