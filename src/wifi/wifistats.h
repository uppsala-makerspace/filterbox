#ifndef WIFISTATS_H
#define WIFISTATS_H

#include <WiFiS3.h>
#include <Arduino.h>

// WiFi and MQTT client objects (declared elsewhere)
extern WiFiClient wifiClient;

// Function declarations only
void connectToWiFi();
bool isWiFiConnected();
void reconnectWiFi();

void setupMQTT();
bool isMQTTConnected();
void reconnectMQTT();
void handleMQTTLoop();
void syncTimeWithNTP();

#endif // WIFISTATS_H