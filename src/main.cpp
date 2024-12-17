#include <Arduino.h>
#include "RTOSTask.h"
#include "MQTT.h"

void setup() {
    
    Serial.begin(115200);

    const char* s = "Penis";
    //Needs to be the last one
    //esp_sleep_enable_ext0_wakeup(WAKEUP_PIN, 1); 

}

void loop() {
    
    Serial.println("What");    
    xTaskCreate(
            task_MQTT,
            "MQTT_Thread",
            2048, 
            (void*)status_idle, 
            5, 
            &handle_mqtt
        );
    xTaskCreate(
            task_MQTT,
            "MQTT_Thread",
            2048, 
            (void*)status_listen, 
            5, 
            &handle_mqtt
        );
    vTaskDelay(pdMS_TO_TICKS(10000));
    
}
