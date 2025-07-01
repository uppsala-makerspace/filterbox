#include "mqttutils.h"

PubSubClient mqttClient(wifiClient);
String clientId = MQTT_CLIENT_ID;
bool sentDiscovery = false;
void setupMQTT()
{
    mqttClient.setServer(MQTT_BROKER, MQTT_PORT);
}

bool isMQTTConnected()
{
    return mqttClient.connected();
}

void reconnectMQTT()
{
    while (!mqttClient.connected())
    {
        Serial.print("Attempting MQTT connection...");
        if (mqttClient.connect("airfilter01", MQTT_USER, MQTT_PASS))
        {
            Serial.println("connected");
        }
        else
        {
            Serial.print("failed, rc=");
            Serial.print(mqttClient.state());
            Serial.println(" try again in 5 seconds");
            delay(5000);
        }
    }
}

void handleMQTTLoop()
{
    mqttClient.loop();
}
void publishAirData(const AirData &data)
{
    StaticJsonDocument<128> doc;
    doc["pm03c"] = data.pm03Count;
    doc["pm1"] = data.pm10;
    doc["pm25"] = data.calibratedPm25;
    doc["pm10"] = data.pm100;
    doc["relay"] = fanEnabled ? 1 : 0; // Add this line (or use your relay state variable)
    String payload;
    serializeJson(doc, payload);

    if (mqttClient.connected())
    {
        mqttClient.publish(("airquality/" + clientId + "/state").c_str(), payload.c_str(), true);
        Serial.print("Published: ");
        Serial.println(payload);
    }
    else
    {
        Serial.println("MQTT not connected, skipping publish");
    }
}

StaticJsonDocument<512> getDeviceConfigurationTemplate()
{
    StaticJsonDocument<512> doc;

    doc["enabled_by_default"] = true;
    doc["state_class"] = "measurement";
    doc["state_topic"] = "airquality/" + clientId + "/state";
    JsonObject device = doc.createNestedObject("device");
    device["identifiers"][0] = clientId.c_str();
    device["name"] = "AirFIlter box";

    return doc;
}

bool sendDeviceTemplate(const char *uniqueId, const char *name, const char *deviceClass, const char *unitOfMeasurement,
                        const char *valueTemplate, const char *stateTopic)
{

    StaticJsonDocument<512> doc = getDeviceConfigurationTemplate();
    if (strlen(deviceClass) > 0)
    {
        doc["device_class"] = deviceClass;
    }
    if (strlen(unitOfMeasurement) > 0)
    {
        doc["unit_of_measurement"] = unitOfMeasurement;
    }
    if (strlen(valueTemplate) > 0)
    {
        doc["value_template"] = valueTemplate;
    }

    String uid = clientId + "_" + uniqueId;
    doc["unique_id"] = uid; // Concatenate uniqueId
    doc["name"] = name;

    if (stateTopic != "")
    {
        doc["state_topic"] = "airquality/" + clientId + "/state" + "/" + stateTopic;
    }
    String fullTopic = "homeassistant/sensor/" + clientId + "/" + uniqueId + "/config";
    String jsonOutput = "";
    serializeJson(doc, jsonOutput);
    Serial.print("Topic: ");
    Serial.println(fullTopic);
    Serial.print("Payload: ");
    Serial.println(jsonOutput);
    Serial.print("Topic length: ");
    Serial.println(fullTopic.length());
    Serial.print("Payload length: ");
    Serial.println(jsonOutput.length());
    Serial.print("MQTT connected: ");
    Serial.println(mqttClient.connected());

    bool isPublished = mqttClient.publish(fullTopic.c_str(), jsonOutput.c_str(), true);
    delay(100); // Small delay to ensure the message is sent
    Serial.println(isPublished ? "MQTT published ✅" : "MQTT publish ❌");
    return isPublished;
}

void sendDeviceConfiguration()
{
    const int maxRetries = 5;
    const int retryDelayMs = 2000;
    bool allSent = false;

    for (int attempt = 0; attempt < maxRetries && !allSent; ++attempt)
    {
        if (!mqttClient.connected())
        {
            reconnectMQTT();
        }
        handleMQTTLoop(); // ensure it doesnt time out
        bool pm03CountSent = sendDeviceTemplate("pm03c", "Count PM0,3", "", "particles/0.1L", "{{value_json.pm03c}}", "");
        bool pm1Sent = sendDeviceTemplate("pm1", "PM1", "pm1", "µg/m³", "{{value_json.pm1}}", "");
        bool pm25Sent = sendDeviceTemplate("pm25", "PM2.5", "pm25", "µg/m³", "{{value_json.pm25}}", "");
        bool pm10Sent = sendDeviceTemplate("pm10", "PM10", "pm10", "µg/m³", "{{value_json.pm10}}", "");
        bool relaySent = sendDeviceTemplate("relay", "Fan Relay", "", "", "{{value_json.relay}}", "");
        allSent = relaySent && pm03CountSent && pm1Sent && pm25Sent && pm10Sent;

        if (!allSent)
        {
            Serial.println("One or more discovery messages failed, retrying...");
            delay(retryDelayMs);
        }
    }

    sentDiscovery = allSent;
    if (allSent)
    {
        Serial.println("All discovery messages sent successfully.");
    }
    else
    {
        Serial.println("Failed to send all discovery messages after retries.");
    }
}
