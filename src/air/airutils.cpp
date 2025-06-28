#include "airutils.h"
#include <Adafruit_NeoPixel.h>
#include <Arduino.h>
#include "airdata.h"

#define LED_PIN 6
#define NUM_PIXELS 24

Adafruit_NeoPixel strip(NUM_PIXELS, LED_PIN, NEO_GRB + NEO_KHZ800);
uint16_t litPixels;
uint32_t green = strip.Color(0, 255, 0);
uint32_t yellow = strip.Color(255, 255, 0);
uint32_t red = strip.Color(255, 0, 0);

void setupNeoPixel()
{
  strip.begin();
  strip.setBrightness(LED_BRIGHTNESS); // Set brightness to 50%
  strip.show();
}

void updateNeoPixel(const AirData &data)
{
  litPixels = data.calibratedPm25;
  if (litPixels == 0)
  {
    litPixels = 1;
  }
  if (litPixels > NUM_PIXELS)
    litPixels = NUM_PIXELS;

  if (litPixels <= 8)
  {
    strip.fill(green, 0, litPixels);
    strip.fill(0, litPixels, NUM_PIXELS - litPixels); // turn off the rest
  }
  else if (litPixels <= 16)
  {
    strip.fill(green, 0, 8);
    strip.fill(yellow, 8, litPixels - 8);
    strip.fill(0, litPixels, NUM_PIXELS - litPixels);
  }
  else
  {
    strip.fill(green, 0, 8);
    strip.fill(yellow, 8, 8);
    strip.fill(red, 16, litPixels - 16);
    strip.fill(0, litPixels, NUM_PIXELS - litPixels);
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