#ifndef MQTTUTILS_H
#define MQTTUTILS_H
#define MQTT_MAX_PACKET_SIZE 512
#include <PubSubClient.h>
#include "air/airdata.h"
#include "fancontrol/fancontrol.h" // for fanc
#include <ArduinoJson.h>
#include "arduino_secrets.h"
#include "wifi/wifistats.h" // for wifiClient

extern PubSubClient mqttClient;
extern bool sentDiscovery;

void setupMQTT();
bool isMQTTConnected();
void reconnectMQTT();
void handleMQTTLoop();
void publishAirData(const AirData &data);
bool sendDeviceTemplate();
void sendDeviceConfiguration();
StaticJsonDocument<512> getDeviceConfigurationTemplate();
#endif // MQTTUTILS_H