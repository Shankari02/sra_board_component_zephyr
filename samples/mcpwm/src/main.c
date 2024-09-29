#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/logging/log.h>
#include <mcpwm_interface.h>

#define 	PWM_POLARITY_NORMAL   (0 << 0)
#define 	PWM_POLARITY_INVERTED   (1 << 0)

LOG_MODULE_REGISTER(MOTOR, LOG_LEVEL_INF);

static const struct pwm_dt_spec pwm_led0 = PWM_DT_SPEC_GET(DT_ALIAS(pwm_led0));

#define MOTOR_SPEED 60
#define MOTOR_DELAY_MS 1000

void motor_thread(void)
{   
       
    struct device *motor1 = device_get_binding(DT_NODELABEL(pwm_led0));
    if (motor1 == NULL) {
        LOG_ERR("Motor device not found");
        return;
    }

    motor_enable(motor1);

    while (1) {
        motor_forward(pwm_led0, MOTOR_SPEED);
        LOG_INF("Forward");
        k_sleep(K_MSEC(MOTOR_DELAY_MS));

        motor_backwards(pwm_led0, MOTOR_SPEED);
        LOG_INF("Backward");
        k_sleep(K_MSEC(MOTOR_DELAY_MS));

        motor_brake(pwm_led0);
        LOG_INF("Stop");
        k_sleep(K_MSEC(MOTOR_DELAY_MS));
    }
}

K_THREAD_DEFINE(motor_thread_id, 1024, motor_thread, NULL, NULL, NULL, 7, 0, 0);
