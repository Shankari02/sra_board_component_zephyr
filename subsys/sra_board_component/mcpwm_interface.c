#include <mcpwm_interface.h>
#include <zephyr/device.h>
#include <zephyr/drivers/pwm.h>
// #include <zephyr/logging/log.h>

// LOG_MODULE_REGISTER(MOTOR, LOG_LEVEL_INF);
#define PWM_POLARITY_NORMAL (0 << 0)
#define PWM_POLARITY_INVERTED (1 << 0)
#define MIN_PERIOD PWM_SEC(1U) / 128U
#define MAX_PERIOD PWM_SEC(1U)

static const struct pwm_dt_spec pwm_led0 = PWM_DT_SPEC_GET(DT_ALIAS(pwm_led0));
uint32_t max_period;
int period;
uint8_t dir = 0U;
int ret;

int motor_enable()
{
    printk("Inside motor enable\n");
    if (!pwm_is_ready_dt(&pwm_led0))
    {
        printk("Error: PWM device %s is not ready\n",
               pwm_led0.dev->name);
        return 0;
    }
    printk("Calibrating for channel %d...\n", pwm_led0.channel);
    max_period = MAX_PERIOD;
    while (pwm_set_dt(&pwm_led0, max_period, max_period / 2U))
    {
        max_period /= 2U;
        if (max_period < (4U * MIN_PERIOD))
        {
            printk("Error: PWM device "
                   "does not support a period at least %lu\n",
                   4U * MIN_PERIOD);
            return 0;
        }
    }
    printk("Done calibrating; maximum/minimum periods %u/%lu nsec\n",
           max_period, MIN_PERIOD);

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
    printk("Setting motor speed\n");
    printk("Period %d\n", period);
    
    if(dir==0U){
    period = MAX_PERIOD;
    while (period > MIN_PERIOD)
    {
        ret = pwm_set(pwm_led0.dev, pwm_led0.channel, period, period / 2U, PWM_POLARITY_NORMAL);
        if (ret)
        {
            printk("Error %d: failed to set pulse width\n", ret);
            return 0;
        }
        printk("Using period %d\n", period);
        period = period / 2U;
        if (period == MIN_PERIOD)
        {
            break;
        }
        k_sleep(K_SECONDS(4U));
    }
    }
    if(dir==1U){
        period = MAX_PERIOD;
    while (period > MIN_PERIOD)
    {
        ret = pwm_set(pwm_led0.dev, pwm_led0.channel, period, period / 2U, PWM_POLARITY_INVERTED);
        if (ret)
        {
            printk("Error %d: failed to set pulse width\n", ret);
            return 0;
        }
        printk("Using period %d\n", period);
        period = period / 2U;
        if (period == MIN_PERIOD)
        {
            break;
        }
        k_sleep(K_SECONDS(4U));
    }
    }
    return 0;
}

int motor_forward()
{
    printk("Inside motor_forward\n");
    if (!pwm_led0.dev->name)
    {
        printk("Invalid argument");
        return -EINVAL;
    }
    dir = 0U;
    // Set PWM for forward direction
    return motor_set_speed();
}

int motor_backward()
{
    if (!pwm_led0.dev->name)
    {
        printk("Invalid argument");
        return -EINVAL;
        motor_backward();
    }
    dir = 1U;
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
        k_sleep(K_SECONDS(1U));
    }
    return 0;
}
