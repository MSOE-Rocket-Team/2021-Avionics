#include "pico/printf.h"
#include "log.h"

FRESULT log_init() {
  FRESULT fr = pf_mount(&sd_fs);
  if (fr) return fr;
  return pf_open(FILENAME);
}

FRESULT log_close(uint* bw) {
  return pf_write(0, 0, bw);
}

FRESULT log_write(const char* buff, uint btw, uint* bw) {
  printf("WRITE: \"%.*s\"\n", btw, buff);
  return pf_write(buff, btw, bw);
}
