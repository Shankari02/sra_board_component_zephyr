#include "sra_board.h"

#define BLACK_MARGIN 4095
#define WHITE_MARGIN 0
#define CONSTRAIN_LSA_LOW 0
#define CONSTRAIN_LSA_HIGH 1000

int main(void)
{
    // Enable line sensor
    enable_line_sensor();
    printk("Line sensor enabled\n");
    line_sensor_array line_sensor_readings;
    while (1)
    {
        printk("Inside while of main.c\n");
        //  Get line sensor readings
        line_sensor_readings = read_line_sensor();
        for (int i = 0; i < 5; i++)
        {
            // Constrain lsa readings between BLACK_MARGIN and WHITE_MARGIN
            line_sensor_readings.adc_reading[i] = bound(line_sensor_readings.adc_reading[i], WHITE_MARGIN, BLACK_MARGIN);
            // Map readings from (BLACK_MARGIN, WHITE_MARGIN) to (CONSTRAIN_LSA_LOW, CONSTRAIN_LSA_HIGH)
            line_sensor_readings.adc_reading[i] = map(line_sensor_readings.adc_reading[i], WHITE_MARGIN, BLACK_MARGIN, CONSTRAIN_LSA_LOW, CONSTRAIN_LSA_HIGH);
            line_sensor_readings.adc_reading[i] = 1000 - line_sensor_readings.adc_reading[i];

            // Sleep for 10 ms
            k_sleep(K_MSEC(5));
        }

        // Log final lsa readings
        printk("LSA_0: %d \t LSA_1: %d \t LSA_2: %d \t LSA_3: %d \t LSA_4: %d \n",line_sensor_readings.adc_reading[0], line_sensor_readings.adc_reading[1], line_sensor_readings.adc_reading[2], line_sensor_readings.adc_reading[3], line_sensor_readings.adc_reading[4]);
    }
}