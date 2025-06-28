#include "fancontrol.h"

bool fanEnabled = false;
long unsigned fanLastEnableTime = 0; // Time when the fan was enabled

void setupFan()
{
    pinMode(FAN_PIN, OUTPUT); // Set the fan pin as input initially
    Serial.println("Fan control initialized");
}
void setFanEnabled()
{
    if (!fanEnabled)
    {
        digitalWrite(FAN_PIN, HIGH); // Turn on the fan
        fanEnabled = true;
        Serial.println("Fan enabled");
        fanLastEnableTime = millis(); // Record the time when the fan was enabled
    }
}
void setFanDisabled()
{
    if (fanEnabled)
    {
        digitalWrite(FAN_PIN, LOW); // Turn off the fan
        fanEnabled = false;
        Serial.println("Fan disabled");
    }
}
void handleFanControl()
{
    Serial.println("Handling fan control...");
    Serial.println("Latest PM2.5 reading: " + String(latestReading.calibratedPm25));
    Serial.println("Fan enabled: " + String(fanEnabled));
    if (millis() - fanLastEnableTime >= FAN_ENABLE_TIME &&
        fanEnabled && latestReading.calibratedPm25 < PM25_THRESHOLD)
    {
        setFanDisabled(); // Disable fan after FAN_ENABLE_TIME
    }
    else if (latestReading.calibratedPm25 >= PM25_THRESHOLD && !fanEnabled)
    {
        setFanEnabled(); // Enable fan if PM2.5 exceeds threshold
    }
}