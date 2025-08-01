#include "experiments/dynamics.h"
#include "display/GFX.h"
#include "display/display.h"
#include "experiments/dynamics/force.h"
#include "experiments/dynamics/weight.h"
#include "keypad/keypad.h"
#include "util.h"
#include <cstdint>

void dynamics_menu(void *param) {

  // Serial.println("Kinematics");
  Display_Param *display_param = (Display_Param *)param;
  GFX *gfx = (GFX *)display_param->display->gfx;

  static uint8_t dynamics_index = 0;

  const char *dynamics_items[] = {"Weight", "Force"};
  static Task menus[] = {
      {weight_experiment,
       display_param}, // You can replace this with actual tasks as needed
      {force_experiment, display_param},
  };

  const uint8_t dynamics_length = 2;

  if (isKeyPressed(KEY_UP, display_param, millis())) {
    dynamics_index--;
    if (dynamics_index < 0)
      dynamics_index = dynamics_length - 1;
  } else if (isKeyPressed(KEY_DOWN, display_param, millis())) {
    dynamics_index++;
    if (dynamics_index >= dynamics_length)
      dynamics_index = 0;
  } else if (isKeyPressed(KEY_RIGHT, display_param, millis())) {
    display_param->current_menu = menus + dynamics_index;
  } else if (isKeyPressed(KEY_LEFT, display_param, millis())) {
    display_param->current_menu = NULL;
  }

  // Determine the starting item index for visible dynamics
  uint8_t start_index = 0;
  if (dynamics_index >= VISIBLE_MENU_ITEMS)
    start_index = dynamics_index - VISIBLE_MENU_ITEMS + 1;

  gfx->clear();
  draw_status_bar(gfx, "Kinematics Menu");

  for (uint8_t i = 0; i < VISIBLE_MENU_ITEMS; i++) {
    uint8_t item_index = start_index + i;
    if (item_index >= dynamics_length)
      break;

    gfx->draw_string(0, 12 + i * 10,
                     std::string((item_index == dynamics_index) ? "> " : " ") +
                         std::string(dynamics_items[item_index]),
                     Colors::WHITE);
  }

  gfx->display();
}
