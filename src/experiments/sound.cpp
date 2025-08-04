#include "experiments/sound.h"
#include "display/GFX.h"
#include "display/display.h"
#include "experiments/kinematics.h"
#include "keypad/keypad.h"
#include "microphone/microphone.h"
#include "setting/config.h"
#include "util.h"
#include <cstdio>
#include <hardware/gpio.h>

void sound_menu(void *param) {
  Display_Param *dp = (Display_Param *)param;
  GFX *gfx = (GFX *)dp->display->gfx;

  static int8_t sound_index = 0;
  const char *items[] = {"Speed of Sound", "Loudness"};
  static Task menus[] = {
      {sound_velocity_experiment, dp},
      {loudness_experiment, dp},
  };
  const uint8_t len = 2;

  if (isKeyPressed(KEY_UP, dp, millis())) {
    sound_index--;
    if (sound_index < 0)
      sound_index = len - 1;
  } else if (isKeyPressed(KEY_DOWN, dp, millis())) {
    sound_index++;
    if (sound_index >= len)
      sound_index = 0;
  } else if (isKeyPressed(KEY_RIGHT, dp, millis())) {
    dp->current_menu = menus + sound_index;
  } else if (isKeyPressed(KEY_LEFT, dp, millis())) {
    dp->current_menu = NULL;
  }

  gfx->clear();
  draw_status_bar(gfx, "Sound Menu");

  for (uint8_t i = 0; i < VISIBLE_MENU_ITEMS; i++) {
    uint8_t idx = i;
    if (idx >= len)
      break;
    gfx->draw_string(0, 12 + i * 10,
                     std::string((idx == sound_index) ? "> " : " ") +
                         items[idx],
                     Colors::WHITE);
  }

  gfx->display();
}

void sound_velocity_experiment(void *param) {
  Display_Param *dp = (Display_Param *)param;
  GFX *gfx = (GFX *)dp->display->gfx;

  static bool done = false;
  static float distance_cm = 15.0f; // Assume user placed object at 1m
  static float measured_time = 0.0f;
  static float speed = 0.0f;
  static bool prev_button = false;
  static bool intialized = false;
  if (!intialized) {
    microphone_init();
  }
  bool button = gpio_get(BUTTON_PIN);

  gfx->clear();
  draw_status_bar(gfx, "Speed of Sound");

  if (!done) {
    gfx->draw_string(0, 16, "Place object at 15cm", Colors::WHITE);
    gfx->draw_string(0, 26, "Press button to", Colors::WHITE);
    gfx->draw_string(0, 36, "measure time", Colors::WHITE);
  } else {
    char buf1[32], buf2[32];
    snprintf(buf1, sizeof(buf1), "Time: %.2f ms", measured_time * 1000);
    snprintf(buf2, sizeof(buf2), "Speed: %.2f m/s", speed);
    gfx->draw_string(0, 16, buf1, Colors::WHITE);
    gfx->draw_string(0, 26, buf2, Colors::WHITE);
  }

  gfx->display();

  if (button && !prev_button) {
    uint64_t t1 = time_us_64();
    float dist = get_distance(5);
    uint64_t t2 = time_us_64(); // each have 5ms delay

    measured_time = (t2 - t1 - 5000 * 5) / 2e6f;
    speed = (dist / 15.0f) / measured_time;
    done = true;
  }

  prev_button = button;

  if (isKeyPressed(KEY_LEFT, dp, millis())) {
    done = false;
    static Task menu[] = {{sound_menu, dp}};
    dp->current_menu = menu;
  }
}

void loudness_experiment(void *param) {
  Display_Param *dp = (Display_Param *)param;
  GFX *gfx = (GFX *)dp->display->gfx;

  static bool initialized = false;
  if (!initialized) {
    microphone_init();
    initialized = true;
  }

  Setting_Config cfg = load_config();
  float scale = cfg.microphone_scale;
  float offset = cfg.microphone_offset;

  float raw = read_decible();
  float loudness = (raw - offset) * scale;

  gfx->clear();
  draw_status_bar(gfx, "Loudness");

  char buf[32];
  snprintf(buf, sizeof(buf), "Loudness: %.2f dB", loudness);
  gfx->draw_string(0, 16, buf, Colors::WHITE);
  gfx->draw_string(0, 28, "Speak louder!", Colors::WHITE);
  gfx->display();

  if (isKeyPressed(KEY_LEFT, dp, millis())) {
    static Task menu[] = {{sound_menu, dp}};
    dp->current_menu = menu;
  }

  sleep_ms(250); // sampling delay
}
