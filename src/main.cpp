#include <Arduino.h>
#include "RTOSTask.h"
#include "MQTT.h"
#include "interaction.h"

//TODO TIRAR
#include "Creds.h"

using namespace std;
void setup(){
    
    Serial.begin(115200);

    //Needs to be the last one
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
