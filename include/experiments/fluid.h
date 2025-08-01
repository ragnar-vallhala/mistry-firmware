#ifndef FLUID_H
#define FLUID_H

// Adjust based on your actual pinout
#define LOADCELL_DOUT_PIN 10
#define LOADCELL_SCK_PIN 11

void fluid_menu(void *params);
void buyoancy_experiment(void *params);
void density_experiment(void *params);
#endif // !FLUID_H
