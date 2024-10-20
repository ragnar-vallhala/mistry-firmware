/*
 * File: OLED.h
 * Author: Ashutosh Vishwakarma
 * Company: Robogen
 * Date: 20-10-2024
 * Description: This file contains SSD1306 class which will manage the OLED display.
 * Copyright (c) 2024 Robogen
 *
 * This file is part of the Robogen project.
 * Unauthorized copying of this file, via any medium, is strictly prohibited.
 * Proprietary and confidential.
 */

#ifndef SSD1306_H
#define SSD1306_H

#include "hardware/i2c.h"
#include "string.h"
#include "stdint.h"
#include "Config.h"
#include "pico/stdlib.h"

#define SSD1306_SETCONTRAST 0x81
#define SSD1306_DISPLAYALLON_RESUME 0xA4
#define SSD1306_DISPLAYALLON 0xA5
#define SSD1306_NORMALDISPLAY 0xA6
#define SSD1306_INVERTDISPLAY 0xA7
#define SSD1306_DISPLAYOFF 0xAE
#define SSD1306_DISPLAYON 0xAF
#define SSD1306_SETDISPLAYOFFSET 0xD3
#define SSD1306_SETCOMPINS 0xDA
#define SSD1306_SETVCOMDETECT 0xDB
#define SSD1306_SETDISPLAYCLOCKDIV 0xD5
#define SSD1306_SETPRECHARGE 0xD9
#define SSD1306_SETMULTIPLEX 0xA8
#define SSD1306_SETLOWCOLUMN 0x00
#define SSD1306_SETHIGHCOLUMN 0x10
#define SSD1306_SETSTARTLINE 0x40
#define SSD1306_MEMORYMODE 0x20
#define SSD1306_COLUMNADDR 0x21
#define SSD1306_PAGEADDR   0x22
#define SSD1306_COMSCANINC 0xC0
#define SSD1306_COMSCANDEC 0xC8
#define SSD1306_SEGREMAP 0xA0
#define SSD1306_CHARGEPUMP 0x8D
#define SSD1306_EXTERNALVCC 0x1
#define SSD1306_SWITCHCAPVCC 0x2


enum class Colors {
	BLACK,
	WHITE,
	INVERSE
};

enum class Size {
	W128xH64,
	W128xH32
};


class SSD1306 {
	protected:
        uint16_t devAddr;
		i2c_inst_t * i2c;
		uint8_t width;
		uint8_t height;
		Size size;
		
		unsigned char * buffer;

		void send_data(uint8_t* buffer, size_t buff_size);
		void send_command(uint8_t command);

	public:
		SSD1306(uint16_t const devAddr, Size size, i2c_inst_t * i2c);
		~SSD1306();

		void display_on(uint8_t on);
		void invert_colors(uint8_t invert);
		void rotate_display(uint8_t rotate);
		void set_contrast(uint8_t contrast);

		void draw_pixel(int16_t x, int16_t y, Colors color = Colors::WHITE);
		void clear(Colors color = Colors::BLACK);
		void display(unsigned char *data = nullptr);
		uint8_t get_height();
		uint8_t get_width();
};
#endif // SSD1306_H

