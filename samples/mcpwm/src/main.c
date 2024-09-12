/*
 * Copyright (c) 2020 Friedt Professional Engineering Services, Inc
 *
 * SPDX-License-Identifier: Apache-2.0
 */

//#include <ztest.h>
//include
// //include/bar_graph.h
// #include "bar_graph.h"

// static void test_bar_graph(void)
// {
// 	int err;

//     err = bar_graph_init();
//     zassert_equal(err, 0, "bar_graph_init() failed");

//     err = bar_graph_toggle();
//     zassert_equal(err, 0, "bar_graph_toggle() failed");
// }

// void test_main(void)
// {
// 	ztest_test_suite(
// 		sra_board_component,
// 		ztest_unit_test(test_bar_graph)
// 	);

// 	ztest_run_test_suite(sra_board_component);
// }
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/logging/log.h>
#include <mcpwm_interface.h>

#define 	PWM_POLARITY_NORMAL   (0 << 0)
#define 	PWM_POLARITY_INVERTED   (1 << 0)

LOG_MODULE_REGISTER(MOTOR, LOG_LEVEL_INF);
//static const struct pwm_dt_spec pwm_led0 = PWM_DT_SPEC_GET(DT_ALIAS(pwm_led0));
static const struct pwm_dt_spec pwm_led0 = PWM_DT_SPEC_GET(DT_ALIAS(pwm_led0));
#define MOTOR_SPEED 60
#define MOTOR_DELAY_MS 1000

void motor_thread(void)
{
    struct device *motor1 = device_get_binding(DT_LABEL(DT_NODELABEL(motor1)));
    if (motor1 == NULL) {
        LOG_ERR("Motor device not found");
        return;
    }

    enable_motor_driver(motor1, MOTOR_A_1);

    while (1) {
        set_motor_speed(pwm_led0, PWM_POLARITY_NORMAL, MOTOR_SPEED);
        LOG_INF("Forward");
        k_sleep(K_MSEC(MOTOR_DELAY_MS));

        set_motor_speed(pwm_led0, PWM_POLARITY_INVERTED, MOTOR_SPEED);
        LOG_INF("Backward");
        k_sleep(K_MSEC(MOTOR_DELAY_MS));

        set_motor_speed(pwm_led0, MOTOR_STOP, 0);
        LOG_INF("Stop");
        k_sleep(K_MSEC(MOTOR_DELAY_MS));
    }
}

K_THREAD_DEFINE(motor_thread_id, 1024, motor_thread, NULL, NULL, NULL, 7, 0, 0);
