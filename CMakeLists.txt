cmake_minimum_required(VERSION 3.13)

# initialize the SDK directly
include(pico_sdk_import.cmake)

project(dht C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)


# initialize the Raspberry Pi Pico SDK
pico_sdk_init()


add_executable(dht
    dht.c
    dhtlib.c
    # dhtlib.h
    # ssd1306.h
    # ssd1306.c
    # font.h
)

pico_enable_stdio_usb(dht 1)
pico_enable_stdio_uart(dht 0)



target_link_libraries(dht
    pico_stdlib
    # hardware_i2c
)

pico_add_extra_outputs(dht)