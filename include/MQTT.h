#ifndef MQTT_H
#define MQTT_H

#include <WiFi.h>
#include <PubSubClient.h>
#include "Creds.h"

#define LOG_MESSAGE_MQTT 1

const char* topic_status    = "esp32/status";

const char* status_idle     = "Idle";
const char* status_listen   = "Listening";
const char* status_playing  = "Playing";

WiFiClient   espClient;
PubSubClient client(espClient);

void callback(char* topic, byte* message, unsigned int length);
void init_mqtt();
void init_wifi();
void reconnect() ;
void update_status(const char* status);


void init_mqtt(){

    init_wifi();
    client.setServer(mqtt_server, mqttPort);
    client.setCallback(callback);

}

void init_wifi(){
    delay(10);
    // We start by connecting to a WiFi network
    #if LOG_MESSAGE_MQTT
        Serial.println();
        Serial.print("Connecting to ");
        Serial.println(ssid);
    #endif
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        #if LOG_MESSAGE_MQTT
            Serial.print(".");
        #endif
        delay(500);
    }

    #if LOG_MESSAGE_MQTT
        Serial.println();
        Serial.println("WiFi connected");
        Serial.println("IP address: ");
        Serial.println(WiFi.localIP());
    #endif

}

void callback(char* topic, byte* message, unsigned int length) {
    
    return ;
    
    #if LOG_MESSAGE_MQTT
        Serial.print("Message arrived on topic: ");
        Serial.print(topic);
        Serial.print(". Message: ");
    #endif
    String messageTemp;

    for (int i = 0; i < length; i++) {
        #if LOG_MESSAGE_MQTT
            Serial.print((char)message[i]);
        #endif
        messageTemp += (char)message[i];
    }
}


void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    #if LOG_MESSAGE_MQTT
        Serial.print("Attempting MQTT connection...");
    #endif
    // Attempt to connect
    if (client.connect(mqttClientID,mqttuser,mqttpass)) {
        #if LOG_MESSAGE_MQTT
          Serial.println("connected");
        #endif
      // Subscribe
      //client.subscribe(Topic);
      
    } else {
        #if LOG_MESSAGE_MQTT
          Serial.print("failed, rc=");
          Serial.print(client.state());
          Serial.println(" try again in 5 seconds");
        #endif
      // Wait 5 seconds before retrying
      vTaskDelay(500);
    }
  }
}


void update_status(const char* status){

    init_mqtt();    
    if (!client.connected()) {
        reconnect();
    }
    client.loop();

    client.publish(topic_status, status);
    WiFi.disconnect();
}


#endif