cmake_minimum_required(VERSION 3.13)

# Initialize pico-sdk from submodule
# Note: must run before project()
include(pico-sdk/pico_sdk_init.cmake)

project(firmware VERSION 1.0)

# Initialize the Raspberry Pi Pico SDK
pico_sdk_init()

include_directories(include)

if (TARGET tinyusb_device)
  file(GLOB SOURCES "src/*.c")
  add_executable(firmware ${SOURCES})

  # Add pico_stdlib library which aggregates commonly used features
  target_link_libraries(firmware
      hardware_i2c
      hardware_irq
      hardware_pwm
      pico_stdlib
      )

  # enable usb output, disable uart output
  pico_enable_stdio_usb(firmware 1)
  pico_enable_stdio_uart(firmware 0)

  # Create map/bin/hex/uf2 file in addition to ELF
  pico_add_extra_outputs(firmware)

elseif(PICO_ON_DEVICE)
  message(WARNING "not building firmware because TinyUSB submodule is not initialized in the SDK")
endif()

