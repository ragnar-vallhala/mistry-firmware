#include "display/GFX.h"
#include "display/SSD1306.h"

namespace {

inline static void swap(int &a, int &b) {
  int tmp = a;
  a = b;
  b = tmp;
}

}; // namespace

/**
 * Create GFX instantion
 *
 * @param devAddr device i2c address.
 * @param size screen size (W128xH64 or W128xH32)
 * @param i2c i2c instance
 */
GFX::GFX(uint16_t const devAddr, Size size, i2c_inst_t *i2c)
    : SSD1306(devAddr, size, i2c) {
 };

/**
 * @brief Draw one char.
 *
 * @param x position from the left edge (0, MAX WIDTH)
 * @param y position from the top edge (0, MAX HEIGHT)
 * @param chr char to be written
 * @param color colors::BLACK, colors::WHITE or colors::INVERSE
 */
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

/**
 * @brief Draw string.
 *
 * @param x position from the left edge (0, MAX WIDTH)
 * @param y position from the top edge (0, MAX HEIGHT)
 * @param str string to be written
 * @param color colors::BLACK, colors::WHITE or colors::INVERSE
 */
void GFX::draw_string(int x, int y, std::string str, Colors color) {
  int x_tmp = x;

  while (str.length()) {
    this->draw_char(x_tmp, y, str.front(), color);
    x_tmp += ((uint8_t)font[1]) + 1;
    str.erase(str.begin());
  }
}

/**
 * @brief Draw empty rectangle.
 *
 * @param x position from the left edge (0, MAX WIDTH)
 * @param y position from the top edge (0, MAX HEIGHT)
 * @param w width of the rectangle
 * @param h height of the rectangle
 * @param color colors::BLACK, colors::WHITE or colors::INVERSE
 */
void GFX::draw_rectangle(int x, int y, uint16_t w, uint16_t h, Colors color) {
  this->draw_horizontal_line(x, y, w, color);
  this->draw_horizontal_line(x, y + h - 1, w, color);
  this->draw_vertical_line(x, y, h, color);
  this->draw_vertical_line(x + w - 1, y, h, color);
}

/**
 * @brief Draw filled rectangle.
 *
 * @param x position from the left edge (0, MAX WIDTH)
 * @param y position from the top edge (0, MAX HEIGHT)
 * @param w width of the rectangle
 * @param h height of the rectangle
 * @param color colors::BLACK, colors::WHITE or colors::INVERSE
 */
void GFX::draw_fill_rectangle(int x, int y, uint16_t w, uint16_t h,
                            Colors color) {
  for (int i = x; i < x + w; i++) {
    this->draw_vertical_line(i, y, h, color);
  }
}

/**
 * @brief Draw progress bar.
 *
 * @param x position from the left edge (0, MAX WIDTH)
 * @param y position from the top edge (0, MAX HEIGHT)
 * @param w width of the rectangle
 * @param h height of the rectangle
 * @param progress progress (0, 100)
 * @param color colors::BLACK, colors::WHITE or colors::INVERSE
 */
void GFX::draw_progress_bar(int x, int y, uint16_t w, uint16_t h,
                          uint8_t progress, Colors color) {
  this->draw_rectangle(x, y, w, h, color);
  this->draw_fill_rectangle(x, y, (uint8_t)((w * progress) / 100), h, color);
}

/**
 * @brief Draw vertical line.
 *
 * @param x position from the left edge (0, MAX WIDTH)
 * @param y position from the top edge (0, MAX HEIGHT)
 * @param h height of the line
 * @param color colors::BLACK, colors::WHITE or colors::INVERSE
 */
void GFX::draw_vertical_line(int x, int y, int h, Colors color) {
  this->draw_line(x, y, x, y + h - 1, color);
}

/**
 * @brief Draw horizontal line.
 *
 * @param x position from the left edge (0, MAX WIDTH)
 * @param y position from the top edge (0, MAX HEIGHT)
 * @param w width of the line
 * @param color colors::BLACK, colors::WHITE or colors::INVERSE
 */
void GFX::draw_horizontal_line(int x, int y, int w, Colors color) {
  this->draw_line(x, y, x + w - 1, y, color);
}

/**
 * @brief Draw straight line.
 *
 * @param xStart position of the first point from the left edge (0, MAX WIDTH)
 * @param yStart position of the first point from the top edge  (0, MAX HEIGHT)
 * @param xEnd position of the second point from the left edge (0, MAX WIDTH)
 * @param yEnd position of the second point from the top edge  (0, MAX HEIGHT)
 * @param color colors::BLACK, colors::WHITE or colors::INVERSE
 */
void GFX::draw_line(int xStart, int yStart, int xEnd, int yEnd,
                   Colors color) {
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

/**
 * @brief Set your own font
 *
 * @param font Pointer to array with your font
 */
void GFX::set_font(const uint8_t *font) { this->font = font; }

/**
 * @brief Get pointer to font array
 *
 * @return Pointer to array with the currently used font
 */
const uint8_t *GFX::get_font() { return font; }
