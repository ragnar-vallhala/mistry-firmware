#include "experiments/kinematics/distance.h"
#include "display/SSD1306.h"
#include "display/display.h"
#include "experiments/kinematics.h"
#include "keypad/keypad.h"
#include "util.h"
#include <hardware/gpio.h>
#include <pico/time.h>
#include <stdint.h>

uint32_t pulseIn(uint pin, bool state, uint32_t timeout_us) {
  uint64_t start_time = time_us_64();
  uint64_t now;

  // Wait for any previous pulse to end
  while (gpio_get(pin) == state) {
    now = time_us_64();
    if ((now - start_time) >= timeout_us)
      return 0;
  }

  // Wait for the pulse to start
  while (gpio_get(pin) != state) {
    now = time_us_64();
    if ((now - start_time) >= timeout_us)
      return 0;
  }

  uint64_t pulse_start = time_us_64();

  // Wait for the pulse to end
  while (gpio_get(pin) == state) {
    now = time_us_64();
    if ((now - start_time) >= timeout_us)
      return 0;
  }

  return (uint32_t)(time_us_64() - pulse_start);
}

void distance_experiment(void *param) {
  Display_Param *dp = (Display_Param *)param;
  GFX *gfx = (GFX *)dp->display->gfx;
  static bool prev_button_state = false;
  static bool measured = false;
  static float distance = -1;
  bool current_button_state = gpio_get(BUTTON_PIN);
  if (current_button_state && current_button_state != prev_button_state) {
    distance = get_distance(5);
    measured = true;
  }
  prev_button_state = current_button_state;

  // Display the result
  gfx->clear();
  draw_status_bar(gfx, "Distance");

  if (!measured) {
    gfx->draw_string(0, 16, "Press Button to", Colors::WHITE);
    gfx->draw_string(0, 26, "Measure Distance", Colors::WHITE);
  }
  if (measured) {
    if (distance > 0) {
      gfx->draw_string(0, 15, std::to_string(distance) + std::string(" cm"));
    } else if (distance < 0) {
      gfx->draw_string(0, 15, std::string("Out of Range"));
    }
  }

  gfx->display();

  if (isKeyPressed(KEY_LEFT, dp, millis())) {
    // reset static variables
    prev_button_state = false;
    measured = false;
    distance = -1;
    static Task menus[] = {{kinematics_menu, dp}};
    dp->current_menu = menus; // Go back
  }
}
