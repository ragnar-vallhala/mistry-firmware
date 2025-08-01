#include "experiments/dynamics/weight.h"
#include "display/display.h"
#include "display/GFX.h"
#include "experiments/dynamics.h"
#include "keypad/keypad.h"
#include "setting/config.h"
#include "util.h"
#include <hardware/gpio.h>
#include <cstdio>
#include "hx711/hx711.h"


void weight_experiment(void *param) {
  Display_Param *dp = (Display_Param *)param;
  GFX *gfx = (GFX *)dp->display->gfx;

  static HX711 hx711(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  static bool initialized = false;
  static bool measured = false;
  static float weight = 0;

  static bool prev_button_state = false;
  bool button_state = gpio_get(BUTTON_PIN);

  if (!initialized) {
    hx711.init();
    initialized = true;
  }

  if (button_state && !prev_button_state) {
    if (hx711.is_ready()) {
      int32_t raw = hx711.read();

      // Get scale and offset from saved config
      Setting_Config cfg = load_config();
      float scale = cfg.loadcell_scale;
      float offset = cfg.loadcell_offset;

      weight = (raw - offset) / scale;
      measured = true;
    }
  }

  prev_button_state = button_state;

  // UI
  gfx->clear();
  draw_status_bar(gfx, "Weight");

  if (!measured) {
    gfx->draw_string(0, 16, "Press Button to", Colors::WHITE);
    gfx->draw_string(0, 26, "Measure Weight", Colors::WHITE);
  } else {
    char buf[32];
    snprintf(buf, sizeof(buf), "%.2f kg", weight);
    gfx->draw_string(0, 20, buf, Colors::WHITE);
  }

  gfx->display();

  // Return to previous menu
  if (isKeyPressed(KEY_LEFT, dp, millis())) {
    measured = false;
    weight = 0;
    prev_button_state = false;
    static Task menu[] = {{dynamics_menu, dp}};
    dp->current_menu = menu;
  }
}

