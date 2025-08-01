#include "experiments/dynamics/force.h"
#include "display/GFX.h"
#include "display/display.h"
#include "experiments/dynamics.h"
#include "hx711/hx711.h"
#include "keypad/keypad.h"
#include "setting/config.h"
#include "util.h"
#include <cstdio>
#include <hardware/gpio.h>

void force_experiment(void *param) {
  Display_Param *dp = (Display_Param *)param;
  GFX *gfx = (GFX *)dp->display->gfx;

  static HX711 hx711(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  static bool initialized = false;
  static bool measured = false;
  static float force = 0;

  static bool prev_button_state = false;
  bool button_state = gpio_get(BUTTON_PIN);

  if (!initialized) {
    hx711.init();
    initialized = true;
  }

  if (button_state && !prev_button_state) {
    if (hx711.is_ready()) {
      int32_t raw = hx711.read();

      Setting_Config cfg = load_config();
      float scale = cfg.loadcell_scale;
      float offset = cfg.loadcell_offset;

      float weight = (raw - offset) / scale; // in kg
      force = weight * 9.8f;                 // in N (Newtons)
      measured = true;
    }
  }

  prev_button_state = button_state;

  // Display section
  gfx->clear();
  draw_status_bar(gfx, "Force");

  if (!measured) {
    gfx->draw_string(0, 16, "Press Button to", Colors::WHITE);
    gfx->draw_string(0, 26, "Measure Force", Colors::WHITE);
  } else {
    char buf[32];
    snprintf(buf, sizeof(buf), "%.2f N", force);
    gfx->draw_string(0, 20, buf, Colors::WHITE);
  }

  gfx->display();

  // Exit
  if (isKeyPressed(KEY_LEFT, dp, millis())) {
    measured = false;
    force = 0;
    prev_button_state = false;
    static Task menu[] = {{dynamics_menu, dp}};
    dp->current_menu = menu;
  }
}
