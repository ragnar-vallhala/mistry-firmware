#ifndef UTIL_H
#define UTIL_H
#include <cstdint>
#include <stdint.h>
#define DEBUG
#define DEBOUNCE_DELAY 150 // delay for keypad press
#define VISIBLE_MENU_ITEMS 5

#define BUTTON_PIN 7

#include "FreeRTOS.h"
#include "display/UIHandler.h"
#include "task.h"
struct Task;

struct Display_Param {
  uint16_t keypad_state = 0;
  uint64_t last_read = 0;
  UIHandler *display = NULL;
  Task *current_menu = NULL;
};

typedef void (*TaskFn)(void *);
uint32_t millis();
void delay(uint32_t ms); // blocking delay
uint32_t millis_nortos();
struct Task {
  TaskFn function = NULL;
  void *param = NULL;
  Task(TaskFn task, void *params) : function(task), param(params) {}
};

uint32_t pulseIn(uint pin, bool state, uint32_t timeout_us);

#endif // !UTIL_H
