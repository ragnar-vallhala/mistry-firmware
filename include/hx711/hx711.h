#ifndef LOADCELL_H
#define LOADCELL_H

#include <cstdint>

class HX711 {
public:
  HX711(int data_pin, int clock_pin);
  bool init();
  bool is_ready();
  int32_t read();

private:
  uint8_t data_pin;
  uint8_t clock_pin;
  int32_t last_val;
  float scale;
  float offset;
};

#endif
