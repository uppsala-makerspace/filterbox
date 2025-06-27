#ifndef DISPLAY_H
#define DISPLAY_H

#include <Arduino.h>
#include "air/airdata.h"

void initDisplay();
void updateDisplay(const AirData &data);
void showSplashScreen();

#endif // DISPLAY_H