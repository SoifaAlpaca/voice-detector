#include <esp_sleep.h>
#include "Arduino.h"

#include "RTOSTask.h"
#include "config.h"
#include "MQTT.h"

SemaphoreHandle_t sem_sleep; 

TimerHandle_t timer_sleep   = NULL;
TaskHandle_t  handle_main   = NULL;
TaskHandle_t  handle_listen = NULL;
TaskHandle_t  handle_mqtt   = NULL;

char* current_status;

void MainThread         (void* args);
void Listen             (void* args);
void task_MQTT_status   (void* args);
void mqtt_update        ();
void timerCallBack      (TimerHandle_t xTimer);

void ThreadsSetup(){
    sem_sleep = xSemaphoreCreateCounting(1, 0);
    
    start_timer();
    xTaskCreate(
            MainThread,
            "Main Thread",
            8192, 
            NULL, 
            5, 
            &handle_main
        );
}


void timerCallBack(TimerHandle_t xTimer){
    Serial.println("Timer: Timeout");
    xSemaphoreGive(sem_sleep);
}

void MainThread(void* args){

    while(true){
        
        //sleep
        //send status  ||
        current_status = status_listen;
        mqtt_update(); 
        //timer  ||
        if( !no_sleep ){ 
            start_timer();
        }
        //listen ||
        start_listen_thread();
        //Wait for Semaphore
        while(xSemaphoreTake(sem_sleep, portMAX_DELAY) != pdTRUE){
            vTaskDelay(pdMS_TO_TICKS(1));
        }
        if( no_sleep ){ 
            continue;
        }
        
        Serial.println("Going To Sleep!");
        //FIXME Esta funcao deve demorar se calhar fazer um timer pra cagar no mqtt
        //TODO ver se é preciso vTaskDelete(handle_mqtt);
        current_status = status_idle;
        update_status(current_status);

        esp_deep_sleep_start();
        
    }
}

void Listen(void* args){
    
    //FIXME caga o mqtt somehow
    //Wait for word 
    //stop timer
    vTaskDelay(40000);
    stop_timer();        
    current_status = status_playing;
    mqtt_update();
    //status playing
    //play
    //Sleep
    //TODO TIRAR O DELAY
    
    xSemaphoreGive(sem_sleep);
    handle_listen = NULL;
    vTaskDelete(NULL);
}

void mqtt_update(){

    while( handle_mqtt != NULL)
        vTaskDelay(11);
        //vTaskDelete(handle_mqtt);
    
    xTaskCreate(
        task_MQTT_status,
        "MQTT Status Thread",
        8192, 
        (void*) 0, 
        5, 
        &handle_mqtt
    );
}

void task_MQTT_status(void* args){

    update_status(current_status);
    handle_mqtt = NULL;
    vTaskDelete(NULL);
}

void stop_timer(){
    if( timer_sleep != NULL){
        Serial.println("Timer: Stopped");
        xTimerDelete(timer_sleep, 0);
        timer_sleep = NULL;
    }
}

void start_timer(){
    if( timer_sleep != NULL){ return; }

    Serial.println("Timer: Starting");
    
    timer_sleep = xTimerCreate(
        "Sleep Timer",               
        pdMS_TO_TICKS(sleep_time),     
        pdTRUE,                  
        NULL,               
        timerCallBack    
    );
    xTimerStart(timer_sleep,0);
}

void restart_timer(){
    stop_timer();
    start_timer();
}

void start_listen_thread(){
    xTaskCreate(
        Listen,
        "Listen Thread",
        2048, 
        NULL, 
        5, 
        &handle_listen
    );
}
