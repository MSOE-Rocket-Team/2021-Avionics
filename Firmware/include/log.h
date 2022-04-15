#ifndef LOG_H
#define LOG_H

/**
 * Initialize logging and mount SD card
 * Consumes GPIO pins 3, 4, 5, 6, & 7
 * @return SD card status; see FRESULT in "ff.h"
 */
int log_init();

/**
 * Unmount SD card
 * @return SD card status; see FRESULT in "ff.h"
 */
int log_deinit();

/**
 * Write message to a file
 * @param filename File name
 * @param message Message to write to file
 * @return SD card status, or -1 if write fails; see FRESULT in "ff.h"
 */
int log_write(const char* filename, const char* message);

#endif // LOG_H
