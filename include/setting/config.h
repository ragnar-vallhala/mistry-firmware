#ifndef SETTING_CONFIG_H
#define SETTING_CONFIG_H
#include <cstring>
#include <stdbool.h>

#define MAGIC_NUMBER 0x2808

typedef struct Setting_Config {
  int magic_number;
  float loadcell_scale;
  float loadcell_offset;
  float microphone_scale;
  float microphone_offset;
} Setting_Config;

void save_config(const Setting_Config &cfg, void* dispConfig);
Setting_Config load_config(bool force_read = false);
#endif // !SETTING_CONFIG_H
