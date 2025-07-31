#include "battery.h"
#include "hardware/adc.h"
#include "microphone/microphone.h"
#include "pico/stdlib.h"
#include "stdio.h"
#include "util.h"
#include <cstdint>

uint8_t get_battery_percentage() {
  static bool battery_init = false;
  static float percent = -1;
  static uint64_t lastUpdate = millis();

  if (!battery_init) {
    adc_init();
    adc_gpio_init(26);
    battery_init = true;
  }

  adc_select_input(0);
  float raw = adc_read();
  raw = 4.0 * raw * 3.3f / 4095;
  raw -= 5.0;

  float val = raw / 3.3 * 100.0;
  if (percent == -1 || millis() - lastUpdate > 3000 || val > 100) {

    percent = val;
    uint8_t moded = percent;

    moded /= 10;
    moded *= 10;
    percent = moded;
    lastUpdate = millis();
  }
  return percent;
}
