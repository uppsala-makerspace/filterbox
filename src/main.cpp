#include <Arduino.h>
#include "air/airdata.h"
#include "air/airutils.h"
#include "display/display.h"
#include "mqtt/mqttutils.h"
#include "rtc/rtcutils.h"
#include "wifi/wifistats.h"
#include "fancontrol/fancontrol.h"
#define CHECK_INTERVAL 10000 // Interval to check WiFi and MQTT connection in milliseconds
long lastCheckMillis = 0;    // Last time WiFi and MQTT were checked
void setup()
{
  Serial.begin(9600);
  initDisplay();
  showSplashScreen();

  showSplashScreen();
  connectToWiFi();
  setupMQTT();
  initRTCandNTP();
  setupPMSSensor();
  setupNeoPixel();
  reconnectMQTT();
  setupFan();
}

void loop()
{

  if (millis() - lastCheckMillis >= CHECK_INTERVAL)
  {
    lastCheckMillis = millis();

    if (!isWiFiConnected())
    {
      reconnectWiFi();
    }

    if (!isMQTTConnected())
    {
      reconnectMQTT();
    }
    if (sentDiscovery == false)
    {
      sendDeviceConfiguration();
    }
  }

  switch (pmsState)
  {
  case PMS_SLEEPING:
    if (millis() - lastDataTime >= PMS_BREAK_INTERVAL)
    {
      wakePMS();
    }
    break;
  case PMS_WAKING:
    if (millis() - lastPMSWakeTimeMs >= PMS_WARMUP_TIME)
    { // delay time seconds passed
      if (readPMSData(airData))
      {
        lastDataTime = millis();

        updateNeoPixel(latestReading);
        addToHistory(latestReading);
        updateDisplay(latestReading);
        publishAirData(latestReading);
        handleFanControl();
        sleepPMS();
      }
    }
    break;
  default:
    pmsState = PMS_SLEEPING;
    break;
  }

  handleMQTTLoop();
}
