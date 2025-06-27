#include "airutils.h"
#include <Adafruit_NeoPixel.h>
#include <Arduino.h>
#include "airdata.h"

#define LED_PIN 6
#define NUM_PIXELS 1

Adafruit_NeoPixel strip(NUM_PIXELS, LED_PIN, NEO_GRB + NEO_KHZ800);

void setupNeoPixel()
{
  strip.begin();
  strip.show();
}

void updateNeoPixel(const AirData &data)
{
  if (data.pm25 <= 12)
  {
    strip.setPixelColor(0, strip.Color(0, 255, 0)); // Green
  }
  else if (data.pm25 <= 35)
  {
    strip.setPixelColor(0, strip.Color(255, 255, 0)); // Yellow
  }
  else
  {
    strip.setPixelColor(0, strip.Color(255, 0, 0)); // Red
  }
  strip.show();
}

void setupPMSSensor()
{
  // PMS sensor is initialized in airdata.cpp
  Serial1.begin(9600);
  pms.passiveMode();
  pms.wakeUp();
  pmsState = PMS_WAKING;
  lastPMSWakeTimeMs = millis();
  Serial.println("PMS Sensor initialized");
}