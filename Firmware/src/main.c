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
  imu_init();
  // Consumes GPIO pins 3, 4, 5, 6, & 7
//  FRESULT fr = log_init();
//  if (fr) die(fr);

//  pid *pid = NULL;
//  pid_init(pid, 1, 5, 4);

  while (true) {
    inertial_measurement_t im;
    imu_read_immediate(&im);
    printf("IMU reading: gyro { %d %d %d } accel { %d %d %d } ts { %lu }\n",
        im.gyro[0], im.gyro[1], im.gyro[2],
        im.accel[0], im.accel[1], im.accel[2],
        im.timestamp);

//    uint bw;
//    fr = log_write("Hello, World!\r\n", 16, &bw);
//    fr ? die(fr) : printf("%d bytes written", bw);
//    sleep_ms(10000);

//    sleep_ms(1000);
//    motor_write(16384, 0);
//    sleep_ms(1000);
//    motor_write(16384, 1);
  }
}

/*

#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/i2c.h"

#define I2C1_SDA 10
#define I2C1_SCL 11

// I2C reserves some addresses for special purposes. We exclude these from the scan.
// These are any addresses of the form 000 0xxx or 111 1xxx
bool reserved_addr(uint8_t addr) {
return (addr & 0x78) == 0 || (addr & 0x78) == 0x78;
}

int main() {
// Enable UART so we can print status output
stdio_init_all();
sleep_ms(5000);
printf("Sleeping 10s\n");
sleep_ms(10000);

// This example will use I2C0 on the default SDA and SCL pins (GP4, GP5 on a Pico)
i2c_init(i2c1, 100 * 1000);
gpio_set_function(I2C1_SDA, GPIO_FUNC_I2C);
gpio_set_function(I2C1_SCL, GPIO_FUNC_I2C);
gpio_pull_up(I2C1_SDA);
gpio_pull_up(I2C1_SCL);

printf("\nI2C Bus Scan\n");
printf(" 0 1 2 3 4 5 6 7 8 9 A B C D E F\n");

for (int addr = 0; addr < (1 << 7); ++addr) {
if (addr % 16 == 0) {
printf("%02x ", addr);
}

// Perform a 1-byte dummy read from the probe address. If a slave
// acknowledges this address, the function returns the number of bytes
// transferred. If the address byte is ignored, the function returns
// -1.

// Skip over any reserved addresses.
int ret;
uint8_t rxdata;
if (reserved_addr(addr))
ret = PICO_ERROR_GENERIC;
else
ret = i2c_read_blocking(i2c1, addr, &rxdata, 1, false);

printf(ret < 0 ? "." : "@");
printf(addr % 16 == 15 ? "\n" : " ");
}
printf("Done.\n");
return 0;
}

*/