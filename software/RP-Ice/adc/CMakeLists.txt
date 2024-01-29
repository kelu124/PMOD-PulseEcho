cmake_minimum_required(VERSION 3.13)

include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project(adc)

pico_sdk_init()

add_executable(pio_adc_dac adc.c)

pico_generate_pio_header(pio_adc_dac ${CMAKE_CURRENT_LIST_DIR}/adc.pio)

target_sources(pio_adc_dac PRIVATE adc.c)

target_link_libraries(pio_adc_dac PRIVATE pico_stdlib pico_multicore hardware_pio hardware_dma hardware_clocks hardware_gpio)
pico_add_extra_outputs(pio_adc_dac)

pico_enable_stdio_usb(pio_adc_dac 1)
pico_enable_stdio_uart(pio_adc_dac 0)
