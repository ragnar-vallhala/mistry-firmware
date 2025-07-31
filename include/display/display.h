#ifndef DISPLAY_H
#define DISPLAY_H
#include "display/GFX.h"
#include "util.h"
void init_display(void *pvParameters);
void main_menu(void *pvParameters);
void run_display(void *pvParameters);
void draw_status_bar(GFX *display, const char *title = "STEM Gun");

#endif // !DISPLAY_H
