#ifndef SETTING_CONFIG_H
#define SETTING_CONFIG_H
#include <stdbool.h>
#include <cstring>

typedef struct Setting_Config {
  float loadcell_scale;
  float loadcell_offset;
  float microphone_scale;
  float microphone_offset;
} Setting_Config;

void save_config(const Setting_Config &cfg);
Setting_Config load_config(bool force_read = false);
#endif // !SETTING_CONFIG_H
