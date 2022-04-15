#include "sd.h"
#include "log.h"

int log_init(const char* filename) {
    sd_init();
    return 0;
}

int log_deinit() {
    return 0;
}

int log_write(const char* filename, const char* message) {
    return 0;
}
