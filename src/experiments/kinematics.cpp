#include "experiments/kinematics.h"
#include "display/GFX.h"
#include "display/SSD1306.h"
#include "display/display.h"
#include "experiments/kinematics/acceleration.h"
#include "experiments/kinematics/distance.h"
#include "experiments/kinematics/velocity.h"
#include "keypad/keypad.h"
#include "util.h"
#include <pico/time.h>
#include <stdint.h>

float get_distance(uint8_t count) {
  static bool setup_done = false;
  if (!setup_done) {
    gpio_init(TRIG_PIN);
    gpio_init(ECHO_PIN);
    gpio_init(BUTTON_PIN);
    gpio_set_dir(TRIG_PIN, GPIO_OUT);
    gpio_set_dir(ECHO_PIN, GPIO_IN);
    gpio_set_dir(BUTTON_PIN, GPIO_IN);
    setup_done = true;
  }

  float durationSum = 0;

  for (int i = 0; i < count; i++) {

    // Trigger ultrasonic pulse
    gpio_put(TRIG_PIN, 0);
    sleep_us(2);
    gpio_put(TRIG_PIN, 1);
    sleep_us(10);
    gpio_put(TRIG_PIN, 0);

    long duration = pulseIn(ECHO_PIN, true, 30000); // timeout after 30ms
    if (duration < 0)
      count--;
    else
      durationSum += duration;
    sleep_ms(5);
  }
  float duration = durationSum / count;
  if (duration > 0) {
    return (duration / 2.0) * 0.0343;
  } else {
    return -1;
  }
}

float get_velocity(uint8_t count) {
  float velocitySum = 0;
  for (int i = 0; i < count; i++) {
    uint32_t time = millis();
    float distA = get_distance(count);
    sleep_ms(5);
    float distB = get_distance(count);
    time = millis() - time;
    velocitySum += ((distB - distA) * 1000.0f) / time;
  }
  velocitySum = velocitySum / count;
  return velocitySum;
}


float get_acceleration(uint8_t count) {
  float accSum = 0;
  for (int i = 0; i < count; i++) {
    uint32_t time = millis();
    float velA = get_velocity(count);
    sleep_ms(5);
    float velB = get_velocity(count);
    time = millis() - time;
    accSum += ((velB - velA) * 1000.0f) / time;
  }
  accSum = accSum / count;
  return accSum;
}


void kinematics_menu(void *param) {
  // Serial.println("Kinematics");
  Display_Param *display_param = (Display_Param *)param;
  GFX *gfx = (GFX *)display_param->display->gfx;
  static int8_t kinematics_index = 0;

  const char *kinematics_items[] = {
      "Distance",
      "Velocity",
      "Accelration",
  };
  static Task menus[] = {
      {distance_experiment,
       display_param}, // You can replace this with actual tasks as needed
      {velocity_experiment, display_param},
      {accelration_experiment, display_param},
  };
  const uint8_t kinematics_length = 3;

  if (isKeyPressed(KEY_UP, display_param, millis())) {
    kinematics_index--;
    if (kinematics_index < 0)
      kinematics_index = kinematics_length - 1;
  } else if (isKeyPressed(KEY_DOWN, display_param, millis())) {
    kinematics_index++;
    if (kinematics_index >= kinematics_length)
      kinematics_index = 0;
  } else if (isKeyPressed(KEY_RIGHT, display_param, millis())) {
    display_param->current_menu = menus + kinematics_index;
  } else if (isKeyPressed(KEY_LEFT, display_param, millis())) {
    display_param->current_menu = NULL;
  }

  // Determine the starting item index for visible kinematics
  uint8_t start_index = 0;
  if (kinematics_index >= VISIBLE_MENU_ITEMS)
    start_index = kinematics_index - VISIBLE_MENU_ITEMS + 1;

  gfx->clear();
  draw_status_bar(gfx, "Kinematics Menu");

  for (uint8_t i = 0; i < VISIBLE_MENU_ITEMS; i++) {
    uint8_t item_index = start_index + i;
    if (item_index >= kinematics_length)
      break;

    gfx->draw_string(0, 12 + i * 10,
                     std::string((item_index == kinematics_index) ? "> " : " ") +
                         std::string(kinematics_items[item_index]),
                     Colors::WHITE);
  }

  gfx->display();
}
