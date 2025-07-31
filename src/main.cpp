#define DEBUG
#include "FreeRTOS.h"

#include "connection/WiFiManager.h"
#include "display/UIHandler.h"
#include "display/display.h"
#include "keypad/keypad.h"
#include "task.h"
#include "util.h"
#include <hardware/gpio.h>
#include <stdio.h>

UIHandler *hndl;

static Display_Param params;

int main() {
  stdio_init_all(); // Initialize stdio for console output
  hndl = UIHandler::get_ui_handler();

  TaskHandle_t displayHandle;
  TaskHandle_t keypadHandle;

  TaskHandle_t displayHandleRun;
  TaskHandle_t keypadHandleRun;

  init_display(&params);
  init_keypad(&params);

  xTaskCreate(run_display, "Display", 2048, (void *)&params, 1,
              &displayHandleRun);
  xTaskCreate(read_keypad_states, "Keypad", 1024, (void *)&params, 2,
              &keypadHandleRun);

  // Create the print message task pinned to Core 1
  vTaskCoreAffinitySet(displayHandleRun, (1 << 0));
  vTaskCoreAffinitySet(keypadHandleRun, (1 << 1));
  // Start the FreeRTOS scheduler
  vTaskStartScheduler();

  /*     // This point should never be reached unless there is insufficient
   * memory
   */
  while (1) {
    // Empty loop
  }

  return 0; // This line will never be reached
}
