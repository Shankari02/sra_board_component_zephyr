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

int bar_graph_init(void);

int bar_graph_toggle(void);

#ifdef __cplusplus
}
#endif

#endif // BAR_GRAPH_H