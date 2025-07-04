#ifndef PMS_H
#define PMS_H

#include "Stream.h"

class PMS
{
public:
  static const uint16_t SINGLE_RESPONSE_TIME = 1000;
  static const uint16_t TOTAL_RESPONSE_TIME = 1000 * 10;
  static const uint16_t STEADY_RESPONSE_TIME = 1000 * 30;

  static const uint16_t BAUD_RATE = 9600;

  struct DATA
  {
    // Standard Particles, CF=1
    uint16_t PM_SP_UG_1_0;
    uint16_t PM_SP_UG_2_5;
    uint16_t PM_SP_UG_10_0;

    // Atmospheric environment
    uint16_t PM_AE_UG_1_0;
    uint16_t PM_AE_UG_2_5;
    uint16_t PM_AE_UG_10_0;

    // Particle counts per 0.1L air
    uint16_t PARTICLE_0_3UM;
    uint16_t PARTICLE_0_5UM;
    uint16_t PARTICLE_1_0UM;
    uint16_t PARTICLE_2_5UM;
    uint16_t PARTICLE_5_0UM;
    uint16_t PARTICLE_10_0UM;

    // Reserved bytes (optional, if you want to access them)
    uint16_t RESERVED1;
    uint16_t RESERVED2;
  };

  PMS(Stream &);
  void sleep();
  void wakeUp();
  void activeMode();
  void passiveMode();

  void requestRead();
  bool read(DATA &data);
  bool readUntil(DATA &data, uint16_t timeout = SINGLE_RESPONSE_TIME);

private:
  enum STATUS
  {
    STATUS_WAITING,
    STATUS_OK
  };
  enum MODE
  {
    MODE_ACTIVE,
    MODE_PASSIVE
  };

  uint8_t _payload[32];
  Stream *_stream;
  DATA *_data;
  STATUS _status;
  MODE _mode = MODE_ACTIVE;

  uint8_t _index = 0;
  uint16_t _frameLen;
  uint16_t _checksum;
  uint16_t _calculatedChecksum;

  void loop();
};

#endif