#ifndef RTOSTASK_H
#define RTOSTASK_H

#include "freertos/timers.h"


extern SemaphoreHandle_t sem_sleep;
extern SemaphoreHandle_t sem_stop_playing; 

extern TimerHandle_t timer_sleep;
extern TaskHandle_t  handle_main;
extern TaskHandle_t  handle_listen;
extern TaskHandle_t  handle_mqtt;

extern char* current_status;

void ThreadsSetup();
void stop_timer();
void start_timer();
void stop_timer();
void restart_timer();
void start_listen_thread();
void mqtt_update();

#endif