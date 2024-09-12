#ifndef MCPWM_INTERFACE_H
#define MCPWM_INTERFACE_H
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/logging/log.h>
#include <zephyr/drivers/pwm.h>

/**
 * @brief Motor interface
 */
typedef struct motor_t motor_t;
typedef struct motor_t* motor_handle_t;

struct motor_t {
    /**
     * @brief Enable motor
     * @param motor Motor object handle
     * @return 0 on success, negative error code on failure
     */
    int (*enable)(motor_t *motor);
    /**
     * @brief Disable motor
     * @param motor Motor object handle
     * @return 0 on success, negative error code on failure
     */
    int (*disable)(motor_t *motor);
    /**
     * @brief Set motor speed
     * @param motor Motor object handle
     * @param speed Speed of motor, 0~100
     * @return 0 on success, negative error code on failure
     */
    int (*set_speed)(motor_t *motor, uint32_t speed);
    /**
     * @brief Set direction of motor to forward
     * @param motor Motor object handle
     * @return 0 on success, negative error code on failure
     */
    int (*forward)(motor_t *motor);
    /**
     * @brief Set direction of motor to backward
     * @param motor Motor object handle
     * @return 0 on success, negative error code on failure
     */
    int (*backward)(motor_t *motor);
    /**
     * @brief Brake motor
     * @param motor Motor object handle
     * @return 0 on success, negative error code on failure
     */
    int (*brake)(motor_t *motor);
    /**
     * @brief Delete motor object
     * @param motor Motor object handle
     * @return 0 on success, negative error code on failure
     */
    int (*del)(motor_t *motor);
};

/**
 * @brief Motor Configuration
 */
typedef struct {
    uint32_t pwm0; /*!< Motor PWM A gpio number */
   // uint32_t pwm0; /*!< Motor PWM B gpio number */
    uint32_t pwm_freq_hz;   /*!< PWM frequency, in Hz */
} motor_config_t;

/**
 * @brief Motor MCPWM specific configuration
 */
typedef struct {
    int group_id;           /*!< MCPWM group number */
    uint32_t resolution_hz; /*!< MCPWM timer resolution */
} motor_mcpwm_config_t;


#ifdef _cplusplus
extern "C" {
#endif
/**
 * @brief Create Motor based on MCPWM peripheral
 *
 * @param motor_config: Motor configuration
 * @param mcpwm_config: MCPWM specific configuration
 * @param ret_motor: Returned Motor handle
 * @return 0 on success, negative error code on failure
 */
int motor_new_mcpwm_device(const motor_config_t *motor_config, const motor_mcpwm_config_t *mcpwm_config, motor_t** ret_motor);

#ifdef _cplusplus
}
#endif
#endif /* MCPWM_INTERFACE_H */
