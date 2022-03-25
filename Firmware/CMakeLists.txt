cmake_minimum_required(VERSION 3.13)

# Initialize pico-sdk from submodule
# Note: must run before project()
include(pico-sdk/pico_sdk_init.cmake)

project(firmware)

# Initialize the Raspberry Pi Pico SDK
pico_sdk_init()

add_executable(firmware
  main.c
  )

# Add pico_stdlib library which aggregates commonly used features
target_link_libraries(firmware pico_stdlib)

# Create map/bin/hex/uf2 file in addition to ELF
pico_add_extra_outputs(firmware)

