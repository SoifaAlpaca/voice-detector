#include "config.h"

bool no_mqtt  = false;
bool no_sleep = false;

RTC_DATA_ATTR unsigned int sleep_time = 20000;
RTC_DATA_ATTR float        volume     = 0.6;
RTC_DATA_ATTR uint8_t      led_status = HIGH;