#include "experiments/kinematics/acceleration.h"
#include "display/SSD1306.h"
#include "display/display.h"
#include "experiments/kinematics.h"
#include "keypad/keypad.h"
#include "util.h"
#include <hardware/gpio.h>
#include <pico/time.h>
#include <stdint.h>

void accelration_experiment(void *param) {
  Display_Param *dp = (Display_Param *)param;
  GFX *gfx = (GFX *)dp->display->gfx;

  static float acc = -1;
  static bool prev_button_state = false;
  static bool measured = false;

  bool button_state = gpio_get(BUTTON_PIN);

  if (button_state && !prev_button_state) {
    acc = get_acceleration(1);
    measured = true;
  }

  prev_button_state = button_state;

  // Display Section
  gfx->clear();
  draw_status_bar(gfx, "Acceleration");

  if (!measured) {
    gfx->draw_string(0, 16, "Press Button to", Colors::WHITE);
    gfx->draw_string(0, 26, "Measure Acceleration", Colors::WHITE);
  }

  if (measured) {
    gfx->draw_string(0, 15, std::to_string(acc) + std::string(" cm/s2"));
  }

  gfx->display();

  if (isKeyPressed(KEY_LEFT, dp, millis())) {
    prev_button_state = false;
    measured = false;
    acc = -1;
    static Task menus[] = {{kinematics_menu, dp}};
    dp->current_menu = menus;
  }
}
