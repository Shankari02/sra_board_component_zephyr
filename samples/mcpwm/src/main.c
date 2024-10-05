#include <mcpwm_interface.h>
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/sys/printk.h>
// #include <zephyr/logging/log.h>

#define PWM_POLARITY_NORMAL (0 << 0)
#define PWM_POLARITY_INVERTED (1 << 0)

// LOG_MODULE_REGISTER(MOTOR, LOG_LEVEL_INF);
#define MIN_PERIOD PWM_SEC(1U) / 128U
#define MAX_PERIOD PWM_SEC(1U)
static const struct pwm_dt_spec pwm_led0 = PWM_DT_SPEC_GET(DT_ALIAS(pwm_led0));

#if DT_NODE_HAS_STATUS(DT_ALIAS(pwm_led0), okay)
#define PWM_DEV_NODE DT_ALIAS(pwm_led0)
#else
#error "Define a PWM device"
#endif

int main(void)
{
    printk("PWM-based blinky\n");

    if (!pwm_is_ready_dt(&pwm_led0))
    {
        printk("Error: PWM device %s is not ready\n",
               pwm_led0.dev->name);
        return 0;
    }

    motor_enable();
    while (1)
    {
        motor_forward();
        printk("Forward");
        k_sleep(K_SECONDS(4U));

        motor_backward();
        printk("Backward");
        k_sleep(K_SECONDS(4U));

        motor_brake();
        printk("Stop");
        k_sleep(K_SECONDS(4U));

        motor_disable();
        k_sleep(K_SECONDS(4U));
    }

    return 0;
}
