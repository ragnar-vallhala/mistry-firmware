#include "keypad/keypad.h"
#include "display/display.h"
#include "projdefs.h"
#include "util.h"
#include <hardware/gpio.h>
#include <stdio.h>

void init_keypad(void *params) {
  Display_Param *display_param = (Display_Param *)params;
  display_param->last_read = millis();

  for (uint8_t i = 0; i < 4; i++) {
    gpio_init(selectorPins[i]);
    gpio_set_dir(selectorPins[i], GPIO_OUT);
    gpio_put(selectorPins[i], 0);
  }

  // Serial.println("Keypad...");
  gpio_init(SIG_PIN);
  gpio_set_dir(SIG_PIN, GPIO_IN);
  gpio_pull_up(SIG_PIN);
  // Serial.println("Keypad Failed");
}

bool isKeyPressed(uint64_t key, Display_Param *dp, uint64_t now) {

  if ((now - dp->last_read) > DEBOUNCE_DELAY && dp->keypad_state == key) {
    dp->last_read = now;
    return true;
  }
  return false;
}

void read_keypad_states(void *params) {
  while (1) {

    Display_Param *display_param = (Display_Param *)params;

    uint16_t keyStates = 0;

    for (uint8_t channel = 0; channel < 16; channel++) {
      // Set selector pins
      for (uint8_t bit = 0; bit < 4; bit++) {
        gpio_put(selectorPins[bit], (channel >> bit) & 0x01);
        // digitalWrite(selectorPins[bit], (channel >> bit) & 0x01);
      }
      vTaskDelay(pdMS_TO_TICKS(1));
      // delayMicroseconds(5); // Settling time for MUX switching

      // Read the button: LOW means pressed (if using INPUT_PULLUP)
      bool pressed = gpio_get(SIG_PIN);
      // bool pressed = digitalRead(SIG_PIN) == HIGH;
      if (pressed) {
        keyStates |= (1 << channel);
      }
    }
    display_param->keypad_state = keyStates;
  }
}
