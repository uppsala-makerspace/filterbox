#include "wifistats.h"


WiFiClient wifiClient; // Remove this if now declared in mqttutils.cpp

void connectToWiFi()
{
  Serial.print("Connecting to WiFi");
  WiFi.begin(SECRET_SSID, SECRET_PASS);
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20)
  {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  if (WiFi.status() == WL_CONNECTED)
  {
    // Wait for a valid IP address
    IPAddress ip = WiFi.localIP();
    int ipWait = 0;
    while (ip[0] == 0 && ipWait < 20)
    { // Wait up to 10 seconds
      delay(500);
      ip = WiFi.localIP();
      ipWait++;
    }
    Serial.println("\nWiFi connected. IP address: " + ip.toString());
  }
  else
  {
    Serial.println("\nWiFi connection failed.");
  }
}

bool isWiFiConnected()
{
  return WiFi.status() == WL_CONNECTED;
}

void reconnectWiFi()
{
  Serial.println("Reconnecting WiFi...");
  WiFi.disconnect();
  delay(1000);
  connectToWiFi();
}