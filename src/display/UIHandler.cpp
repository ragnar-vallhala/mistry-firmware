#include "display/UIHandler.h"
#include "display/GFX.h"
#include "display/SSD1306.h"
#include <hardware/timer.h>
#include "display/display.h"

UIHandler *UIHandler::handler = nullptr;
void *UIHandler::gfx = nullptr;
UIHandler::~UIHandler() {
  if (handler)
    delete handler;
  if (gfx)
    delete (GFX *)gfx;
}
UIHandler *UIHandler::get_ui_handler() {
  if (handler == nullptr)
    handler = new UIHandler();
  if (gfx == nullptr)
    gfx = (void *)new GFX(0x3C, Size::W128xH64, i2c0);
  return handler;
}

void UIHandler::start() {
  if (time_us_64() / 100000 > lastUpdate) {
    lastUpdate = time_us_64() / 100000;
    if (display == Display::SSD1306) {
    }
  }
}

Display UIHandler::get_display() const { return display; }

void UIHandler::set_display(Display newDisplay) { display = newDisplay; }
