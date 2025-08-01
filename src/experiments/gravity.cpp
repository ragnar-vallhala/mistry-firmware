#include "experiments/gravity.h"
#include "display/display.h"
#include "experiments/kinematics.h"
#include "keypad/keypad.h"

void gravity_menu(void *param) {

  Display_Param *display_param = (Display_Param *)param;
  GFX *gfx = (GFX *)display_param->display->gfx;
  static int8_t gravity_index = 0;

  const char *gravity_items[] = {"Measure g", "Measure G"};
  static Task menus[] = {
      {g_experiment,
       display_param}, // You can replace this with actual tasks as needed
      {G_experiment, display_param},
  };
  const uint8_t gravity_length = 2;

  if (isKeyPressed(KEY_UP, display_param, millis())) {
    gravity_index--;
    if (gravity_index < 0)
      gravity_index = gravity_length - 1;
  } else if (isKeyPressed(KEY_DOWN, display_param, millis())) {
    gravity_index++;
    if (gravity_index >= gravity_length)
      gravity_index = 0;
  } else if (isKeyPressed(KEY_RIGHT, display_param, millis())) {
    display_param->current_menu = menus + gravity_index;
  } else if (isKeyPressed(KEY_LEFT, display_param, millis())) {
    display_param->current_menu = NULL;
  }

  // Determine the starting item index for visible gravity
  uint8_t start_index = 0;
  if (gravity_index >= VISIBLE_MENU_ITEMS)
    start_index = gravity_index - VISIBLE_MENU_ITEMS + 1;

  gfx->clear();
  draw_status_bar(gfx, "Kinematics Menu");

  for (uint8_t i = 0; i < VISIBLE_MENU_ITEMS; i++) {
    uint8_t item_index = start_index + i;
    if (item_index >= gravity_length)
      break;

    gfx->draw_string(0, 12 + i * 10,
                     std::string((item_index == gravity_index) ? "> " : " ") +
                         std::string(gravity_items[item_index]),
                     Colors::WHITE);
  }

  gfx->display();
}

void g_experiment(void *param) {
  Display_Param *dp = (Display_Param *)param;
  GFX *gfx = (GFX *)dp->display->gfx;

  static bool running = false;
  static bool result_shown = false;
  static float min_acc = 10000.0f; // very large number
  static uint32_t start_time = 0;

  bool button_state = gpio_get(BUTTON_PIN);
  static bool prev_button = false;

  // On button press → Start measuring
  if (button_state && !prev_button) {
    running = true;
    result_shown = false;
    min_acc = 10000.0f;
    start_time = millis();
  }

  prev_button = button_state;

  if (running) {
    float acc = get_acceleration(1); // cm/s²
    if (acc < min_acc)
      min_acc = acc;

    // Stop after 2 seconds of sampling
    if (millis() - start_time > 2000) {
      running = false;
      result_shown = true;
    }
  }

  // Display section
  gfx->clear();
  draw_status_bar(gfx, "Gravity");

  if (!running && !result_shown) {
    gfx->draw_string(0, 16, "Press Button to", Colors::WHITE);
    gfx->draw_string(0, 26, "Drop & Measure g", Colors::WHITE);
  }

  if (running) {
    gfx->draw_string(0, 20, "Measuring...", Colors::WHITE);
  }

  if (result_shown) {
    char buf[32];
    snprintf(buf, sizeof(buf), "Lowest acc: %.2f", min_acc);
    gfx->draw_string(0, 16, buf, Colors::WHITE);

    snprintf(buf, sizeof(buf), "~g = %.2f m/s2", min_acc / 100.0f);
    gfx->draw_string(0, 26, buf, Colors::WHITE);
  }

  gfx->display();

  // Return to menu
  if (isKeyPressed(KEY_LEFT, dp, millis())) {
    running = false;
    result_shown = false;
    min_acc = 10000.0f;
    static Task menu[] = {{gravity_menu, dp}};
    dp->current_menu = menu;
  }
}

void G_experiment(void *param) {
  Display_Param *dp = (Display_Param *)param;
  GFX *gfx = (GFX *)dp->display->gfx;

  static std::string input_buffer = "";
  static float mass = 0.0f;       // in kg (Earth's mass)
  static float radius = 0.0f;     // in meters (Earth's radius)
  static float g_measured = 0.0f; // m/s²
  static float G_computed = 0.0f; // m³/kg·s²

  static int input_phase = 0; // 0 = mass, 1 = radius
  static bool measured = false;
  static bool prev_button = false;

  bool button_state = gpio_get(BUTTON_PIN);

  if (input_phase >= 2 && button_state && !prev_button) {
    g_measured = get_acceleration(1) / 100.0f; // cm/s² → m/s²
    G_computed = (g_measured * radius * radius) / mass;
    measured = true;
  }

  prev_button = button_state;

  // Handle input
  if (input_phase < 2) {
    uint8_t num = get_num_pressed(dp);
    if (num != 0xFF) {
      if (num >= 1 && num <= 9) {
        input_buffer += ('0' + num);
      } else if (num == 10) {
        input_buffer += '.';
      } else if (num == 11) {
        input_buffer += '0';
      } else if (num == 12 && !input_buffer.empty()) {
        input_buffer.pop_back();
      }
    }

    // Confirm each input on RIGHT key
    if (isKeyPressed(KEY_RIGHT, dp, millis()) && !input_buffer.empty()) {
      float val = std::stof(input_buffer);
      if (input_phase == 0)
        mass = val;
      else
        radius = val;

      input_buffer.clear();
      input_phase++;
    }
  }

  // Display section
  gfx->clear();
  draw_status_bar(gfx, "Compute G");

  if (input_phase == 0) {
    gfx->draw_string(0, 16, "Enter Earth's mass", Colors::WHITE);
    gfx->draw_string(0, 26, "(kg): " + input_buffer, Colors::WHITE);
  } else if (input_phase == 1) {
    gfx->draw_string(0, 16, "Enter Earth's radius", Colors::WHITE);
    gfx->draw_string(0, 26, "(m): " + input_buffer, Colors::WHITE);
  } else if (!measured) {
    gfx->draw_string(0, 16, "Mass & Radius saved", Colors::WHITE);
    gfx->draw_string(0, 26, "Press button to", Colors::WHITE);
    gfx->draw_string(0, 36, "Measure g & calc G", Colors::WHITE);
  } else {
    char buf1[40], buf2[40], buf3[40], buf4[40];
    snprintf(buf1, sizeof(buf1), "M: %.3e kg", mass);
    snprintf(buf2, sizeof(buf2), "R: %.3e m", radius);
    snprintf(buf3, sizeof(buf3), "g: %.3f m/s2", g_measured);
    snprintf(buf4, sizeof(buf4), "G: %.3e m3/kg·s2", G_computed);

    gfx->draw_string(0, 16, buf1, Colors::WHITE);
    gfx->draw_string(0, 26, buf2, Colors::WHITE);
    gfx->draw_string(0, 36, buf3, Colors::WHITE);
    gfx->draw_string(0, 46, buf4, Colors::WHITE);
  }

  gfx->display();

  // Reset and go back
  if (isKeyPressed(KEY_LEFT, dp, millis())) {
    input_buffer.clear();
    mass = 0.0f;
    radius = 0.0f;
    g_measured = 0.0f;
    G_computed = 0.0f;
    input_phase = 0;
    measured = false;
    static Task menu[] = {{gravity_menu, dp}};
    dp->current_menu = menu;
  }
}
