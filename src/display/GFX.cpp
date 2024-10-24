#include "display/GFX.h"
#include "display/SSD1306.h"
#include <cstdint>
#include <hardware/timer.h>
#include <pico/stdio_usb.h>
#include "connection/WiFiManager.h"


namespace {

inline static void swap(int &a, int &b) {
  int tmp = a;
  a = b;
  b = tmp;

}

}; // namespace
GFX::GFX(uint16_t const devAddr, Size size, i2c_inst_t *i2c)
    : SSD1306(devAddr, size, i2c){};

void GFX::draw_char(int x, int y, char chr, Colors color) {
  if (chr > 0x7E)
    return; // chr > '~'

  for (uint8_t i = 0; i < this->font[1]; i++) {
    uint8_t line =
        (uint8_t)(this->font)[(chr - 0x20) * (this->font)[1] + i + 2];

    for (int8_t j = 0; j < this->font[0]; j++, line >>= 1) {
      if (line & 1) {
        this->draw_pixel(x + i, y + j, color);
      }
    }
  }
}

void GFX::draw_string(int x, int y, std::string str, Colors color) {
  int x_tmp = x;

  while (str.length()) {
    this->draw_char(x_tmp, y, str.front(), color);
    x_tmp += ((uint8_t)font[1]) + 1;
    str.erase(str.begin());
  }
}

void GFX::draw_rectangle(int x, int y, uint16_t w, uint16_t h, Colors color) {
  this->draw_horizontal_line(x, y, w, color);
  this->draw_horizontal_line(x, y + h - 1, w, color);
  this->draw_vertical_line(x, y, h, color);
  this->draw_vertical_line(x + w - 1, y, h, color);
}

void GFX::draw_fill_rectangle(int x, int y, uint16_t w, uint16_t h,
                              Colors color) {
  for (int i = x; i < x + w; i++) {
    this->draw_vertical_line(i, y, h, color);
  }
}

void GFX::draw_progress_bar(int x, int y, uint16_t w, uint16_t h,
                            uint8_t progress, Colors color) {
  this->draw_rectangle(x, y, w, h, color);
  this->draw_fill_rectangle(x, y, (uint8_t)((w * progress) / 100), h, color);
}

void GFX::draw_vertical_line(int x, int y, int h, Colors color) {
  this->draw_line(x, y, x, y + h - 1, color);
}

void GFX::draw_horizontal_line(int x, int y, int w, Colors color) {
  this->draw_line(x, y, x + w - 1, y, color);
}

void GFX::draw_line(int xStart, int yStart, int xEnd, int yEnd, Colors color) {
  int16_t steep = abs(yEnd - yStart) > abs(xEnd - xStart);

  if (steep) {
    swap(xStart, yStart);
    swap(xEnd, yEnd);
  }

  if (xStart > xEnd) {
    swap(xStart, xEnd);
    swap(yStart, yEnd);
  }

  int16_t dx = xEnd - xStart;
  int16_t dy = abs(yEnd - yStart);

  int16_t err = dx / 2;
  int16_t ystep;

  if (yStart < yEnd) {
    ystep = 1;
  } else {
    ystep = -1;
  }

  while (xStart <= xEnd) {
    if (steep) {
      this->draw_pixel(yStart, xStart, color);
    } else {
      this->draw_pixel(xStart, yStart, color);
    }

    xStart++;
    err -= dy;

    if (err < 0) {
      yStart += ystep;
      err += dx;
    }
  }
}

void GFX::set_font(const uint8_t *font) { this->font = font; }

const uint8_t *GFX::get_font() { return font; }

void GFX::draw_symbol(int x, int y, char symbol, Colors color) {
  // TODO:: Implement Symbol Drawing
  if (symbol > 0x7E)
    return; // symbol > '~'

  for (uint8_t i = 0; i < this->symbol[1]; i++) {
    uint8_t line =
        (uint8_t)(this->symbol)[(symbol) * (this->symbol)[1] + i + 2];

    for (int8_t j = 0; j < this->symbol[0]; j++, line >>= 1) {
      if (line & 1) {
        this->draw_pixel(x + i, y + j, color);
      }
    }
  }
}

std::string get_uptime_string() {
  uint64_t time = time_us_64() / 1000000;
  std::string sTime = "";
  uint16_t piece = time / 3600; // get hours
  sTime += std::to_string(piece) + ':';
  time %= 3600;
  piece = time / 60; // get minutes
  if (piece < 10)
    sTime += '0' + std::to_string(piece) + ':';
  else
    sTime += std::to_string(piece) + ':';
  time %= 60;
  if (time < 10)
    sTime += '0' + std::to_string(time);
  else
    sTime += std::to_string(time);
  return sTime;
}

void GFX::draw_status_bar() {
  WiFiManager* wifi = WiFiManager::get_wifi_manager();
  draw_fill_rectangle(0, 0, 128, 10, Colors::BLACK);
  if(stdio_usb_connected()) draw_symbol(118,0,1);
  if(wifi->is_connected_wifi()) draw_symbol(108,0,0);
  draw_string(0, 0, get_uptime_string());
  draw_horizontal_line(2, 10, 126);
}
