/**
 * @file bar_graph.c
 * @brief LED Bar Graph Driver.
 * 
 * This file contains the implementation of the LED Bar Graph Driver.
 */

#include "bar_graph.h"
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>

#define LED0_NODE DT_ALIAS(led0)
static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED0_NODE, gpios);

int bar_graph_init(void)
{
    int err;

    err = gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE);
    if (err < 0)
    {
        return err;
    }

    return 0;
}

int bar_graph_toggle(void)
{
    int err = gpio_pin_toggle(&led);
    if (err < 0)
    {
        return err;
    }

    return 0;
}