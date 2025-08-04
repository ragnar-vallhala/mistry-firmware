#include "display/SSD1306.h"

SSD1306::SSD1306(uint16_t const devAddr, Size size, i2c_inst_t *i2c)
    : devAddr(devAddr), width(width), height(height), i2c(i2c), size(size) {
  i2c_init(SSD1306_I2C_PORT, 400000); // 400 kHz
  gpio_set_function(SSD1306_I2C_SDA, GPIO_FUNC_I2C);
  gpio_set_function(SSD1306_I2C_SCL, GPIO_FUNC_I2C);
  gpio_pull_up(SSD1306_I2C_SDA);
  gpio_pull_up(SSD1306_I2C_SCL);

  this->width = 128;
  this->height = 64;

  this->buffer = new unsigned char[this->width * this->height / 8];
  this->send_command(SSD1306_DISPLAYOFF);

  this->send_command(SSD1306_SETLOWCOLUMN);
  this->send_command(SSD1306_SETHIGHCOLUMN);
  this->send_command(SSD1306_SETSTARTLINE);
  this->send_command(SSD1306_MEMORYMODE);
  this->send_command(0x00);

  this->set_contrast(0xFF);

  this->rotate_display(1);

  this->send_command(SSD1306_NORMALDISPLAY);

  this->send_command(0xA8);
  this->send_command(0x3F);

  this->send_command(SSD1306_SETDISPLAYOFFSET);
  this->send_command(0x00);

  this->send_command(SSD1306_SETDISPLAYCLOCKDIV);
  this->send_command(0x80);

  this->send_command(SSD1306_SETPRECHARGE);
  this->send_command(0x22);

  this->send_command(SSD1306_SETCOMPINS);
  this->send_command(0x12);

  this->send_command(SSD1306_SETVCOMDETECT);
  this->send_command(0x40);

  this->send_command(SSD1306_CHARGEPUMP);
  this->send_command(0x14);

  this->send_command(SSD1306_DISPLAYALLON_RESUME);

  this->display_on(1);
  this->clear();
  this->display();
}

/*!
    @brief  Deconstructor for I2C-interfaced OLED display.
*/
SSD1306::~SSD1306() { delete this->buffer; }

/*!
 * @brief Send command to display.
 *
 */
void SSD1306::send_command(uint8_t command) {
  uint8_t mess[2] = {0x00, command};
  i2c_write_blocking(this->i2c, this->devAddr, mess, 2, false);
}

/*!
 * @brief Invert colors.
 *
 */
void SSD1306::invert_colors(uint8_t Invert) {
  this->send_command(Invert ? SSD1306_INVERTDISPLAY : SSD1306_NORMALDISPLAY);
}

/*!
 * @brief rotate display.
 *
 */
void SSD1306::rotate_display(uint8_t rotate) {
  if (rotate > 1)
    rotate = 1;

  this->send_command(
      0xA0 | (0x01 & rotate)); // Set Segment Re-Map Default
                               // 0xA0 (0x00) => column Address 0 mapped to 127
                               // 0xA1 (0x01) => Column Address 127 mapped to 0

  this->send_command(
      0xC0 |
      (0x08 &
       (rotate << 3))); // Set COM Output Scan Direction
                        // 0xC0	(0x00) => normal mode (RESET) Scan from COM0 to
                        // COM[N-1];Where N is the Multiplex ratio. 0xC8
                        // (0xC8) => remapped mode. Scan from COM[N-1] to
                        // COM0;;Where N is the Multiplex ratio.
}

/*!
 * @brief Turn on display.
 * 0 – Turn OFF
 * 1 – Turn ON
 */
void SSD1306::display_on(uint8_t on) {
  this->send_command(on ? SSD1306_DISPLAYON : SSD1306_DISPLAYOFF);
}

/*!
 * @brief Set contrast.
 *
 */
void SSD1306::set_contrast(uint8_t contrast) {
  this->send_command(SSD1306_SETCONTRAST);
  this->send_command(contrast);
}

/*!
 * @brief Draw pixel in the buffer.
 * @param x position from the left edge (0, MAX WIDTH)
 * @param y position from the top edge (0, MAX HEIGHT)
 * @param color colors::BLACK, colors::WHITE or colors::INVERSE
 */
void SSD1306::draw_pixel(int16_t x, int16_t y, Colors color) {

  if ((x < 0) || (x >= this->width) || (y < 0) || (y >= this->height))
    return;
  if (size == Size::W128xH32)
    y = (y << 1) + 1;

  switch (color) {
  case Colors::WHITE:
    this->buffer[x + (y / 8) * this->width] |= (1 << (y & 7));
    break;
  case Colors::BLACK:
    this->buffer[x + (y / 8) * this->width] &= ~(1 << (y & 7));
    break;
  case Colors::INVERSE:
    this->buffer[x + (y / 8) * this->width] ^= (1 << (y & 7));
    break;
  }
}

/*!
 * @brief Clear the buffer.
 * @param color colors::BLACK, colors::WHITE or colors::INVERSE
 */
void SSD1306::clear(Colors color) {
  switch (color) {
  case Colors::WHITE:
    memset(buffer, 0xFF, (this->height * this->width / 8));
    break;
  case Colors::BLACK:
    memset(buffer, 0x00, (this->height * this->width / 8));
    break;
  }
}

/*!
 * @brief Send buffer to OLED GCRAM.
 * @param data (Optional) Pointer to data array.
 */
void SSD1306::display(unsigned char *data) {
  if (data == nullptr)
    data = this->buffer;
  this->send_command(0x22);
  this->send_command(0x00);
  this->send_command(0x07);
  this->send_data(data, this->width * this->height / 8);
}

void SSD1306::send_data(uint8_t *buffer, size_t buff_size) {
  unsigned char mess[buff_size + 1];

  mess[0] = 0x40;
  memcpy(mess + 1, buffer, buff_size);

  i2c_write_blocking(this->i2c, this->devAddr, mess, buff_size + 1, false);
}

/*!
 * @brief Return display height.
 * @return display height
 */
uint8_t SSD1306::get_height() {
  if (this->size == Size::W128xH64)
    return 64;
  else
    return 32;
}

/*!
 * @brief Return display width.
 * @return display width
 */
uint8_t SSD1306::get_width() { return this->width; }
