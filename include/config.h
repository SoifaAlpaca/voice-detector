#ifndef _CONFIG_H
#define _CONFIG_H

#include <Arduino.h>
#include <driver/i2s.h>
#include <WiFi.h>
#include <PubSubClient.h>

extern RTC_DATA_ATTR float volume;

// I2S Microphone Settings
#define USE_I2S_MIC_INPUT
// Which channel is the I2S microphone on? I2S_CHANNEL_FMT_ONLY_LEFT or I2S_CHANNEL_FMT_ONLY_RIGHT
#define I2S_MIC_CHANNEL I2S_CHANNEL_FMT_ONLY_RIGHT
// #define I2S_MIC_CHANNEL I2S_CHANNEL_FMT_ONLY_LEFT
#define I2S_MIC_SERIAL_CLOCK GPIO_NUM_26
#define I2S_MIC_LEFT_RIGHT_CLOCK GPIO_NUM_22
#define I2S_MIC_SERIAL_DATA GPIO_NUM_21

// speaker settings
#define I2S_SPEAKER_SERIAL_CLOCK GPIO_NUM_19
#define I2S_SPEAKER_LEFT_RIGHT_CLOCK GPIO_NUM_27
#define I2S_SPEAKER_SERIAL_DATA GPIO_NUM_18

extern i2s_config_t i2sMemsConfigBothChannels;
extern i2s_pin_config_t i2s_mic_pins;
extern i2s_pin_config_t i2s_speaker_pins;

extern WiFiClient espClient;
extern PubSubClient client;

#define WAKEUP_PIN GPIO_NUM_33
#define PIN_LED 32
#define TIME_MAX (2 * 60 * 1000)

extern RTC_DATA_ATTR bool no_mqtt; // Declare with extern
extern bool no_sleep;

extern RTC_DATA_ATTR unsigned int sleep_time;
extern RTC_DATA_ATTR float volume;
extern RTC_DATA_ATTR uint8_t led_status;

#endif