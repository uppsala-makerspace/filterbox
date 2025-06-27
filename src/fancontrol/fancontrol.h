#ifndef FANCONTROL_H
#define FANCONTROL_H
#define FAN_PIN 2 // Pin for the fan control, change as needed
#include <Arduino.h>
#define FAN_ENABLE_TIME 600000 // Minimum time the fan should be enabled in milliseconds
#define PM25_THRESHOLD 5       // PM2.5 threshold to enable the fan, change as needed

extern bool fanEnabled;
extern unsigned long fanLastEnableTime; // Time when the fan was enabled
void handleFanControl();
void setFanEnabled();
void setFanDisabled();
void setupFan();
#endif // FANCONTROL_H