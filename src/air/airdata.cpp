#include "airdata.h"

// --- Global Variables ---
PMS pms(Serial1);
PMS::DATA airData;
PMSState pmsState = PMS_SLEEPING;
long lastPMSWakeTimeMs = 0;
long lastDataTime = 0;
uint16_t historyIndex = 0;
bool historyFilled = false;
AirData latestReading = {};
// Use only HISTORY_SIZE here!
uint16_t pm25History[HISTORY_SIZE] = {0};
uint16_t pm10History[HISTORY_SIZE] = {0};
uint16_t pm100History[HISTORY_SIZE] = {0};

void wakePMS()
{
  Serial.println("Waking up PMS sensor...");
  pms.wakeUp();
  pmsState = PMS_WAKING;
  lastPMSWakeTimeMs = millis();
}

void sleepPMS()
{
  Serial.println("Sleeping PMS sensor...");
  pms.sleep();
  pmsState = PMS_SLEEPING;
}

void addToHistory(const AirData &data)
{
  pm25History[historyIndex] = data.calibratedPm25;
  pm10History[historyIndex] = data.pm10;
  pm100History[historyIndex] = data.pm100;
  historyIndex++;
  if (historyIndex >= HISTORY_SIZE)
  {
    historyIndex = 0;
    historyFilled = true;
  }
}
uint16_t calibratePMS25(int pm03Count, int pm25)
{
  uint16_t calibratedPm25; // Default to original PM2.5 value
  // calibrationlogic and values from https://www.airgradient.com/blog/low-readings-from-pms5003/
  uint16_t PM25_calibrated_low = SCALING_FACTOR * pm03Count + INTERCEPT;
  if (PM25_calibrated_low < 31)
  {
    calibratedPm25 = PM25_calibrated_low; // Ensure no negative values
  }
  else
  {
    calibratedPm25 = pm25; // If calibration is not valid, use original PM2.5 value
  }
  if (calibratedPm25 < 0)
  {
    calibratedPm25 = 0; // Ensure no negative values
  }
  Serial.print("calibration0.3: ");
  Serial.println(pm03Count);
  Serial.print("reading 2.5: ");
  Serial.println(pm25);
  Serial.print("calculated 2.5: ");
  Serial.println(PM25_calibrated_low);
  return calibratedPm25;
}
// --- Read Sensor Data ---
bool readPMSData(PMS::DATA &data)
{
  // Clear buffer (removes potentially old data) before read. Some data could have been also sent before switching to passive mode.
  while (Serial.available())
  {
    Serial.read();
  }

  Serial.println("Send read request...");
  pms.requestRead();

  Serial.println("Reading data...");

  if (pms.readUntil(data))
  {
    Serial.println("PMS data received:");
    Serial.print("PM1.0: ");
    Serial.println(data.PM_AE_UG_1_0);
    Serial.print("PM2.5: ");
    Serial.println(data.PM_AE_UG_2_5);
    Serial.print("PM10: ");
    Serial.println(data.PM_AE_UG_10_0);

    uint16_t calibratedPm25 = calibratePMS25(data.PARTICLE_0_3UM, data.PM_AE_UG_2_5);
    latestReading = {airData.PARTICLE_0_3UM, airData.PM_AE_UG_1_0,
                     airData.PM_AE_UG_2_5, airData.PM_AE_UG_10_0, calibratedPm25};

    return true;
  }
  else
  {
    Serial.println("PMS read timeout");
    return false;
  }
}
