#include "pico/stdlib.h"
#include "pico/printf.h"

#include "imu.h"
#include "motor.h"
#include "pid.h"

int main() {
  stdio_init_all();

  // Consumes GPIO pins 14 & 15
//  motor_init();
  // Consumes GPIO pins 9, 10, & 11
  imu_init();

//  pid *pid = NULL;
//  pid_init(pid, 1, 5, 4);

  while (true) {
    inertial_measurement_t im;
    imu_read_immediate(&im);
    printf("IMU reading: gyro { %d %d %d } accel { %d %d %d } ts { %lu }\n",
        im.gyro[0], im.gyro[1], im.gyro[2],
        im.accel[0], im.accel[1], im.accel[2],
        im.timestamp);

    sleep_ms(1000);
//    motor_write(16384, 0);
//    sleep_ms(1000);
//    motor_write(16384, 1);
  }
}
