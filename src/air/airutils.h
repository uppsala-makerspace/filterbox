#ifndef AIRUTILS_H
#define AIRUTILS_H

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include "airdata.h"
#include "pmslib/PMS.h" //PMS sensor library
#include <RTC.h>        // Add this include

// Display
void initDisplay();
void showSplashScreen();
void updateDisplay(const AirData &data);

// NeoPixel
void setupNeoPixel();
void updateNeoPixel(const AirData &data);

// PMS sensor

extern PMS pms;
void setupPMSSensor();

// MQTT publish (you can put this here or in a separate file if preferred)
void publishAirData(const AirData &data);

#endif // AIRUTILS_H