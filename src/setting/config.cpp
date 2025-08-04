#include "setting/config.h"
#include "display/SSD1306.h"
#include "display/display.h"
#include "hardware/flash.h"
#include "hardware/sync.h"
#include "hardware/watchdog.h"
#include "pico/stdlib.h"
#include "projdefs.h"
#include "util.h"

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
void save_config(const Setting_Config &cfg, void *dispConfig)
    __attribute__((section(".time_critical.save_config")));

void save_config(const Setting_Config &cfg, void *dispConfig) {
  if (cmp_setting_config(cfg, settings_config))
    return;
  uint8_t buf[CONFIG_SIZE] = {0};
  memcpy(buf, &cfg, sizeof(Setting_Config));

  Display_Param *disp = (Display_Param *)dispConfig;
  GFX *gfx = (GFX *)disp->display->gfx;
  gfx->clear(Colors::WHITE);
  gfx->draw_string(20, 30, "Rebooting...", Colors::BLACK);
  gfx->display();
  uint32_t ints = save_and_disable_interrupts();
  flash_range_erase(FLASH_TARGET_OFFSET, FLASH_SECTOR_SIZE);
  flash_range_program(FLASH_TARGET_OFFSET, buf, CONFIG_SIZE);
  restore_interrupts(ints);
  watchdog_reboot(0, 0, 3000);
  while (1)
    ;
}

Setting_Config load_config(bool force_read) {
  static bool read_once = false;
  if (read_once && !force_read)
    return settings_config;
  read_once = true;
  const uint8_t *flash_data = (const uint8_t *)(XIP_BASE + FLASH_TARGET_OFFSET);
  memcpy(&settings_config, flash_data, sizeof(Setting_Config));
  if (settings_config.magic_number != MAGIC_NUMBER) {
    settings_config.loadcell_scale = 1.0;
    settings_config.loadcell_offset = 0.0;
    settings_config.microphone_scale = 1.0;
    settings_config.microphone_offset = 0.0;
  }
  return settings_config;
}
