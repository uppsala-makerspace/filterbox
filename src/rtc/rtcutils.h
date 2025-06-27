#ifndef RTCUTILS_H
#define RTCUTILS_H

#include <Arduino.h>

void initRTCandNTP();
void syncRTCWithNTP();
void updateNTP();
String getFormattedRTCTime();

#endif // RTCUTILS_H