#include "experiments/energy.h"
#include "display/GFX.h"
#include "display/display.h"
#include "experiments/fluid.h"
#include "experiments/kinematics.h"
#include "hx711/hx711.h"
#include "keypad/keypad.h"
#include "setting/config.h"
#include "util.h"

#include <cstdio>
#include <hardware/gpio.h>

void energy_menu(void *param) {
  Display_Param *dp = (Display_Param *)param;
  GFX *gfx = (GFX *)dp->display->gfx;

  static int8_t index = 0;
  const char *items[] = {"Kinetic Energy", "Potential Energy"};
  static Task menus[] = {
      {kinetic_energy_experiment, dp},
      {potential_energy_experiment, dp},
  };
  const uint8_t len = 2;

  if (isKeyPressed(KEY_UP, dp, millis())) {
    index = (index - 1 + len) % len;
  } else if (isKeyPressed(KEY_DOWN, dp, millis())) {
    index = (index + 1) % len;
  } else if (isKeyPressed(KEY_RIGHT, dp, millis())) {
    dp->current_menu = menus + index;
  } else if (isKeyPressed(KEY_LEFT, dp, millis())) {
    dp->current_menu = NULL;
  }

  gfx->clear();
  draw_status_bar(gfx, "Energy Menu");

  for (uint8_t i = 0; i < VISIBLE_MENU_ITEMS; i++) {
    if (i >= len)
      break;
    gfx->draw_string(0, 12 + i * 10,
                     std::string((i == index) ? "> " : " ") + items[i],
                     Colors::WHITE);
  }

  gfx->display();
}

void kinetic_energy_experiment(void *param) {
  Display_Param *dp = (Display_Param *)param;
  GFX *gfx = (GFX *)dp->display->gfx;

  static float velocity = 0.0f;
  static float mass = 1.0f;
  static float ke = 0.0f;

  static std::string input_buffer = "";
  static bool mass_entered = false;
  static bool measured = false;
  static bool prev_button = false;
  bool button = gpio_get(BUTTON_PIN);

  gfx->clear();
  draw_status_bar(gfx, "Kinetic Energy");

  if (!mass_entered) {
    // Mass entry phase using number input
    gfx->draw_string(0, 16, "Enter mass (kg):", Colors::WHITE);
    gfx->draw_string(0, 26, input_buffer, Colors::WHITE);

    uint8_t num = get_num_pressed(dp);
    if (num != 0xFF) {
      if (num >= 1 && num <= 9)
        input_buffer += ('0' + num);
      else if (num == 10)
        input_buffer += '.';
      else if (num == 11)
        input_buffer += '0';
      else if (num == 12 && !input_buffer.empty())
        input_buffer.pop_back();
    }

    if (isKeyPressed(KEY_RIGHT, dp, millis()) && !input_buffer.empty()) {
      mass = std::stof(input_buffer);
      input_buffer.clear();
      mass_entered = true;
    }
  } else if (!measured) {
    gfx->draw_string(0, 16, "Press button to", Colors::WHITE);
    gfx->draw_string(0, 26, "Measure velocity", Colors::WHITE);
    if (button && !prev_button) {
      velocity = get_velocity(1); // m/s
      ke = 0.5f * mass * velocity * velocity;
      measured = true;
    }
  } else {
    char buf1[32], buf2[32], buf3[32];
    snprintf(buf1, sizeof(buf1), "Mass: %.2f kg", mass);
    snprintf(buf2, sizeof(buf2), "Velocity: %.2f m/s", velocity);
    snprintf(buf3, sizeof(buf3), "KE: %.2f J", ke);
    gfx->draw_string(0, 16, buf1, Colors::WHITE);
    gfx->draw_string(0, 26, buf2, Colors::WHITE);
    gfx->draw_string(0, 36, buf3, Colors::WHITE);
  }

  gfx->display();
  prev_button = button;

  if (isKeyPressed(KEY_LEFT, dp, millis()) && measured) {
    // Reset state
    measured = false;
    mass_entered = false;
    velocity = 0.0f;
    ke = 0.0f;
    input_buffer.clear();
    static Task menu[] = {{energy_menu, dp}};
    dp->current_menu = menu;
  }
}

void potential_energy_experiment(void *param) {
  Display_Param *dp = (Display_Param *)param;
  GFX *gfx = (GFX *)dp->display->gfx;

  static HX711 hx711(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  static bool initialized = false;
  static float height = 1.0f;
  static float mass = 0.0f;
  static float pe = 0.0f;

  static bool measured = false;
  static bool height_entered = false;
  static std::string input_buffer = "";
  static bool prev_button = false;
  bool button = gpio_get(BUTTON_PIN);

  if (!initialized) {
    hx711.init();
    initialized = true;
  }

  gfx->clear();
  draw_status_bar(gfx, "Potential Energy");

  if (!height_entered) {
    gfx->draw_string(0, 16, "Enter height (m):", Colors::WHITE);
    gfx->draw_string(0, 26, input_buffer, Colors::WHITE);

    uint8_t num = get_num_pressed(dp);
    if (num != 0xFF) {
      if (num >= 1 && num <= 9)
        input_buffer += ('0' + num);
      else if (num == 10)
        input_buffer += '.';
      else if (num == 11)
        input_buffer += '0';
      else if (num == 12 && !input_buffer.empty())
        input_buffer.pop_back();
    }

    if (isKeyPressed(KEY_RIGHT, dp, millis()) && !input_buffer.empty()) {
      height = std::stof(input_buffer);
      input_buffer.clear();
      height_entered = true;
    }
  } else if (!measured) {
    gfx->draw_string(0, 16, "Place object on scale", Colors::WHITE);
    gfx->draw_string(0, 26, "Press button", Colors::WHITE);

    if (button && !prev_button && hx711.is_ready()) {
      int32_t raw = hx711.read();
      Setting_Config cfg = load_config();
      mass = (raw - cfg.loadcell_offset) * cfg.loadcell_scale;
      pe = mass * 9.8f * height;
      measured = true;
    }
  } else {
    char buf1[32], buf2[32], buf3[32];
    snprintf(buf1, sizeof(buf1), "Mass: %.2f kg", mass);
    snprintf(buf2, sizeof(buf2), "Height: %.2f m", height);
    snprintf(buf3, sizeof(buf3), "PE: %.2f J", pe);

    gfx->draw_string(0, 12, buf1, Colors::WHITE);
    gfx->draw_string(0, 22, buf2, Colors::WHITE);
    gfx->draw_string(0, 32, buf3, Colors::WHITE);
  }

  gfx->display();
  prev_button = button;

  if (isKeyPressed(KEY_LEFT, dp, millis()) && measured) {
    measured = false;
    height_entered = false;
    input_buffer.clear();
    static Task menu[] = {{energy_menu, dp}};
    dp->current_menu = menu;
  }
}
