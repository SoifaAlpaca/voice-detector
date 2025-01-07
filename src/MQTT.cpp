
#include "MQTT.h"
#include "Creds.h"
#include "config.h"

#if GRAFANA

char *status_idle       = (char *)"0";
char *status_listen     = (char *)"1";
char *status_playing    = (char *)"2";

const char *topic_status = "ems/t1/g1";
#else

#include "interaction.h"

char *status_idle       = (char *)"Idle";
char *status_listen     = (char *)"Listening";
char *status_playing    = (char *)"Playing Music";

const char *topic_status        = "ems/g1/t1/status";
const char *topic_volume_state  = "ems/t1/g1/volume/state";
const char *topic_volume_set    = "ems/t1/g1/volume/set";
const char *topic_led_state     = "ems/t1/g1/enableled/state";
const char *topic_led_set       = "ems/t1/g1/enableled/set";
const char *topic_time_state    = "ems/t1/g1/timesleep/state";
const char *topic_time_set      = "ems/t1/g1/timesleep/set";
const char *topic_sleep         = "ems/t1/g1/sleep";
#endif

void callback(char *topic, byte *message, unsigned int length);
void reconnect();

void mqtt_loop(){
    if (!WiFi.isConnected()){
        init_wifi();
    }
    
    if (!client.connected()){
        reconnect();
    }
    client.loop();
}

void init_mqtt(){

    if (no_mqtt)
        return;

    if (!WiFi.isConnected()){
        init_wifi();
    }

    client.setServer(mqtt_server, mqttPort);
    client.setCallback(callback);
    reconnect();
}

void init_wifi(){
    // We start by connecting to a WiFi network

    #if LOG_MESSAGE_MQTT
        Serial.println();
        Serial.print("Connecting to ");
        Serial.println(ssid);
    #endif
        WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED){
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

void callback(char *topic, byte *message, unsigned int length){

    #if GRAFANA
        return;
    #else
    #if LOG_MESSAGE_MQTT
        Serial.print("Message arrived on topic: ");
        Serial.print(topic);
        Serial.print(". Message: ");
    #endif
        String messageTemp;

        for (int i = 0; i < length; i++){
            #if LOG_MESSAGE_MQTT
                Serial.print((char)message[i]);
            #endif
                messageTemp += (char)message[i];
        }
        Serial.println();
        String command;
        if (!strcmp(topic, topic_led_set)){
            command = "LED=" + messageTemp;
        }
        else if (!strcmp(topic, topic_time_set)){
            command = "TIME_SLEEP=" + messageTemp;
        }
        else if (!strcmp(topic, topic_volume_set)){
            command = "VOLUME=" + messageTemp;
        }
        else if (!strcmp(topic, topic_sleep)){
            command = "SLEEP";
        }
        read_command(command);
    #endif
}

void reconnect(){
    // Loop until we're reconnected
    if(no_mqtt)
        return;

    while (!client.connected()){
        #if LOG_MESSAGE_MQTT
            Serial.print("Attempting MQTT connection...");
        #endif
        // Attempt to connect
        if (client.connect(mqttClientID, mqttuser, mqttpass)){
            #if LOG_MESSAGE_MQTT
                Serial.println("connected");
            #endif
            // Subscribe
            #if !GRAFANA
                client.subscribe(topic_time_set);
                client.subscribe(topic_volume_set);
                client.subscribe(topic_led_set);
                client.subscribe(topic_sleep);
                // client.subscribe(Topic);
            #endif
        }else{
            #if LOG_MESSAGE_MQTT
                Serial.print("failed, rc=");
                Serial.print(client.state());
                Serial.println(" try again in 5 seconds");
            #endif
            // Wait 5 seconds before retrying
            vTaskDelay(50);
        }
    }
}

void update_status(const char *status){

    if (no_mqtt)
        return;

    #if GRAFANA
        char message[100] = "audio,source=esp32 status=";
        strcat(message, status);
        char aux[17] = ",Volume=";
        String saux((int)(volume * 100.0));
        strcat(aux, saux.c_str());
        strcat(message, aux);

        char aux2[17] = ",TimeToSleep=";
        String saux2((int)sleep_time / 1000);
        strcat(aux2, saux2.c_str());
        strcat(message, aux2);

        char aux3[14] = ",EnableLed=";
        char c[2] = " ";
        c[0] = '0' + led_status;
        strcat(aux3, c);
        strcat(message, aux3);

        client.publish(topic_status, message);
        vTaskDelay(100);
        Serial.println(message);
        // client.disconnect();
        // WiFi.disconnect();
    #else
        client.publish(topic_status, status);
        String saux((int)(volume * 100.0));
        client.publish(topic_volume_state, saux.c_str());
        client.publish(topic_led_state, led_status ? "On" : "Off");
        String saux2(sleep_time / 1000);
        client.publish(topic_time_state, saux2.c_str());

    #endif
}
