#ifndef MQTT_H
#define MQTT_H

#include <WiFi.h>
#include <PubSubClient.h>
#include "Creds.h"

#define LOG_MESSAGE_MQTT 1
#define GRAFANA          1 //

const char* topic_status    = "ems/t1/g1";

const char* status_idle     = "0";
const char* status_listen   = "1";
const char* status_playing  = "2";

WiFiClient   espClient;
PubSubClient client(espClient);

void callback(char* topic, byte* message, unsigned int length);
void init_mqtt();
void init_wifi();
void reconnect() ;
void update_status(const char* status);


void init_mqtt(){
    
    if( no_mqtt )
        return;

    if(!WiFi.isConnected()){
        init_wifi();
    }
    client.setServer(mqtt_server, mqttPort);
    client.setCallback(callback);

}

void init_wifi(){
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
        vTaskDelay(500);
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

    if( no_mqtt )
        return;

    if(!WiFi.isConnected()){
        init_wifi();
    }
    if (!client.connected()) {
        reconnect();
    }
    client.loop();
    char message[100] = "audio,source=esp32 status=";
    strcat(message,status );
    char aux[17]     = ",Volume=";
    String saux((int) (volume * 100.0));
    strcat(aux,saux.c_str());
    strcat(message,aux);

    char aux2[17]     = ",TimeToSleep=";
    String saux2((int) sleep_time / 1000);
    strcat(aux2,saux2.c_str());
    strcat(message,aux2);
    
    char aux3[14]     = ",EnableLed=";
    char c[2]  = " ";
    c[0] = '0' + led_status;
    strcat(aux3,c);
    strcat(message,aux3);

    client.publish(topic_status, message);
    vTaskDelay(100);
    Serial.println(message);
    //client.disconnect();
    //WiFi.disconnect();
}


#endif