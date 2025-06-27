#include <WiFiUdp.h>
#include <NTPClient.h>
#include <RTC.h>

WiFiUDP ntpUDP;                                         // UDP instance for NTPClient
NTPClient timeClient(ntpUDP, "pool.ntp.org", 0, 60000); // UTC timezone, update every 60s

void initRTCandNTP()
{
  RTC.begin();

  Serial.println("Starting NTP...");
  timeClient.begin();
  timeClient.update();

  if (timeClient.isTimeSet())
  {
    RTCTime timeToSet = RTCTime(timeClient.getEpochTime());
    RTC.setTime(timeToSet);
    RTCTime currentTime;
    RTC.getTime(currentTime);

    Serial.print("RTC set to NTP epoch time: ");
    Serial.println(currentTime);
  }
  else
  {
    Serial.println("NTP update failed");
  }
}

void updateNTPtime()
{
  timeClient.update();

  if (timeClient.isTimeSet())
  {
    RTCTime timeToSet = RTCTime(timeClient.getEpochTime());
    RTC.setTime(timeToSet);
    RTCTime currentTime;
    RTC.getTime(currentTime);

    Serial.print("RTC set to NTP epoch time: ");
    Serial.println(currentTime);
  }
  else
  {
    Serial.println("NTP update failed");
  }
}

String getFormattedRTCTime()
{
  RTCTime currentTime;
  RTC.getTime(currentTime);
  unsigned long epoch = currentTime.getUnixTime();

  int hour = (epoch % 86400L) / 3600;
  int minute = (epoch % 3600) / 60;
  int second = epoch % 60;

  char buf[9];
  snprintf(buf, sizeof(buf), "%02d:%02d:%02d", hour, minute, second);
  return String(buf);
}
