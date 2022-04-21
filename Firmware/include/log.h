#ifndef LOG_H
#define LOG_H

#include "pico/printf.h"
#include "pff.h"

#define FILENAME "log.txt"

static FATFS sd_fs;  // SD file system object

/**
 * Initialize logging and mount SD card
 * Consumes GPIO pins 3, 4, 5, 6, & 7
 * @return SD card status
 */
FRESULT log_init();

/**
 * Close log file
 * @param bw Pointer to the number of bytes written
 * @return SD card status
 */
FRESULT log_close(uint* bw);

 /**
  * Write buffer to a file
  * @param buff Pointer to the data to be written
  * @param btw Number of bytes to write
  * @param bw Pointer to the number of bytes written
  * @return SD card status
  */
FRESULT log_write(const char* buff, uint btw, uint* bw);

#endif // LOG_H
