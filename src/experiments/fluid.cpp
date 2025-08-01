#include "experiments/fluid.h"
#include "display/display.h"
#include "hx711/hx711.h"
#include "keypad/keypad.h"
#include "setting/config.h"

HX711 loadcell(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);

void fluid_menu(void *param) {
  Display_Param *dp = (Display_Param *)param;
  GFX *gfx = (GFX *)dp->display->gfx;

  static int8_t fluid_index = 0;
  const char *items[] = {"Buoyancy", "Density"};
  static Task menus[] = {
      {buyoancy_experiment, dp},
      {density_experiment, dp},
  };
  const uint8_t len = 2;

  if (isKeyPressed(KEY_UP, dp, millis())) {
    fluid_index--;
    if (fluid_index < 0)
      fluid_index = len - 1;
  } else if (isKeyPressed(KEY_DOWN, dp, millis())) {
    fluid_index++;
    if (fluid_index >= len)
      fluid_index = 0;
  } else if (isKeyPressed(KEY_RIGHT, dp, millis())) {
    dp->current_menu = menus + fluid_index;
  } else if (isKeyPressed(KEY_LEFT, dp, millis())) {
    dp->current_menu = NULL;
  }

  gfx->clear();
  draw_status_bar(gfx, "Fluid Menu");

  for (uint8_t i = 0; i < VISIBLE_MENU_ITEMS; i++) {
    uint8_t idx = i;
    if (idx >= len)
      break;
    gfx->draw_string(0, 12 + i * 10,
                     std::string((idx == fluid_index) ? "> " : " ") +
                         items[idx],
                     Colors::WHITE);
  }

  gfx->display();
}

void buyoancy_experiment(void *param) {
  Display_Param *dp = (Display_Param *)param;
  GFX *gfx = (GFX *)dp->display->gfx;

  static HX711 hx(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  static Setting_Config cfg = load_config();

  static float w_air = 0.0f;
  static float w_fluid = 0.0f;
  static bool air_done = false;
  static bool fluid_done = false;
  static bool prev_button = false;
  bool button = gpio_get(BUTTON_PIN);

  static bool initialized = false;
  if (!initialized) {
    hx.init();
    initialized = true;
  }
  if (button && !prev_button && hx.is_ready()) {
    int32_t raw = hx.read();
    float mass = (raw - cfg.loadcell_offset) * cfg.loadcell_scale;

    if (!air_done) {
      w_air = mass;
      air_done = true;
    } else if (!fluid_done) {
      w_fluid = mass;
      fluid_done = true;
    }
  }

  prev_button = button;

  gfx->clear();
  draw_status_bar(gfx, "Buoyancy");

  if (!air_done) {
    gfx->draw_string(0, 16, "Press to weigh", Colors::WHITE);
    gfx->draw_string(0, 26, "in air", Colors::WHITE);
  } else if (!fluid_done) {
    char msg[32];
    snprintf(msg, sizeof(msg), "Air weight: %.2f g", w_air * 1000);
    gfx->draw_string(0, 16, msg, Colors::WHITE);
    gfx->draw_string(0, 26, "Now weigh in fluid", Colors::WHITE);
  } else {
    float buoyant_force = (w_air - w_fluid) * 9.8f;
    char buf1[32], buf2[32], buf3[32];
    snprintf(buf1, sizeof(buf1), "Air: %.2f g", w_air * 1000);
    snprintf(buf2, sizeof(buf2), "Fluid: %.2f g", w_fluid * 1000);
    snprintf(buf3, sizeof(buf3), "Buoyancy: %.2f N", buoyant_force);

    gfx->draw_string(0, 12, buf1, Colors::WHITE);
    gfx->draw_string(0, 22, buf2, Colors::WHITE);
    gfx->draw_string(0, 32, buf3, Colors::WHITE);
  }

  gfx->display();

  if (isKeyPressed(KEY_LEFT, dp, millis())) {
    w_air = w_fluid = 0;
    air_done = fluid_done = false;
    static Task menu[] = {{fluid_menu, dp}};
    dp->current_menu = menu;
  }
}

void density_experiment(void *param) {
  Display_Param *dp = (Display_Param *)param;
  GFX *gfx = (GFX *)dp->display->gfx;

  static HX711 hx(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  static Setting_Config cfg = load_config();

  static float w_water = 0.0f;
  static float w_fluid = 0.0f;
  static bool water_done = false;
  static bool fluid_done = false;

  static bool prev_button = false;
  bool button = gpio_get(BUTTON_PIN);

  static bool initialized = false;
  if (!initialized) {
    hx.init();
    initialized = true;
  }
  if (button && !prev_button && hx.is_ready()) {
    int32_t raw = hx.read();
    float mass = (raw - cfg.loadcell_offset) * cfg.loadcell_scale;

    if (!water_done) {
      w_water = mass;
      water_done = true;
    } else if (!fluid_done) {
      w_fluid = mass;
      fluid_done = true;
    }
  }

  prev_button = button;

  gfx->clear();
  draw_status_bar(gfx, "Density");

  if (!water_done) {
    gfx->draw_string(0, 16, "Weigh in water", Colors::WHITE);
  } else if (!fluid_done) {
    char msg[32];
    snprintf(msg, sizeof(msg), "Water: %.2f g", w_water * 1000);
    gfx->draw_string(0, 16, msg, Colors::WHITE);
    gfx->draw_string(0, 26, "Now in fluid", Colors::WHITE);
  } else {
    // Assume ρ_water = 1 g/cm³
    float density = (w_fluid / w_water); // g/cm³

    char buf1[32], buf2[32], buf3[32];
    snprintf(buf1, sizeof(buf1), "Water: %.2f g", w_water * 1000);
    snprintf(buf2, sizeof(buf2), "Fluid: %.2f g", w_fluid * 1000);
    snprintf(buf3, sizeof(buf3), "Density: %.2f g/cm3", density);

    gfx->draw_string(0, 12, buf1, Colors::WHITE);
    gfx->draw_string(0, 22, buf2, Colors::WHITE);
    gfx->draw_string(0, 32, buf3, Colors::WHITE);
  }

  gfx->display();

  if (isKeyPressed(KEY_LEFT, dp, millis())) {
    w_water = w_fluid = 0;
    water_done = fluid_done = false;
    static Task menu[] = {{fluid_menu, dp}};
    dp->current_menu = menu;
  }
}
