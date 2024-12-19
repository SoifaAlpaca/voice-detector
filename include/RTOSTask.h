#ifndef RTOSTASK_H
#define RTOSTASK_H

#include <esp_sleep.h>
#include "freertos/timers.h"
#include "MQTT.h"

#define WAKEUP_PIN GPIO_NUM_33
#define SLEEP_TIME (10000)

SemaphoreHandle_t sem_sleep; 

TimerHandle_t timer_sleep;
TaskHandle_t  handle_main;
TaskHandle_t  handle_listen;
TaskHandle_t  handle_mqtt;

void timerCallBack  (TimerHandle_t xTimer);
void ThreadsSetup   ();
void MainThread     (void* args);
void Listen         (void* args);
void task_MQTT      (void* args);


void ThreadsSetup(){
    sem_sleep = xSemaphoreCreateCounting(1, 0);
    
    timer_sleep = xTimerCreate(
        "Sleep Timer",               
        pdMS_TO_TICKS(SLEEP_TIME),     
        pdTRUE,                  
        NULL,               
        timerCallBack    
    );
    xTaskCreate(
            MainThread,
            "Main Thread",
            2048, 
            NULL, 
            5, 
            &handle_main
        );

}


void timerCallBack(TimerHandle_t xTimer){

    xSemaphoreGive(sem_sleep);
}

void MainThread(void* args){

    while(true){
        
        //sleep
        esp_deep_sleep_start();
        //send status  ||
        // TODO ver se é preciso vTaskDelete(handle_mqtt);
        xTaskCreate(
            task_MQTT,
            "MQTT Status Thread",
            2048, 
            (void*)status_listen, 
            5, 
            &handle_mqtt
        );
        //timer  ||
        xTimerStart(timer_sleep, 0);
        //listen ||
        xTaskCreate(
            Listen,
            "Listen Thread",
            2048, 
            NULL, 
            5, 
            &handle_listen
        );
        //Wait for Semaphore
        while(xSemaphoreTake(sem_sleep, portMAX_DELAY) != pdTRUE){
            vTaskDelay(pdMS_TO_TICKS(1));
        }
        //FIXME Esta funcao deve demorar se calhar fazer um timer pra cagar no mqtt
        //TODO ver se é preciso vTaskDelete(handle_mqtt);
        update_status(status_idle);

        //kill all tasks
        if( handle_listen != NULL){
            vTaskDelete(handle_listen);
            handle_listen = NULL;
        }
    }
}

void Listen(void* args){
    
    //Wait for word 
    //stop timer
    xTimerDelete(timer_sleep, 0);
    //status playing
    xTaskCreate(
        task_MQTT,
        "MQTT Status Thread",
        2048, 
        (void*)status_playing, 
        5, 
        &handle_mqtt
    );
        //play
    //Sleep
    xSemaphoreGive(sem_sleep);
    handle_listen = NULL;
    vTaskDelete(NULL);
}

void task_MQTT(void* args){
    
    const char* status = (const char*) args;
    Serial.println("Test");
    //update_status(status);
    handle_mqtt = NULL;
    vTaskDelete(NULL);
}

#endif