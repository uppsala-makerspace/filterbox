#define MQTT_PORT 1883
#define MQTT_CLIENT_ID "airfilter01"
#define SCALING_FACTOR 0.017
#define INTERCEPT 0
#define HISTORY_SIZE 180
#define PMS_BREAK_INTERVAL 100000 // Interval to read PMS data in milliseconds
#define PMS_WARMUP_TIME 30000     // PMS sensor warmup time in milliseconds
#define FAN_ENABLE_TIME 600000    // Minimum time the fan should be enabled in milliseconds
#define PM25_THRESHOLD 10         // PM2.5 threshold to enable the fan, change as needed
#define LED_BRIGHTNESS 20         // NeoPixel brightness (0-255)
