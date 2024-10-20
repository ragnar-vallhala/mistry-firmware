/*
 * File: UARTManager.h
 * Author: Ashutosh Vishwakarma
 * Company: Robogen
 * Date: 20-10-2024
 * Description: This file contains UARTManager which inherit from
 * ConnectionManager interface class. Copyright (c) 2024 Robogen
 *
 * This file is part of the Robogen project.
 * Unauthorized copying of this file, via any medium, is strictly prohibited.
 * Proprietary and confidential.
 */

#ifndef UART_MANAGER_H
#define UART_MANAGER_H
#include "ConnectionManager.h"
#include "pico/stdlib.h"
#include <string>
#include <pico/stdio.h>
#include <pico/time.h>

class UARTManager : public ConnectionManager {

public:
  ~UARTManager();
  static UARTManager *get_uart_manager();
  bool establish(std::string *msg = nullptr);
  void send(std::string msg);
  void recieve(std::string *msg);
  void recieve_file(std::string *content, std::string *fileName); // TODO: Implement this to save files to SD Card
private:
  UARTManager() = default;
  static UARTManager *obj;
};
#endif // UART_MANAGER_H
