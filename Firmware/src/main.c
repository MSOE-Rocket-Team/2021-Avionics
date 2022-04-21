#include "pff.h"
#include "pico/stdlib.h"
#include "pico/printf.h"

#include "imu.h"
#include "log.h"
#include "motor.h"
#include "pid.h"

void die(FRESULT fr) {
  uint bw;
  log_close(&bw);
  for (;;) {
    switch (fr) {
      case FR_OK:
        printf("Something is definitely wrong.\n");
        break;
      case FR_DISK_ERR:
        printf("The function failed due to a hard error in the disk function, "
               "wrong FAT structure, or an internal error.\n");
        break;
      case FR_NOT_READY:
        printf("The storage device could not be initialized due to a hard error or no medium.\n");
        break;
      case FR_NO_FILE:
        printf("Could not find the file or path.\n");
        break;
      case FR_NOT_OPENED:
        printf("The file has not been opened.\n");
        break;
      case FR_NOT_ENABLED:
        printf("The volume has not been mounted.\n");
      case FR_NO_FILESYSTEM:
        printf("There is no valid FAT partition on the drive.\n");
        break;
      default:
        printf("An unknown error occurred during file write.\n");
        break;
    }
    sleep_ms(5000);
  }
}

int main() {
  stdio_init_all();
  sleep_ms(5000);
  printf("Sleeping 10s\n");
  sleep_ms(10000);

  // Consumes GPIO pins 14 & 15
//  motor_init();
  // Consumes GPIO pins 9, 10, & 11
//  imu_init();
  // Consumes GPIO pins 3, 4, 5, 6, & 7
  FRESULT fr = log_init();
  if (fr) die(fr);

//  pid *pid = NULL;
//  pid_init(pid, 1, 5, 4);

  while (true) {
//    inertial_measurement_t im;
//    imu_read_immediate(&im);
//    printf("IMU reading: gyro { %d %d %d } accel { %d %d %d } ts { %lu }\n",
//        im.gyro[0], im.gyro[1], im.gyro[2],
//        im.accel[0], im.accel[1], im.accel[2],
//        im.timestamp);

    uint bw;
    fr = log_write("Hello, World!\r\n", 16, &bw);
    fr ? die(fr) : printf("%d bytes written", bw);
    sleep_ms(10000);

//    sleep_ms(1000);
//    motor_write(16384, 0);
//    sleep_ms(1000);
//    motor_write(16384, 1);
  }
}
