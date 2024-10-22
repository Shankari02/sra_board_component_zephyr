#ifndef _ADC_H
#define _ADC_H

#include <zephyr/kernel.h>
#include <zephyr/drivers/adc.h>
#include <zephyr/logging/log.h>

#define ADC_RESOLUTION 12 // 12 bits resolution
#define ADC_REFERENCE_VOLTAGE 3.3 // Reference voltage


/**
 * @brief call function config_adc1() and characterize_adc1().
 * 
 * @return returns 0 if successful, or a negative error code.
 **/
int config_adc();

/**
 * @brief Reads the adc value from the GPIO(channel) specified.
 * 
 * @param adc_obj_t adc_handle - pointer to adc object.
 * @param int gpio - gpio pin number of the channel to be read.
 * @return int - returns the adc value read from the channel. 
 **/
int read_adc();

#endif