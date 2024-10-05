#ifndef MCPWM_INTERFACE_H
#define MCPWM_INTERFACE_H
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/logging/log.h>
#include <zephyr/drivers/pwm.h>

/**
 * @brief Motor interface
 */

int motor_enable();
/**
 * @brief Disable motor
 * @return 0 on success, negative error code on failure
 */
int motor_disable();
/**
 * @brief Set motor speed
 * @return 0 on success, negative error code on failure
 */
int motor_set_speed();
/**
 * @brief Set direction of motor to forward
 * @return 0 on success, negative error code on failure
 */
int motor_forward();
/**
 * @brief Set direction of motor to backward
 * @return 0 on success, negative error code on failure
 */
int motor_backward();
/**
 * @brief Brake motor
 * @return 0 on success, negative error code on failure
 */
int motor_brake();

#ifdef _cplusplus
#endif
#endif /* MCPWM_INTERFACE_H */
