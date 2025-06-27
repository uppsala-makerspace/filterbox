#ifndef AIRDATA_H
#define AIRDATA_H

#include <Arduino.h>
#include "pmslib/PMS.h"

// --- Data Structure ---
struct AirData
{
  uint16_t pm03Count;
  uint16_t pm10;
  uint16_t pm25;
  uint16_t pm100;
  uint16_t calibratedPm25;
};
// --- PMS State Machine ---
enum PMSState
{
  PMS_SLEEPING,
  PMS_WAKING,
};
// --- Function Declarations ---
#define SCALING_FACTOR 0.02
#define INTERCEPT 0
#define HISTORY_SIZE 180
#define PMS_BREAK_INTERVAL 100000 // Interval to read PMS data in milliseconds
#define PMS_WARMUP_TIME 30000     // PMS sensor warmup time in milliseconds
extern AirData latestReading;
extern uint16_t pm25History[HISTORY_SIZE];
extern uint16_t pm10History[HISTORY_SIZE];
extern uint16_t pm100History[HISTORY_SIZE];
extern uint16_t historyIndex;
extern bool historyFilled;
extern bool historyFilled; // Flag to indicate if history is filled
extern long lastDataTime;  // Last time data was read in milliseconds
extern long lastPMSWakeTimeMs;
extern PMSState pmsState;
extern PMS pms;
extern PMS::DATA airData;

// --- Function Declarations ---
uint16_t calibratePMS25(int pm03Count, int pm25);
void wakePMS();
void sleepPMS();
bool readPMSData(PMS::DATA &data);
void addToHistory(const AirData &data);

#endif // AIRDATA_H