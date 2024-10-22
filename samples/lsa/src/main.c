#include <zephyr/logging/log.h>
#include <stdio.h>
#include "adc.h" 
#include "lsa.h"
#include "utils.h"

#define BLACK_MARGIN 4095
#define WHITE_MARGIN 0
#define CONSTRAIN_LSA_LOW 0
#define CONSTRAIN_LSA_HIGH 1000

static const struct pwm_dt_spec pwm_led0 = PWM_DT_SPEC_GET(DT_ALIAS(pwm_led0));

int main(void)
{
    // Enable line sensor
    enable_line_sensor();

    line_sensor_array line_sensor_readings;

    while (1)
    {
        // Get line sensor readings
        line_sensor_readings = read_line_sensor();
        
        for (int i = 0; i < 5; i++)
        {
            // Constrain lsa readings between BLACK_MARGIN and WHITE_MARGIN
            line_sensor_readings.adc_reading[i] = bound(line_sensor_readings.adc_reading[i], WHITE_MARGIN, BLACK_MARGIN);
            // Map readings from (BLACK_MARGIN, WHITE_MARGIN) to (CONSTRAIN_LSA_LOW, CONSTRAIN_LSA_HIGH)
            line_sensor_readings.adc_reading[i] = map(line_sensor_readings.adc_reading[i], WHITE_MARGIN, BLACK_MARGIN, CONSTRAIN_LSA_LOW, CONSTRAIN_LSA_HIGH);
            line_sensor_readings.adc_reading[i] = 1000 - line_sensor_readings.adc_reading[i];
            
            // Sleep for 10 ms
            k_msleep(10);
        }

        // Log final lsa readings
        printk("LSA_0: %d \t ",
                 line_sensor_readings.adc_reading[0]);
    }
}