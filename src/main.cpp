#include "display/GFX.h"
#include "hardware/i2c.h"
#include "pico/stdlib.h"
#include <pico/time.h>
#include "Config.h"


int main() {
  stdio_init_all();

    GFX oled(0x3C, Size::W128xH64, i2c0);
  while (true) {
    sleep_ms(1000);
    oled.clear(); // Clear buffer
    oled.draw_string(0, 0, "Robogen");
    oled.draw_string(0, 10, "Oled Example");
    oled.draw_string(0, 20, "Have fun!");
    oled.draw_progress_bar(0, oled.get_height() - 5, oled.get_width(), 5,
                         rand() % 100 + 1);

    oled.display(); // Send buffer to the screen
  }
  return 0;
}
