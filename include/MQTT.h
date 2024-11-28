#ifndef MQTT_H
#define MQTT_H

#include <WiFi.h>
#include <PubSubClient.h>
#include "Creds.h"

#define LOG_MESSAGE_MQTT 1

WiFiClient espClient;
PubSubClient client(espClient);

void callback(char* topic, byte* message, unsigned int length);

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
        delay(500);
        #if LOG_MESSAGE_MQTT
            Serial.print(".");
        #endif
    }

    #if LOG_MESSAGE_MQTT
        Serial.println();
        Serial.println("WiFi connected");
        Serial.println("IP address: ");
        Serial.println(WiFi.localIP());
    #endif

}

void callback(char* topic, byte* message, unsigned int length) {
  
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
      vTaskDelay(5000);
    }
  }
}

/*
void MqttTask(void *pvParameters){

  char* aux;
  char* OnStr   = "On";
  char* OffStr  = "Off";
  char  stg[2]  = {0};
  long  now;
  long  lastMsg = 0;

  while( true ){

    if (!client.connected()) {
      reconnect();
    }
    client.loop();

    now = millis();
    if(now - lastMsg > SendRate){

      client.publish(TopicState, "On");
      lastMsg = now;
    }

    if( xSemaphoreTake(SendSem, 1000) == pdTRUE){
      
      aux = PumpOn ? OnStr : OffStr;
      client.publish(TopicPumpState, aux);
      
      stg[0] = stage + '0';
      client.publish(TopicWateringStage, stg);
      
      aux = TransOn ? OnStr : OffStr;
      client.publish(TopicTransState, aux);
      
    }
  }
}
*/

#endif