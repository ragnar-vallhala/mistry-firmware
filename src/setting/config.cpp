#include "setting/config.h"
#include "hardware/flash.h"
#include "hardware/sync.h"
#include "pico/stdlib.h"

#define FLASH_TARGET_OFFSET (PICO_FLASH_SIZE_BYTES - 4096) // Last 4KB
#define CONFIG_SIZE 256

static Setting_Config settings_config;

bool cmp_setting_config(const Setting_Config &cfg1,
                        const Setting_Config &cfg2) {

  return cfg1.loadcell_scale == cfg2.loadcell_scale &&
         cfg1.loadcell_offset == cfg2.loadcell_offset &&
         cfg1.microphone_scale == cfg2.microphone_scale &&
         cfg1.microphone_offset == cfg2.microphone_offset;
}

void save_config(const Setting_Config &cfg) {
  if (cmp_setting_config(cfg, settings_config))
    return;
  uint8_t buf[CONFIG_SIZE] = {0};
  memcpy(buf, &cfg, sizeof(Setting_Config));

  uint32_t ints = save_and_disable_interrupts();
  flash_range_erase(FLASH_TARGET_OFFSET, FLASH_SECTOR_SIZE);
  flash_range_program(FLASH_TARGET_OFFSET, buf, CONFIG_SIZE);
  restore_interrupts(ints);
}
Setting_Config load_config(bool force_read) {
  static bool read_once = false;
  if (read_once && !force_read)
    return settings_config;
  read_once = true;
  const uint8_t *flash_data = (const uint8_t *)(XIP_BASE + FLASH_TARGET_OFFSET);
  memcpy(&settings_config, flash_data, sizeof(Setting_Config));
  return settings_config;
}
