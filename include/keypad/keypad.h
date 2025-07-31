#ifndef KEYPAD_H
#define KEYPAD_H
#include "util.h"
#include <stdint.h>

#define S0_PIN 0
#define S1_PIN 1
#define S2_PIN 2
#define S3_PIN 3
#define SIG_PIN 4

#define KEY_NONE 0
#define KEY_1 1
#define KEY_2 2
#define KEY_3 4
#define KEY_4 8
#define KEY_5 16
#define KEY_6 32
#define KEY_7 64
#define KEY_8 128
#define KEY_9 256
#define KEY_DOT 512
#define KEY_0 1024
#define KEY_BACKSPACE 2048
#define KEY_DOWN 4096
#define KEY_LEFT 8192
#define KEY_RIGHT 16384
#define KEY_UP 32768

const uint8_t selectorPins[4] = {S0_PIN, S1_PIN, S2_PIN, S3_PIN};

void init_keypad(void *params);
void read_keypad_states(void *params);
bool isKeyPressed(uint64_t key, Display_Param *dp, uint64_t now);

#endif // !KEYPAD_H
