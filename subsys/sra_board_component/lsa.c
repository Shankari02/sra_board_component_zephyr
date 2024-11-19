#include "sra_board.h"

int enable_line_sensor()
{
    config_adc();
    return 0;
}

line_sensor_array read_line_sensor()
{
    line_sensor_array line_sensor_readings;

    for (int i = 0; i < 5; i++)
    {
        line_sensor_readings.adc_reading[i] = 0;
    }

    for (int i = 0; i < NUMBER_OF_SAMPLES; i++)
    {
        for (int j = 0; j < 5; j++)
        {
            line_sensor_readings.adc_reading[j] = line_sensor_readings.adc_reading[j] + read_adc();
            k_sleep(K_MSEC(10));
        }
    }

    for (int i = 0; i < 5; i++)
    {
    line_sensor_readings.adc_reading[i] = line_sensor_readings.adc_reading[i] / NUMBER_OF_SAMPLES;
    }

    return line_sensor_readings;
}