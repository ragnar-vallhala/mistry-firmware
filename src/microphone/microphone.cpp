#include "microphone/microphone.h"

#include "hardware/adc.h"
#include "pico/stdlib.h"

void microphone_init() {
  adc_init();
  adc_gpio_init(27);
}

float read_decible() {
  adc_select_input(1);
  return adc_read();
}
