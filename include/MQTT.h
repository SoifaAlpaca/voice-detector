#ifndef MQTT_H
#define MQTT_H

#define LOG_MESSAGE_MQTT 1
#define GRAFANA          1 //whether or not is using grafana or Home Assistant 

#if GRAFANA
    extern char* status_idle;
    extern char* status_listen;
    extern char* status_playing;
#else
    extern char* status_idle;
    extern char* status_listen;
    extern char* status_playing;
#endif


void update_status(const char* status);
void init_mqtt();
void mqtt_loop();

#endif