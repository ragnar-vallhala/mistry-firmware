/*
 * File: UIHandler.h
 * Author: Ashutosh Vishwakarma
 * Company: Robogen
 * Date: 21-10-2024
 * Description: This file contains ConnectionManager interface class. All the connection managers will inherit fdrom it.
 * Copyright (c) 2024 Robogen
 *
 * This file is part of the Robogen project.
 * Unauthorized copying of this file, via any medium, is strictly prohibited.
 * Proprietary and confidential.
 */
#ifndef UI_HANDLER_H
#define UI_HANDLER_H
#include "display/GFX.h"
#include <cstdint>
enum class Display{
  SSD1306
};


class UIHandler{
public:
  ~UIHandler();
  static UIHandler* get_ui_handler();
  void start();
  Display get_display() const;
  void set_display(Display newDisplay);
private:
  UIHandler()=default;
  static UIHandler* handler;
  Display display = Display::SSD1306;
  uint32_t lastUpdate=0;
  static void* gfx;
};

#endif // !UI_HANDLER_H
