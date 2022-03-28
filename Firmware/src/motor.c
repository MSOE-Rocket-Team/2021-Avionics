#include <inttypes.h>

#include "hardware/pwm.h"
#include "pico/stdlib.h"

#include "motor.h"

#define PWM_IN1 14
#define PWM_IN2 15

void motor_init() {
  // RP2040 GPIO 14 -> MCU DRV8870DDA IN1
  gpio_set_function(PWM_IN1, GPIO_FUNC_PWM);
  // RP2040 GPIO 15 -> MCU DRV8870DDA IN2
  gpio_set_function(PWM_IN2, GPIO_FUNC_PWM);

  pwm_set_enabled(pwm_gpio_to_slice_num(PWM_IN1), true);
  pwm_set_enabled(pwm_gpio_to_slice_num(PWM_IN2), true);
}

void motor_write(uint16_t value, bool direction) {
  if (direction) {
    // Backwards
    pwm_set_gpio_level(PWM_IN1, UINT16_MAX - value); // Invert value (two HIGH values apply braking)
    pwm_set_gpio_level(PWM_IN2, UINT16_MAX);
  } else {
    // Forwards
    pwm_set_gpio_level(PWM_IN1, UINT16_MAX);
    pwm_set_gpio_level(PWM_IN2, UINT16_MAX - value);
  }
}
