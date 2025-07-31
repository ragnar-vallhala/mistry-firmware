#include "keypad/keypad.h"
#include "display/display.h"
#include "projdefs.h"
#include "util.h"
#include <cmath>
#include <cstdint>
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
  gpio_pull_down(SIG_PIN);
  // Serial.println("Keypad Failed");
}

bool isKeyPressed(uint64_t key, Display_Param *dp, uint64_t now) {
  bool result = false;
  if ((now - dp->last_read) > DEBOUNCE_DELAY && dp->keypad_state == key) {
    dp->last_read = now;
    result = true;
  }
  return result;
}

void read_keypad_states(void *params) {
  while (1) {

    Display_Param *display_param = (Display_Param *)params;

    uint16_t keyStates = 0;

    for (uint8_t channel = 0; channel < 16; channel++) {
      // Set selector pins
      for (uint8_t bit = 0; bit < 4; bit++) {
        gpio_put(selectorPins[bit], (channel >> bit) & 0x01);
      }
      vTaskDelay(pdMS_TO_TICKS(1));
      bool pressed = gpio_get(SIG_PIN);
      if (pressed) {
        keyStates |= (1 << channel);
      }
    }
    display_param->keypad_state = keyStates;

    printf("Called keypad: %d\n", display_param->keypad_state);
  }
}

uint8_t get_num_pressed(void *params) {
  Display_Param *disp = (Display_Param *)params;
  if (isKeyPressed(KEY_0, disp, millis()))
    return 11; // physical mapping
  else if (isKeyPressed(KEY_1, disp, millis()))
    return 1;
  else if (isKeyPressed(KEY_2, disp, millis()))
    return 2;
  else if (isKeyPressed(KEY_3, disp, millis()))
    return 3;
  else if (isKeyPressed(KEY_4, disp, millis()))
    return 4;
  else if (isKeyPressed(KEY_5, disp, millis()))
    return 5;
  else if (isKeyPressed(KEY_6, disp, millis()))
    return 6;
  else if (isKeyPressed(KEY_7, disp, millis()))
    return 7;
  else if (isKeyPressed(KEY_8, disp, millis()))
    return 8;
  else if (isKeyPressed(KEY_9, disp, millis()))
    return 9;
  else if (isKeyPressed(KEY_DOT, disp, millis()))
    return 10;
  else if (isKeyPressed(KEY_BACKSPACE, disp, millis()))
    return 12;
  return 0xff;
}
