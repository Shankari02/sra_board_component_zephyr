#include <zephyr/zephyr.h>
#include <zephyr/device.h>
#include <zephyr/drivers/pwm.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(MOTOR, LOG_LEVEL_INF);

typedef struct {
    // Add specific Zephyr PWM data structure
    const struct device *pwm_dev;
    pwm_pin_t pwm_pin;
    uint32_t period;
    uint32_t duty;
} motor_pwm_obj;

static int motor_enable(motor_pwm_obj *motor)
{
    if (!motor || !motor->pwm_dev) {
        LOG_ERR("Invalid argument");
        return -EINVAL;
    }
    
    //Set the period and pulse width for a single PWM output.
    return pwm_pin_set_usec(motor->pwm_dev, motor->pwm_pin, motor->period, motor->duty, 0); //duty, pwm, pin
}

static int motor_disable(motor_pwm_obj *motor)
{
    if (!motor || !motor->pwm_dev) {
        LOG_ERR("Invalid argument");
        return -EINVAL;
    }
    
    // Stop PWM (Set duty to 0)
    return pwm_pin_set_usec(motor->pwm_dev, motor->pwm_pin, motor->period, 0, 0);
}

static int motor_set_speed(motor_pwm_obj *motor, uint32_t speed)
{
    if (!motor || !motor->pwm_dev) {
        LOG_ERR("Invalid argument");
        return -EINVAL;
    }

    motor->duty = speed;
    return pwm_pin_set_usec(motor->pwm_dev, motor->pwm_pin, motor->period, motor->duty, 0);
}

static int motor_forward(motor_pwm_obj *motor)
{
    if (!motor || !motor->pwm_dev) {
        LOG_ERR("Invalid argument");
        return -EINVAL;
    }

    // Set PWM for forward direction
    return motor_set_speed(motor, motor->duty);
}

static int motor_backwards(motor_pwm_obj *motor)
{
    if (!motor || !motor->pwm_dev) {
        LOG_ERR("Invalid argument");
        return -EINVAL;
    }

    // Set PWM for backward direction
    return motor_set_speed(motor, motor->duty);
}

static int motor_brake(motor_pwm_obj *motor)
{
    if (!motor || !motor->pwm_dev) {
        LOG_ERR("Invalid argument");
        return -EINVAL;
    }
    return motor_set_speed(motor, 0);
}

static int motor_del(motor_pwm_obj *motor)
{
    if (!motor) {
        return -EINVAL;
    }
    
    // Cleanup
    k_free(motor);
    return 0;
}

int motor_new_pwm_device(const struct device *pwm_dev, pwm_pin_t pwm_pin, uint32_t period, motor_pwm_obj **ret_motor)
{
    motor_pwm_obj *motor = (motor_pwm_obj *)k_malloc(sizeof(motor_pwm_obj));
    if (!motor) {
        LOG_ERR("No memory for motor");
        return -ENOMEM;
    }
    
    motor->pwm_dev = pwm_dev;
    motor->pwm_pin = pwm0;
    motor->period = period;
    motor->duty = 0;
    
    *ret_motor = motor;
    
    LOG_INF("Motor %p created", motor);
    return 0;
}