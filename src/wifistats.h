// Include the RTC library
#include "RTC.h"
//Include the NTP library
#include <NTPClient.h>
#include <WiFiS3.h>
#include <WiFiUdp.h>
#include "arduino_secrets.h" 
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library
#define cs   10
#define dc   9
#define rst  8  // you can also connect this to the Arduino reset
Adafruit_ST7735 _tft = Adafruit_ST7735(cs, dc, rst);
char ssid[] = SECRET_SSID;        // your network SSID (name)
char pass[] = SECRET_PASS;    // your network password (use for WPA, or use as key for WEP)
int wifiStatus = WL_IDLE_STATUS;

WiFiUDP Udp; // A UDP instance to let us send and receive packets over UDP
NTPClient timeClient(Udp);

void printWifiStatus() {

  // print the SSID of the network you're attached to:

  Serial.print("SSID: ");

  Serial.println(WiFi.SSID());


  // print your board's IP address:

  IPAddress ip = WiFi.localIP();

  Serial.print("IP Address: ");

  Serial.println(ip);


  // print the received signal strength:

  long rssi = WiFi.RSSI();

  Serial.print("signal strength (RSSI):");

  Serial.print(rssi);

  Serial.println(" dBm");
}

String getWifiStatus() {
    String a = WiFi.SSID() + WiFi.localIP();
    return a;
}
void connectToWiFi(){
  // check for the WiFi module:
    _tft.setCursor(10, 11);
    _tft.setTextColor(ST7735_GREEN);
    _tft.setTextSize(1);
    _tft.print("connecting to wifi  "); _tft.print(WiFi.SSID());
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    _tft.setCursor(10, 31);
    _tft.setTextColor(ST7735_GREEN);
    _tft.setTextSize(1);
    _tft.print("Connection failed  ");
    // don't continue

    while (true);
  }
  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Please upgrade the firmware");
    _tft.setCursor(10, 31);
    _tft.setTextColor(ST7735_GREEN);
    _tft.setTextSize(1);
    _tft.print("Please upgrade the firmware  ");
  }
  // attempt to connect to WiFi network:
  while (wifiStatus != WL_CONNECTED) {

    Serial.print("Attempting to connect to SSID: ");
    _tft.setCursor(10, 51);
    _tft.setTextColor(ST7735_GREEN);
    _tft.setTextSize(1);
    _tft.print("Attempting to connect");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    wifiStatus = WiFi.begin(ssid, pass);
    // wait 10 seconds for connection:
    delay(10000);

  }

    _tft.setCursor(10, 71);
    _tft.setTextColor(ST7735_GREEN);
    _tft.setTextSize(1);
    _tft.print("Connected to WiFi");
  Serial.println("Connected to WiFi");

  printWifiStatus();

}
void rtcSetup(Adafruit_ST7735 tft){
    _tft =  tft;
    connectToWiFi();
    RTC.begin();
    Serial.println("\nStarting connection to server...");
    timeClient.begin();
    timeClient.update();
    // Get the current date and time from an NTP server and convert

    // it to UTC +2 by passing the time zone offset in hours.

    // You may change the time zone offset to your local one.

    auto timeZoneOffsetHours = 2;
    auto unixTime = timeClient.getEpochTime() + (timeZoneOffsetHours * 3600);
    Serial.print("Unix time = ");
    Serial.println(unixTime);
    RTCTime timeToSet = RTCTime(unixTime);
    RTC.setTime(timeToSet);

    // Retrieve the date and time from the RTC and print them
    RTCTime currentTime;
    RTC.getTime(currentTime); 
  
    Serial.println("The RTC was just set to: " + String(currentTime));
}