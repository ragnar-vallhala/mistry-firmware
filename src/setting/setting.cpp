#include "setting/setting.h"
#include "display/GFX.h"
#include "keypad/keypad.h"
#include "setting/config.h"
#include "util.h"
#include <cstdint>

#include "display/display.h"

void setting_menu(void *param) {
  Display_Param *disp = (Display_Param *)param;
  GFX *gfx = (GFX *)disp->display->gfx;

  static int8_t setting_index = 0;

  const char *setting_item[] = {"Loadcell", "Microphone", "About"};
  static Task menus[] = {
      {loadcell_setting, disp},
      {setting_menu, disp},
      {setting_menu, disp},
  };

  const uint8_t setting_length = 3;

  if (isKeyPressed(KEY_UP, disp, millis())) {
    setting_index--;
    if (setting_index < 0)
      setting_index = setting_length - 1;
  } else if (isKeyPressed(KEY_DOWN, disp, millis())) {
    setting_index++;
    if (setting_index >= setting_length)
      setting_index = 0;
  } else if (isKeyPressed(KEY_RIGHT, disp, millis())) {
    disp->current_menu = menus + setting_index;
  } else if (isKeyPressed(KEY_LEFT, disp, millis())) {
    disp->current_menu = NULL;
  }

  uint8_t start_index = 0;
  if (setting_index >= VISIBLE_MENU_ITEMS)
    start_index = setting_index - VISIBLE_MENU_ITEMS + 1;

  gfx->clear();
  draw_status_bar(gfx, "Settings");

  for (uint8_t i = 0; i < VISIBLE_MENU_ITEMS; i++) {
    uint8_t item_index = start_index + i;
    if (item_index >= setting_length)
      break;

    gfx->draw_string(0, 12 + i * 10,
                     std::string((item_index == setting_index) ? "> " : " ") +
                         std::string(setting_item[item_index]),
                     Colors::WHITE);
  }

  gfx->display();
}

void loadcell_setting(void *param) {
  Display_Param *disp = (Display_Param *)param;
  GFX *gfx = (GFX *)disp->display->gfx;
  static int selected_field = 0; // 0 = scale, 1 = offset
  static std::string input_buffer = "";

  static Setting_Config cfg = load_config();

  gfx->clear();
  draw_status_bar(gfx, "Loadcell");

  // Display current values
  gfx->draw_string(0, 12,
                   std::string((selected_field == 0) ? "> " : " ") +
                       "Scale: " + std::to_string(cfg.loadcell_scale),
                   Colors::WHITE);

  gfx->draw_string(0, 22,
                   std::string((selected_field == 1) ? "> " : " ") +
                       "Offset: " + std::to_string(cfg.loadcell_offset),
                   Colors::WHITE);

  // Show number being typed (if any)
  if (!input_buffer.empty()) {
    gfx->draw_string(0, 34, "Input: " + input_buffer, Colors::WHITE);
  }

  gfx->draw_string(0, 46, "LEFT: Back | RIGHT: Save", Colors::WHITE);
  gfx->draw_string(0, 56, "UP/DOWN: Select", Colors::WHITE);

  gfx->display();

  // Handle navigation
  if (isKeyPressed(KEY_UP, disp, millis())) {
    selected_field = (selected_field == 0) ? 1 : 0;
  } else if (isKeyPressed(KEY_DOWN, disp, millis())) {
    selected_field = (selected_field == 1) ? 0 : 1;
  } else if (isKeyPressed(KEY_LEFT, disp, millis())) {
    disp->current_menu = NULL;
    input_buffer.clear();
  } else if (isKeyPressed(KEY_RIGHT, disp, millis())) {
    // Save current buffer if any
    if (!input_buffer.empty()) {
      float val = std::stof(input_buffer);
      if (selected_field == 0)
        cfg.loadcell_scale = val;
      else
        cfg.loadcell_offset = val;

      save_config(cfg);
      input_buffer.clear();
    }
  }

  /* // Accept numbers from keypad */
  /* uint8_t num = get_num_pressed(disp); */
  /* if (num == 0xff) */
  /*   return; */
  /* if (num >= 1 && num <= 9) { */
  /*   input_buffer += ('0' + num); */
  /* } else if (num == 11) { */
  /*   input_buffer += '0'; */
  /* } else if (num == 10) { // Let's say 10 = '.' key if you support it */
  /*   input_buffer += '.'; */
  /* } else if (num == 12) { // 11 = backspace */
  /*   if (!input_buffer.empty()) */
  /*     input_buffer.pop_back(); */
  /* } */
}
void microphone_setting(void *param) {}
void about(void *param) {}
