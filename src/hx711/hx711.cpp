#include "hx711/hx711.h"
#include <cstdint>
#include <hardware/gpio.h>
#include <hardware/timer.h>

HX711::HX711(int data_pin, int clock_pin)
    : data_pin(data_pin), clock_pin(clock_pin) {}

bool HX711::init() {
  gpio_init(data_pin);
  gpio_init(clock_pin);

  gpio_set_dir(data_pin, GPIO_IN);

  gpio_set_dir(clock_pin, GPIO_OUT);

  this->last_val = 0;
  return true;
}

bool HX711::is_ready() { return gpio_get(data_pin) == 0x0; }

int32_t HX711::read() {
  while (!is_ready())
    ;

  uint32_t val = 0;

  for (int i = 0; i < 24; i++) {
    gpio_put(clock_pin, true);
    val <<= 1;
    if (gpio_get(data_pin))
      val++;
    gpio_put(clock_pin, false);
  }
  busy_wait_us(64); // End session
  if (val == 0x7FFFFF || val == 0x800000)
    return last_val;
  if (val & 0x800000)  // If sign bit is set
    val |= 0xFF000000; // Sign-extend to 32 bits

  last_val = val;
  return last_val;
}
