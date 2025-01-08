#include <Arduino.h>
#include <esp_task_wdt.h>

#include "RTOSTask.h"
#include "MQTT.h"
#include "interaction.h"
#include "SPIFFS.h"

#include "config.h"

using namespace std;
void setup(){
    
    Serial.begin(115200);

    //Needs to be the last one
    esp_task_wdt_init(10, false);
    init_mqtt();
    pinMode(PIN_LED, OUTPUT);
    digitalWrite(PIN_LED, led_status);
    esp_sleep_enable_ext0_wakeup(WAKEUP_PIN, 1); 
    ThreadsSetup();
}



void loop() {
    
    mqtt_loop();

    String command = Serial.readString();

    if( command != ""){
        read_command(command);
    }
}
