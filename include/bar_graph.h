#ifndef BAR_GRAPH_H
#define BAR_GRAPH_H

/**
 * @file bar_graph.h
 * @brief LED Bar Graph Driver.
 * 
 * This file contains the declarations of the LED Bar Graph Driver.
 */

#ifdef __cplusplus
extern "C" {
#endif
/**
  * enabled_bar_graph_flag is set to the value of Mode accordingly.
 * 
 * @brief enables the bar graph led's gpio pins
 * 
 * @return sees if enabled correctly, returns 0 if not
 */
int enable_bar_graph();
/**
 * @brief Set motor speed
 * 
 * @param data
 * @return sees if enabled correctly, returns 0 if not
 */
int set_bar_graph(uint8_t data);
/**
 * @brief converts a boolean array of size 8 into 8-bit unsigned integer
 * 
 * @param input_array boolean input array of size 8
 * @return uint8_t returns a 8 bit number formed after combining the boolean array
 */
uint8_t bool_to_uint8(bool *input_array);

#ifdef __cplusplus
}
#endif

#endif // BAR_GRAPH_H