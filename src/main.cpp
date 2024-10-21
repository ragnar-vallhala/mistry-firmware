#include "pico/stdlib.h"
#include <hardware/timer.h>
#include <pico/time.h>
#include "display/UIHandler.h"
#include "connection/UARTManager.h"

int main() {
  stdio_init_all();
  UIHandler* handler = UIHandler::get_ui_handler();
  UARTManager* uart = UARTManager::get_uart_manager();
  while (true) {
    handler->start();
    if(time_us_64()%2000000==0)
    {
      uart->send("Hello From Pico!");
    }
  }
  return 0;
}
