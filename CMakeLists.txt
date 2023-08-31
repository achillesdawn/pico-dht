cmake_minimum_required(VERSION 3.13)

# initialize the SDK directly
include(pico_sdk_import.cmake)

project(blink_irq C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)


# initialize the Raspberry Pi Pico SDK
pico_sdk_init()



add_executable(blink_irq
    blink_irq.c
    ssd1306.h
    ssd1306.c
    font.h
)

pico_enable_stdio_usb(blink_irq 1)
pico_enable_stdio_uart(blink_irq 0)

# target_include_directories(blink_irq
#     PRIVATE ssd1306
# )

target_link_libraries(blink_irq 
    pico_stdlib
    hardware_i2c
)

pico_add_extra_outputs(blink_irq)