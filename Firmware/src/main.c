#include "pico/stdlib.h"

#include "pid.h"
#include "pico/printf.h"

int main() {
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
      sleep_ms(1000);
      gpio_put(LED_PIN, 0);
      printf("Hello, World!\n");
      sleep_ms(1000);
    }
  #endif
}
