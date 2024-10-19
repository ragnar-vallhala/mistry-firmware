#include "pico/stdlib.h"

#define LED_PIN 1

int main() {
    stdio_init_all();

    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    int n = LED_PIN;
    while (1) {
        // Will never reach here
      gpio_put(LED_PIN,0);
      sleep_ms(1000);
      gpio_put(LED_PIN,1);
      sleep_ms(1000);
      
    }

    return 0;
}

