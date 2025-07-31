#ifndef KINEMATICS_H
#define KINEMATICS_H
#include <cstdint>

#define TRIG_PIN 6
#define ECHO_PIN 5

void kinematics_menu(void *param);
float get_distance(uint8_t count);
float get_velocity(uint8_t count);
float get_acceleration(uint8_t count);
#endif // !KINEMATICS_H
