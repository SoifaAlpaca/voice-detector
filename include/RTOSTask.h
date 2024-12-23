#ifndef RTOSTASK_H
#define RTOSTASK_H

#include <esp_sleep.h>
#include "freertos/timers.h"
#include "MQTT.h"

SemaphoreHandle_t sem_sleep; 

TimerHandle_t timer_sleep   = NULL;
TaskHandle_t  handle_main   = NULL;
TaskHandle_t  handle_listen = NULL;
TaskHandle_t  handle_mqtt   = NULL;

void ThreadsSetup       ();
void stop_timer         ();
void start_timer        ();
void stop_timer         ();
void restart_timer      ();
void start_listen_thread();
void MainThread         (void* args);
void Listen             (void* args);
void task_MQTT_status   (void* args);
void mqtt_update        (const char*status);
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
    Serial.println("Timeout");
    xSemaphoreGive(sem_sleep);
}

void MainThread(void* args){

    while(true){
        
        //sleep
        //send status  ||
        mqtt_update(status_listen); 
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
        update_status(status_idle);

        esp_deep_sleep_start();
        
    }
}

void Listen(void* args){
    
    //FIXME caga o mqtt somehow
    //Wait for word 
    //stop timer
    vTaskDelay(40000);
    stop_timer();
    //status playing
    //play
    //Sleep
    //TODO TIRAR O DELAY
    
    xSemaphoreGive(sem_sleep);
    handle_listen = NULL;
    vTaskDelete(NULL);
}

void mqtt_update( const char*status ){

    while( handle_mqtt != NULL)
        vTaskDelay(11);
        //vTaskDelete(handle_mqtt);
    
    xTaskCreate(
        task_MQTT_status,
        "MQTT Status Thread",
        8192, 
        (void*)status, 
        5, 
        &handle_mqtt
    );
}

void task_MQTT_status(void* args){

    const char* status = (const char*) args;
    update_status(status);
    handle_mqtt = NULL;
    vTaskDelete(NULL);
}

void stop_timer(){
    if( timer_sleep != NULL){
        xTimerDelete(timer_sleep, 0);
        timer_sleep = NULL;
    }
}

void start_timer(){
    if( timer_sleep != NULL){ return; }

    timer_sleep = xTimerCreate(
        "Sleep Timer",               
        pdMS_TO_TICKS(sleep_time),     
        pdTRUE,                  
        NULL,               
        timerCallBack    
    );
    Serial.println(sleep_time);
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

#endif