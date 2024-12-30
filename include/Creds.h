#ifndef CREDS_H
#define CREDS_H

#include <Arduino.h>

extern const char* ssid; 
extern const char* password; 
extern const char* mqttpass; 
extern const char* mqttuser; 
extern const char* mqttClientID; 
extern const char* mqtt_server; 
extern const unsigned int mqttPort; 

//FIXME por no header certo
#define WAKEUP_PIN GPIO_NUM_33
#define PIN_LED    32
#define TIME_MAX   (2*60*1000)

extern bool no_mqtt;  // Declare with extern
extern bool no_sleep;

extern RTC_DATA_ATTR unsigned int sleep_time;
extern RTC_DATA_ATTR float        volume;
extern RTC_DATA_ATTR uint8_t      led_status;

#endif