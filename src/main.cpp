#include <Arduino.h>
#include "PMS.h"
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library
#include <SPI.h>
#include "RTC.h"  // Bibliothque pour le module RTC
#include <Wire.h>
#include <Adafruit_NeoPixel.h>
#include <arduino_secrets.h>
// Include the RTC library
#include "RTC.h"
#include "wifistats.h"
#include <MQTTClient.h>
#include <mqtt.h>
#include <ArduinoJson.h>
//Include the NTP library

// To use Deep Sleep connect RST to GPIO16 (D0) and uncomment below line.
// #define DEEP_SLEEP

// GPIO2 (D4 pin on ESP-12E Development Board).
#define DEBUG_OUT Serial
#define LED_PIN    6
#define LED_COUNT 24
#define LED_Brightness 10
Adafruit_NeoPixel pixels(LED_COUNT, LED_PIN, NEO_RGB + NEO_KHZ800);
String status = "";
String newStatus = "";
uint32_t green = pixels.Color(255, 0, 0);
uint32_t orange = pixels.Color(150, 250, 0);
uint32_t red = pixels.Color(0, 255, 0);
int nrOfLeds=0;
int x=2;
RTCTime lastRead;

// PMS_READ_INTERVAL (4:30 min) and PMS_READ_DELAY (30 sec) CAN'T BE EQUAL! Values are also used to detect sensor state.
static const uint32_t PMS_READ_INTERVAL = 60000;
//static const uint32_t PMS_READ_INTERVAL = 5000;
static const uint32_t PMS_READ_DELAY = 40000;
//static const uint32_t PMS_READ_DELAY = 4000;

// Default sensor state.
uint32_t timerInterval = PMS_READ_DELAY;

  #if defined(__SAM3X8E__)
    #undef __FlashStringHelper::F(string_literal)
    #define F(string_literal) string_literal
#endif

PMS pms(Serial1);
#define cs   10
#define dc   9
#define rst  8  // you can also connect this to the Arduino reset
Adafruit_ST7735 tft = Adafruit_ST7735(cs, dc, rst);
int temps;                // temps d'acquisition
double tempsInit;         // initialisation du timer au dmarrage du loop()

#define relayPin 2


void setup()
{
  pinMode(relayPin, OUTPUT);
  // GPIO1, GPIO3 (TX/RX pin on ESP-12E Development Board)
  Serial.begin(9600);
  Serial1.begin(9600);
  while (!Serial);
  Wire.begin();

  tft.initR(INITR_GREENTAB);
  tft.fillScreen(ST7735_BLACK);

  pixels.begin();           
  pixels.show();
  pixels.setBrightness(LED_Brightness);

  pms.passiveMode();
  pms.wakeUp();


  #ifdef DEEP_SLEEP
    delay(PMS_READ_DELAY);
    readData();
    pms.sleep();
    ESP.deepSleep(PMS_READ_INTERVAL * 1000);
  #endif // DEEP_SLEEP
  
  rtcSetup(tft);
  RTCTime checkconfig;
  RTC.getTime(checkconfig);
  while(checkconfig.getYear() == 2066){
      rtcSetup(tft);
  }
  connectToMQTT();
  tft.fillScreen(ST7735_BLACK);
}

void setLeds(int PM_AE_UG_1_0){
  nrOfLeds = int(PM_AE_UG_1_0);

  Serial.print ("leds: " ); Serial.println(nrOfLeds);
    if ((nrOfLeds<=8) and (nrOfLeds>=1)){
      pixels.fill(green , 0, nrOfLeds);
    
    }
    
    else if ((nrOfLeds<=16) and (nrOfLeds>=8)) {
      pixels.fill(green , 0, 8);
      pixels.fill(orange , 8, ((nrOfLeds)-8));
      
    }
    else if  (nrOfLeds>16) {
      pixels.fill(green , 0, 8);
      pixels.fill(orange , 8, 8);
      pixels.fill(red , 16, ((nrOfLeds)-16));
    }
    else  if (nrOfLeds<=1) {
      pixels.fill(green , 0, 1);
    }
    pixels.show();   // Send the updated pixel colors to the hardware.
}
void readData()
{
  pixels.clear();
  PMS::DATA data;

  // Clear buffer (removes potentially old data) before read. Some data could have been also sent before switching to passive mode.
  while (Serial.available()) { Serial.read(); }

  
  DEBUG_OUT.println("Send read request...");
  pms.requestRead();

  DEBUG_OUT.println("Reading data...");
  if (pms.readUntil(data))
  {
    

    DEBUG_OUT.println();

    DEBUG_OUT.print("PM 1.0 (ug/m3): ");
    
    DEBUG_OUT.println(data.PM_AE_UG_1_0);

    DEBUG_OUT.print("PM 2.5 (ug/m3): ");
    DEBUG_OUT.println(data.PM_AE_UG_2_5);

    DEBUG_OUT.print("PM 10.0 (ug/m3): ");
    DEBUG_OUT.println(data.PM_AE_UG_10_0);
    
    DEBUG_OUT.println();
    tft.fillScreen(ST7735_BLACK);
    
    tft.setCursor(10, 5);
      tft.setTextColor(ST7735_WHITE);
      tft.println(" nbre parts/ 0.1 l");

    tft.setCursor(10, 41);
      tft.setTextColor(ST7735_GREEN);
      tft.setTextSize(1);
      tft.print("1.0 um  ");tft.print(data.PM_AE_UG_1_0);


    tft.setCursor(10, 53);
      tft.setTextColor(ST7735_GREEN);
      tft.setTextSize(1);
      tft.print("2.5 um  ");tft.print(data.PM_AE_UG_2_5);
      

      

    tft.setCursor(10, 5);
      tft.setTextColor(ST7735_WHITE);
      tft.setTextSize(1);
      tft.println(" nbre parts/ 0.1 l");

      RTC.getTime(lastRead);

      setLeds(data.PM_AE_UG_1_0);
      sendToMQTT(data.PM_AE_UG_2_5, data.PM_AE_UG_1_0, lastRead);
  }
  else
  {
    DEBUG_OUT.println("No data.");
  }
}
#ifndef DEEP_SLEEP
void timerCallback() {
  if (timerInterval == PMS_READ_DELAY)
  {
    readData();
    DEBUG_OUT.println("Going to sleep.");
    newStatus = "Sleeping";
    pms.sleep();
  }
  else
  {
    DEBUG_OUT.println("Waking up.");
    newStatus = "waking";
    pms.wakeUp();
  }
}
#endif // DEEP_SLEEP

void updateStatus(){

      tft.setCursor(10, 73);
      tft.setTextColor(ST7735_GREEN);
      tft.setTextSize(1);
      tft.print("lastRead:  ");tft.print(lastRead.getHour());
      tft.print(":");tft.print(lastRead.getMinutes());

      tft.setCursor(10, 93);
      tft.setTextColor(ST7735_BLACK);
      tft.setTextSize(1);
      tft.print("Status  ");tft.print(status);
      tft.setCursor(10, 93);
      tft.setTextColor(ST7735_GREEN);
      tft.print("Status  ");tft.print(newStatus);
      status = newStatus;
  
}
void setFanPin(){
    if(nrOfLeds > 5){
    digitalWrite(relayPin, HIGH);
  } else {
    digitalWrite(relayPin, LOW);
  }
}
void loop()
{
  mqtt.loop();
  if(!mqtt.connected()){
    reconnect();
  }
  if (!network.connected()) {
  connectToWiFi();
}

  if(newStatus != status){
    updateStatus();
  }
  
  setFanPin();

  #ifndef DEEP_SLEEP
    temps = ((millis() - tempsInit))/1000;

    static uint32_t timerLast = 0;

    uint32_t timerNow = millis();
    if (timerNow - timerLast >= timerInterval) {
      timerLast = timerNow;
      timerCallback();
      timerInterval = timerInterval == PMS_READ_DELAY ? PMS_READ_INTERVAL : PMS_READ_DELAY;
    }
  #endif // DEEP_SLEEP
}//homeassistant/device_automation/0AFFD2/bla1/config