#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <sra/mcpwm_interface/mcpwm_interface.h>
//#include <zephyr/logging/log.h>


#define 	PWM_POLARITY_NORMAL   (0 << 0)
#define 	PWM_POLARITY_INVERTED   (1 << 0)

//LOG_MODULE_REGISTER(MOTOR, LOG_LEVEL_INF);

static const struct pwm_dt_spec pwm_led0 = PWM_DT_SPEC_GET(DT_ALIAS(pwm_led0));

#define MOTOR_SPEED 60
#define MOTOR_DELAY_MS 1000
#if DT_NODE_HAS_STATUS(DT_ALIAS(pwm_led0), okay)
#define PWM_DEV_NODE DT_ALIAS(pwm_led0)
#else
#error "Define a PWM device"
#endif


const struct device *get_pwm_device(void)
{
	return DEVICE_DT_GET(PWM_DEV_NODE);
}

void motor_thread(void)
{   
    // struct device *motor1 = device_get_binding(DT_NODELABEL(pwm_led0));
    // if (motor1 == NULL) {
    //     LOG_ERR("Motor device not found");
    //     return;
    // }
    const struct device *pwm_dev = get_pwm_device();

	if (!device_is_ready(pwm_dev)) {
		printk("PWM device is not ready\n");
		return;
	}
    motor_enable(*pwm_dev);
    motor_set_speed(*pwm_dev, MOTOR_SPEED);
    while (1) {
        motor_forward(*pwm_dev);
        printk("Forward");
        k_sleep(K_MSEC(MOTOR_DELAY_MS));

        motor_backwards(*pwm_dev);
        printk("Backward");
        k_sleep(K_MSEC(MOTOR_DELAY_MS));

        motor_brake(*pwm_dev);
        printk("Stop");
        k_sleep(K_MSEC(MOTOR_DELAY_MS));
    }
    motor_disable(*pwm_dev);
    motor_del(*pwm_dev);
}

K_THREAD_DEFINE(motor_thread_id, 1024, motor_thread, NULL, NULL, NULL, 7, 0, 0);
