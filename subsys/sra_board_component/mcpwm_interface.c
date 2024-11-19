#include <sra_board.h>

#define PWM_POLARITY_NORMAL (0 << 0)
#define PWM_POLARITY_INVERTED (1 << 0)
#define MIN_PERIOD PWM_SEC(1U) / 128U
#define MAX_PERIOD PWM_SEC(1U)
static const struct pwm_dt_spec pwm_led0 = PWM_DT_SPEC_GET(DT_ALIAS(pwm_led0));
static const struct pwm_dt_spec pwm_led1 = PWM_DT_SPEC_GET(DT_ALIAS(pwm_led1));

uint32_t max_period;
//int period;
uint8_t dir = 0U;
int ret1;
int ret2;

int motor_enable()
{
    if (!pwm_is_ready_dt(&pwm_led0))
    {
        printk("Error: PWM device %s is not ready\n",
               pwm_led0.dev->name);
        return 0;
    }
    if (!pwm_is_ready_dt(&pwm_led1))
    {
        printk("Error: PWM device %s is not ready\n",
               pwm_led1.dev->name);
        return 0;
    }
    printk("Calibrating for channel %d, %d...\n", pwm_led0.channel, pwm_led1.channel);
    max_period = MAX_PERIOD;
    while (pwm_set_dt(&pwm_led0, max_period, max_period / 2U) && pwm_set_dt(&pwm_led1, max_period, max_period / 2U))
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
    printk("Done calibrating; maximum/minimum periods %u, %lu nsec\n",
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
    if (!pwm_led1.dev->name)
    {
        printk("Invalid argument");
        return -EINVAL;
    }

    // Stop PWM (Set duty to 0)
    pwm_set_dt(&pwm_led0, MAX_PERIOD, 0);
    pwm_set_dt(&pwm_led1, MAX_PERIOD, 0);
    return 0;
}

int motor1_set_speed(int period1)
{
    if (!pwm_led0.dev->name)
    {
        printk("Invalid argument");
        return -EINVAL;
    }
    printk("Setting motor1 speed\n");
    printk("Period %d\n", period1);

    while (1)
    {
        ret1 = pwm_set(pwm_led0.dev, pwm_led0.channel, 3U*period1, period1 / 4U, PWM_POLARITY_INVERTED);
        if (ret1)
        {
            printk("Error %d: failed to set pulse width\n", ret1);
            return 0;
        }
        if (period1 < MIN_PERIOD)
        {
            break;
        }
        k_sleep(K_SECONDS(4U));
    }
    //}
    return 0;
}

int motor2_set_speed(int period2)
{
    if (!pwm_led1.dev->name)
    {
        printk("Invalid argument");
        return -EINVAL;
    }
    printk("Setting motor2 speed\n");
    printk("Period %d\n", period2);
    
    while (1)
    {
        ret2 = pwm_set(pwm_led1.dev, pwm_led1.channel, 3U*period2, period2 / 4U, PWM_POLARITY_INVERTED);
        if (ret2)
        {
            printk("Error %d: failed to set pulse width\n", ret2);
            return 0;
        }
        if (period2 < MIN_PERIOD)
        {
            break;
        }
        k_sleep(K_SECONDS(4U));
    }
    return 0;
}

int motor_backward()
{
    if (!pwm_led0.dev->name)
    {
        printk("Invalid argument");
        return -EINVAL;
    }
    if (!pwm_led1.dev->name)
    {
        printk("Invalid argument");
        return -EINVAL;
    }
    dir = 1U;
    // Set PWM for backward direction
    return 0;
}

int motor_brake()
{
    if (!pwm_led0.dev->name)
    {
        printk("Invalid argument");
        return -EINVAL;
    }
    if (!pwm_led1.dev->name)
    {
        printk("Invalid argument");
        return -EINVAL;
    }
    while (1)
    {
        ret1 = pwm_set_dt(&pwm_led0, 0, 0);
        ret2 = pwm_set_dt(&pwm_led1, 0, 0);
        if (ret1 && ret2)
        {
            printk("Error %d, %d: failed to set pulse width\n", ret1, ret2);
            return 0;
        }
        k_sleep(K_SECONDS(1U));
    }
    return 0;
}
