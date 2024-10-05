#include <mcpwm_interface.h>
#include <zephyr/device.h>
#include <zephyr/drivers/pwm.h>
// #include <zephyr/logging/log.h>

// LOG_MODULE_REGISTER(MOTOR, LOG_LEVEL_INF);
#define MIN_PERIOD PWM_SEC(1U) / 128U
#define MAX_PERIOD PWM_SEC(1U)

static const struct pwm_dt_spec pwm_led0 = PWM_DT_SPEC_GET(DT_ALIAS(pwm_led0));
uint32_t max_period;
uint32_t period;
uint8_t dir = 0U;
int ret;

int motor_enable()
{
    if (!pwm_is_ready_dt(&pwm_led0))
    {
        printk("Error: PWM device %s is not ready\n",
               pwm_led0.dev->name);
        return 0;
    }

    return 0;
}

int motor_disable()
{
    if (!pwm_led0.dev->name)
    {
        printk("Invalid argument");
        return -EINVAL;
    }

    // Stop PWM (Set duty to 0)
    return pwm_set_dt(&pwm_led0, MAX_PERIOD, 0);
}

int motor_set_speed()
{
    if (!pwm_led0.dev->name)
    {
        printk("Invalid argument");
        return -EINVAL;
    }

    period = max_period;
    while (1)
    {
        ret = pwm_set_dt(&pwm_led0, period, period / 2U);
        if (ret)
        {
            printk("Error %d: failed to set pulse width\n", ret);
            return 0;
        }
        printk("Using period %d\n", period);

        // period = dir ? (period * 2U) : (period / 2U);
        if (period > max_period)
        {
            period = max_period / 2U;
            dir = 0U;
        }
        else if (period < MIN_PERIOD)
        {
            period = MIN_PERIOD * 2U;
            dir = 1U;
        }

        k_sleep(K_SECONDS(4U));
    }
    return 0;
}

int motor_forward()
{
    if (!pwm_led0.dev->name)
    {
        printk("Invalid argument");
        return -EINVAL;
    }

    // Set PWM for forward direction
    return motor_set_speed();
}

int motor_backward()
{
    if (!pwm_led0.dev->name)
    {
        printk("Invalid argument");
        return -EINVAL;
    }

    // Set PWM for backward direction
    return motor_set_speed();
}

int motor_brake()
{
    if (!pwm_led0.dev->name)
    {
        printk("Invalid argument");
        return -EINVAL;
    }
    while (1)
    {
        period = 0;
        ret = pwm_set_dt(&pwm_led0, 0, 0);
        if (ret)
        {
            printk("Error %d: failed to set pulse width\n", ret);
            return 0;
        }
        printk("Using period %d\n", period);
        k_sleep(K_SECONDS(4U));
    }
    return 0;
}
