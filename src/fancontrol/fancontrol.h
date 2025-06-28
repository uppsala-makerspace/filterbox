#ifndef FANCONTROL_H
#define FANCONTROL_H
#define FAN_PIN 2 // Pin for the fan control, change as needed
#include <Arduino.h>
#include "air/airdata.h"
#include "arduino_preferences.h"

extern bool fanEnabled;
extern unsigned long fanLastEnableTime; // Time when the fan was enabled
void handleFanControl();
void setFanEnabled();
void setFanDisabled();
void setupFan();
#endif // FANCONTROL_H