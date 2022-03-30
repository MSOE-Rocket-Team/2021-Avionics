#include "pico/stdlib.h"
#include "pico/printf.h"

#include "imu.h"
#include "motor.h"
#include "pid.h"

int main() {
  // Consumes GPIO pins 14 & 15
  motor_init();
  // Consumes GPIO pins 9, 10, & 11
  imu_init();

  #ifndef PICO_DEFAULT_LED_PIN
    #warning blink example requires a board with a regular LED
  #else
    const uint LED_PIN = PICO_DEFAULT_LED_PIN;
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    stdio_init_all();

    pid *pid = NULL;
    pid_init(pid, 1, 5, 4);

    while (true) {
      gpio_put(LED_PIN, 1);
      sleep_ms(500);
      gpio_put(LED_PIN, 0);
      inertial_measurement_t im;
      imu_read(&im);
      printf("IMU reading: gyro { %d %d %d } accel { %d %d %d } ts { %d }",
             im.gyro[0], im.gyro[1], im.gyro[2],
             im.accel[0], im.accel[1], im.accel[2],
             im.timestamp);
      sleep_ms(500);
    }
  #endif
}
