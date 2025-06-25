#include <MQTTClient.h>
#include <Arduino.h>
#include <ArduinoJson.h>

const char MQTT_BROKER_ADRRESS[] = "192.168.10.38";  // CHANGE TO MQTT BROKER'S ADDRESS
int MQTT_PORT = 1883;
const char MQTT_CLIENT_ID[] = "airfilter01";  // CHANGE IT AS YOU DESIRE
const char MQTT_USERNAME[] = "";                        // CHANGE IT IF REQUIRED, empty if not required
const char MQTT_PASSWORD[] = "";                        // CHANGE IT IF REQUIRED, empty if not required
const char PUBLISH_TOPIC[] = "makerspace/traverkstan/filterbox/particlesensor";
String clientId = "airfilter01";
//const char DISCOVERY_TOPIC[] = "homeassistant/device/0AFFD2/config"; 
//const char NODE_ID[] = "0AFFD2";    // CHANGE IT AS YOU DESIRE
String SUBSCRIBE_TOPIC = "homeassistant/device/0AFFD2/receive";  // CHANGE IT AS YOU DESIRE
const int PUBLISH_INTERVAL = 5000;

MQTTClient mqtt = MQTTClient(512);
unsigned long lastPublishTime = 0;
WiFiClient network;


void messageHandler(String &topic, String &payload) {
  Serial.println("Arduino UNO R4 - received from MQTT:");
  Serial.println("- topic: " + topic);
  Serial.println("- payload:");
  Serial.println(payload);
}
void reconnect() {
    while (!mqtt.connected()) {
        Serial.print("Attempting MQTT connection...");

        // Attempt to connect
        if (mqtt.connect(MQTT_CLIENT_ID, MQTT_USERNAME, MQTT_PASSWORD)) {
            Serial.print("Connected");
        } else {
            // Wait 5 seconds before retrying
            delay(2000);
        }
    }
}
StaticJsonDocument<512> getDeviceConfigurationTemplate() {
    StaticJsonDocument<512> doc;

    doc["enabled_by_default"] = true;
    doc["state_class"] = "measurement";
    doc["device_class"] = "humidity";  // Sets the icon in the UI
    doc["state_topic"] = "airquality/" + clientId + "/state";
    doc["unit_of_measurement"] = "%";
    doc["value_template"] = "{{ value_json.humidity}}";
    doc["unique_id"] = "hum01ae";

    JsonObject device = doc.createNestedObject("device");
    device["identifiers"][0] = clientId.c_str();
    device["manufacturer"] = "Wesley Elfring";
    device["model"] = "Air Quality V3";
    device["name"] = "Air Quality Sensor";
    device["sw_version"] = "3";

    return doc;
}


void sendJsonToTopic(const char* topic, String json) {
    mqtt.loop();
    if(!network.connected()){
        connectToWiFi();
    }
    if(!mqtt.connected()){
        reconnect();
    }
    Serial.println(">> Publishing to topic: " + String(topic));
    Serial.println(">> Payload: " + json);

    bool isPublished = mqtt.publish(topic, json.c_str(), json.length(), true);
    Serial.println(isPublished ? "MQTT published ✅" : "MQTT publish ❌");
}


void sendDeviceTemplate(const char* uniqueId, const char* name, const char* deviceClass, const char* unitOfMeasurement,
                              const char* valueTemplate, const char* stateTopic) {
    StaticJsonDocument<512>  doc = getDeviceConfigurationTemplate();
    doc["device_class"] = deviceClass;
    doc["unit_of_measurement"] = unitOfMeasurement;
    doc["value_template"] = valueTemplate;

    String uid = clientId + "_" + uniqueId;
    doc["unique_id"] = uid;                                             // Concatenate uniqueId
    doc["name"] = name;

    if (stateTopic != "") {
        doc["state_topic"] = "airquality/" + clientId + "/state" + "/" + stateTopic;
    }
    String fullTopic = "homeassistant/sensor/" + clientId + "/" + uniqueId + "/config";
    String jsonOutput = "";
    serializeJson(doc, jsonOutput);
    
    sendJsonToTopic(fullTopic.c_str(), jsonOutput);
}

void sendDeviceConfiguration(){
    sendDeviceTemplate("pm1", "PM1", "pm1", "µg/m³", "{{value_json.pm1}}", "");
    sendDeviceTemplate("pm25", "PM2.5", "pm25", "µg/m³", "{{value_json.pm25}}", "");
    sendDeviceTemplate("pm10", "PM10", "pm10", "µg/m³", "{{value_json.pm10}}", "");

}
void connectToMQTT() {
    
  // Connect to the MQTT broker
    mqtt.begin(MQTT_BROKER_ADRRESS, MQTT_PORT, network);
    mqtt.setOptions(60, true, 512);
    // Create a handler for incoming messages
    mqtt.onMessage(messageHandler);

    Serial.print("Arduino UNO R4 - Connecting to MQTT broker");
    _tft.setCursor(10, 91);
    _tft.setTextColor(ST7735_GREEN);
    _tft.setTextSize(1);
    _tft.print("Connecting to MQTT broker");
    while (!mqtt.connect(MQTT_CLIENT_ID, MQTT_USERNAME, MQTT_PASSWORD)) {
        Serial.print(".");
        delay(100);
    }
    Serial.println();

    if (!mqtt.connected()) {
        Serial.println("Arduino UNO R4 - MQTT broker Timeout!");
        return;
    } else{
        sendDeviceConfiguration();
    }

    // Subscribe to a topic, the incoming messages are processed by messageHandler() function
    if (mqtt.subscribe(SUBSCRIBE_TOPIC))
        Serial.print("Arduino UNO R4 - Subscribed to the topic: ");
    else
        Serial.print("Arduino UNO R4 - Failed to subscribe to the topic: ");

    Serial.println(SUBSCRIBE_TOPIC);
    Serial.println("Arduino UNO R4 - MQTT broker Connected!");
}

void sendToMQTT(uint16_t pm25, uint16_t pm1, RTCTime lastRead) {
  if(!mqtt.connected()){
    Serial.println("connection lost:");
    connectToMQTT();
  }
  if(!network.connected()){
    connectToWiFi();
  }
  StaticJsonDocument<512> doc;
  Serial.println(lastRead.getUnixTime());
  doc["timestamp"] = lastRead.getUnixTime();
  doc["pm25"] = pm25;
  doc["pm1"] = pm1;
  char jsonOutput[512];
  serializeJson(doc, jsonOutput);

  //mqtt.publish(PUBLISH_TOPIC, jsonOutput);

  Serial.println("Arduino UNO R4 - sent to MQTT:");
  Serial.print("- topic: ");
  Serial.println(PUBLISH_TOPIC);
  Serial.print("- payload:");
  Serial.println(jsonOutput);
}

