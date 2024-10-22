zephyr_library()
zephyr_library_sources_ifdef(CONFIG_SRA_BOARD_COMPONENT mcpwm_interface.c adc.c lsa.c utils.c)
