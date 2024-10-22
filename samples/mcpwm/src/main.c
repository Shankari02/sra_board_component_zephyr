#include <mcpwm_interface.h>
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/sys/printk.h>
// #include <zephyr/logging/log.h>

static const struct pwm_dt_spec pwm_led0 = PWM_DT_SPEC_GET(DT_ALIAS(pwm_led0));

#if DT_NODE_HAS_STATUS(DT_ALIAS(pwm_led0), okay)
#define PWM_DEV_NODE DT_ALIAS(pwm_led0)
#else
#error "Define a PWM device"
#endif

int main(void)
{
    if (!pwm_is_ready_dt(&pwm_led0))
    {
        printk("Error: PWM device %s is not ready\n",
               pwm_led0.dev->name);
        return 0;
    }
    motor_enable();
    printk("Motor enabled\n");

    while (1)
    {
        printk("Forward");
        motor_forward();
        k_sleep(K_SECONDS(4U));

        printk("Backward");
        motor_backward();
        k_sleep(K_SECONDS(4U));

        // motor_brake();
        // printk("Stop");
        // k_sleep(K_SECONDS(4U));

        // motor_disable();
        // k_sleep(K_SECONDS(4U));
    }

    return 0;
}
