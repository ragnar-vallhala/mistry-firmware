/*
 * File: UARTManager.cpp
 * Author: Ashutosh Vishwakarma
 * Company: Robogen
 * Date: 20-10-2024
 * Description: This file contains UARTManager which inherit from ConnectionManager interface class.
 * Copyright (c) 2024 Robogen
 *
 * This file is part of the Robogen project.
 * Unauthorized copying of this file, via any medium, is strictly prohibited.
 * Proprietary and confidential.
 */

#include "connection/UARTManager.h"

UARTManager* UARTManager::obj = nullptr;
UARTManager *UARTManager::get_uart_manager() {
  if (!obj)
    obj = new UARTManager();
  return obj;
}
UARTManager::~UARTManager() {
  if (obj)
    delete obj;
}
bool UARTManager::establish(std::string *msg) {
  stdio_init_all();
  sleep_ms(100);
  if (!stdio_usb_connected())
    return false;
  return true;
}

void UARTManager::send(std::string msg) { stdio_puts(msg.c_str()); }

void UARTManager::recieve(std::string *msg) {
  if (stdio_usb_connected()) {
    while (true) {
      int ch = getchar_timeout_us(1000); // Read character with 1 ms timeout
      if (ch != PICO_ERROR_TIMEOUT && ch != EOF && ch != '\n') {
        (*msg) += (char)ch; // Store character
      } else {
        break;
      }
    }
  }
}
