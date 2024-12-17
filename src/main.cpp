#include <Arduino.h>
#include "RTOSTask.h"
#include "MQTT.h"

void setup(){
    
    Serial.begin(115200);

    //Needs to be the last one
    esp_sleep_enable_ext0_wakeup(WAKEUP_PIN, 1); 

}

void loop() {
    
    vTaskDelay(10000);
    
}
