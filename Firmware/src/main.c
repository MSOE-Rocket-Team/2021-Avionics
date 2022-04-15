#include "pico/stdlib.h"
#include "pico/printf.h"

#include "f_util.h"
#include "ff.h"

#include "imu.h"
#include "log.h"
#include "motor.h"
#include "pid.h"

int main() {
  stdio_init_all();

  // Consumes GPIO pins 14 & 15
//  motor_init();
  // Consumes GPIO pins 9, 10, & 11
//  imu_init();
  // Consumes GPIO pins 3, 4, 5, 6, & 7
  int fr = log_init();

  if (fr != FR_OK) {
    while (true) { printf("Init error: %s (%d)\n", FRESULT_str(fr), fr); }
  }

//  pid *pid = NULL;
//  pid_init(pid, 1, 5, 4);

  while (true) {
//    inertial_measurement_t im;
//    imu_read_immediate(&im);
//    printf("IMU reading: gyro { %d %d %d } accel { %d %d %d } ts { %lu }\n",
//        im.gyro[0], im.gyro[1], im.gyro[2],
//        im.accel[0], im.accel[1], im.accel[2],
//        im.timestamp);

    fr = log_write("hello_world.txt", "Hello, World!\n");
    if (fr != FR_OK) {
        break;
    }
    sleep_ms(9000);

    sleep_ms(1000);
//    motor_write(16384, 0);
//    sleep_ms(1000);
//    motor_write(16384, 1);
  }

  if (fr != FR_OK) {
    if (fr == -1) {
      while (true) { printf("Write error\n"); }
    }
    while (true) { printf("Init error: %s (%d)\n", FRESULT_str(fr), fr); }
  }

  fr = log_deinit();
}
