#include <Arduino.h>
#include <esp_task_wdt.h>

#include "Listen.h"
#include "I2SOutput.h"
#include "I2SMicSampler.h"
#include "Speaker.h"
#include "IndicatorLight.h"
#include "IntentProcessor.h"
#include "RTOSTask.h"
#include "MQTT.h"
#include "interaction.h"
#include "SPIFFS.h"

#include "config.h"

using namespace std;

// This task does all the heavy lifting for our application
void listenTask(void *param)
{

    Listen *listen = static_cast<Listen *>(param);
    const TickType_t xMaxBlockTime = pdMS_TO_TICKS(100);

    while (true)
    {

        // wait for some audio samples to arrive
        uint32_t ulNotificationValue = ulTaskNotifyTake(pdTRUE, xMaxBlockTime);
        if (ulNotificationValue > 0)
        {
            listen->run();
        }
    }
}

void setup()
{

    Serial.begin(115200);
    delay(100);

    init_mqtt();
    pinMode(PIN_LED, OUTPUT);
    digitalWrite(PIN_LED, led_status);
    esp_sleep_enable_ext0_wakeup(WAKEUP_PIN, 1);
    // Needs to be the last one
    esp_task_wdt_init(10, false);

    // startup SPIFFS for the wav files
    SPIFFS.begin();
    /*********** Listen Set up ***********/

    // startup SPIFFS for the wav files
    SPIFFS.begin();
    // make sure we don't get killed for our long running tasks
    esp_task_wdt_init(10, false);

    // start up the I2S input (from either an I2S microphone or Analogue microphone via the ADC)
    // Direct i2s input from INMP441 or the SPH0645
    I2SSampler *i2s_sampler = new I2SMicSampler(i2s_mic_pins);

    // start the i2s speaker output
    I2SOutput *i2s_output = new I2SOutput();
    i2s_output->start(I2S_NUM_1, i2s_speaker_pins);
    Speaker *speaker = new Speaker(i2s_output);

    // indicator light to show when we are listening
    IndicatorLight *indicator_light = new IndicatorLight();

    // and the intent processor
    IntentProcessor *intent_processor = new IntentProcessor(speaker);

    // create our application
    Listen *listen = new Listen(i2s_sampler, intent_processor, speaker, indicator_light);

    // set up the i2s sample writer task
    TaskHandle_t listenTaskHandle;
    xTaskCreatePinnedToCore(listenTask, "Listening Task", 8192, listen, configMAX_PRIORITIES - 1, &listenTaskHandle, 1);

    // start sampling from i2s device - use I2S_NUM_0 as that's the one that supports the internal ADC
    i2s_sampler->start(I2S_NUM_0, i2sMemsConfigBothChannels, listenTaskHandle);

    /*********** End Listen Set up ***********/
    ThreadsSetup();
}

void loop()
{

    mqtt_loop();

    String command = Serial.readString();

    if (command != "")
    {
        read_command(command);
    }
}
