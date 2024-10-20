// #pragma once

#ifndef _GFX_H
#define _GFX_H

#include "display/SSD1306.h"
#include "font.h"
#include <stdlib.h>
#include <string>
#include "hardware/i2c.h"
#include "pico/stdlib.h"
#include"Config.h"



class GFX : public SSD1306 {
    const uint8_t* font = font_8x5;

    public:
        GFX(uint16_t const devAddr, Size size, i2c_inst_t * i2c);

        void draw_char(int x, int y, char chr, Colors color = Colors::WHITE);
        void draw_string(int x, int y, std::string str, Colors color = Colors::WHITE);
        void draw_progress_bar(int x, int y, uint16_t w, uint16_t h, uint8_t progress, Colors color = Colors::WHITE);
        void draw_fill_rectangle(int x, int y, uint16_t w, uint16_t h, Colors color = Colors::WHITE);
        void draw_rectangle(int x, int y, uint16_t w, uint16_t h, Colors color = Colors::WHITE);
        void draw_horizontal_line(int x, int y, int w, Colors color = Colors::WHITE);
        void draw_vertical_line(int x, int y, int w, Colors color = Colors::WHITE);
        void draw_line(int xStart, int yStart, int xEnd, int yEnd, Colors color = Colors::WHITE);

        void set_font(const uint8_t* font);
        const uint8_t* get_font();
};

#endif
