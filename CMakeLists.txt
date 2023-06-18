cmake_minimum_required(VERSION 3.24)
include(pico_sdk_import.cmake)


project(pi_pico_sh1106 C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
pico_sdk_init()

add_executable(pi_pico_sh1106 main.c sh1106_i2c.c sh1106_i2c.h consolas.h font_inconsolata.h)

target_link_libraries(pi_pico_sh1106 pico_stdlib hardware_i2c)

# create map/bin/hex file etc.
pico_enable_stdio_usb(pi_pico_sh1106 1)
pico_add_extra_outputs(pi_pico_sh1106)