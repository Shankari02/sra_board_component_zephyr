#include <sra_board.h>
// #include <device.h>
// #include <drivers/motor.h>
// #include <drivers/adc.h>
// #include <logging/log.h>

#define STACK_SIZE 2048

#define BLACK_MARGIN 4095
#define WHITE_MARGIN 0
#define BOUND_LSA_LOW 0
#define BOUND_LSA_HIGH 1000
#define BLACK_BOUNDARY 890

/*
 * Weights given to respective line sensors
 */
const int weights[5] = {-5, -3, 1, 3, 5};

/*
 * Motor value bounds
 */

int lower_duty_cycle = PWM_SEC(1U) / 128U;
int higher_duty_cycle = PWM_SEC(1U) / 32U;
int optimum_duty_cycle = PWM_SEC(1U) / 64U;
float left_duty_cycle = 0, right_duty_cycle = 0;
double kp = 4, ki = 0, kd = 12;

/*
 * Line Following PID Variables
 */
double error = 0, prev_error = 0, difference, cumulative_error, correction;

// Read line sensor data
line_sensor_array line_sensor_readings;

void calculate_correction()
{
    error *= 10; // Scale error
    difference = error - prev_error;
    cumulative_error += error;

    cumulative_error = bound(cumulative_error, -30, 30);

    correction = kp * error + ki * cumulative_error + kd * difference;
    prev_error = error;
}

void calculate_error()
{
    int all_black_flag = 1; // Assume all black condition
    float weighted_sum = 0, sum = 0;
    float pos = 0;
    int k;

    for (int i = 0; i < 5; i++)
    {
        if (line_sensor_readings.adc_reading[i] > BLACK_BOUNDARY)
        {
            all_black_flag = 0;
        }
        k = (line_sensor_readings.adc_reading[i] > BLACK_BOUNDARY) ? 1 : 0;
        weighted_sum += (float)(weights[i]) * k;
        sum += k;
    }

    if (sum != 0) // Safety check
    {
        pos = (weighted_sum - 1) / sum; // Position wrt line
    }

    if (all_black_flag == 1) // All black condition
    {
        error = (prev_error > 0) ? 2.5 : -2.5;
    }
    else
    {
        error = pos;
    }
}

void line_follow_task(void)
{
    motor_enable();
    enable_line_sensor();
    while (1)
    {
        line_sensor_readings = read_line_sensor();
        for (int i = 0; i < 5; i++)
        {
            line_sensor_readings.adc_reading[i] = bound(line_sensor_readings.adc_reading[i], WHITE_MARGIN, BLACK_MARGIN);
            line_sensor_readings.adc_reading[i] = map(line_sensor_readings.adc_reading[i], WHITE_MARGIN, BLACK_MARGIN, BOUND_LSA_LOW, BOUND_LSA_HIGH);
            line_sensor_readings.adc_reading[i] = 1000 - (line_sensor_readings.adc_reading[i]);
        }

        calculate_error();
        calculate_correction();

        left_duty_cycle = bound((optimum_duty_cycle + correction), lower_duty_cycle, higher_duty_cycle);
        right_duty_cycle = bound((optimum_duty_cycle - correction), lower_duty_cycle, higher_duty_cycle);

        // Set motor speed
        motor1_set_speed(left_duty_cycle);
        motor2_set_speed(right_duty_cycle);

        k_sleep(K_MSEC(10));
    }
}

int main(void)
{
    line_follow_task();
}
