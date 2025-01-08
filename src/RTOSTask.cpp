#include <esp_sleep.h>
#include "Arduino.h"

#include "Listen.h"
#include "I2SOutput.h"
#include "I2SMicSampler.h"
#include "Speaker.h"
#include "IndicatorLight.h"
#include "IntentProcessor.h"
#include "RTOSTask.h"
#include "config.h"
#include "MQTT.h"

SemaphoreHandle_t sem_sleep;
SemaphoreHandle_t sem_stop_playing; 

TimerHandle_t timer_sleep       = NULL;
TaskHandle_t  handle_main       = NULL;
TaskHandle_t  handle_listen     = NULL;
TaskHandle_t  listenTaskHandle  = NULL;
TaskHandle_t  handle_mqtt       = NULL;

char* current_status;

void MainThread      (void* args);
void ListenThread    (void* args);
void task_MQTT_status(void* args);
void listenTask      (void *param);
void timerCallBack   (TimerHandle_t xTimer);
void mqtt_update();

void ThreadsSetup(){
    sem_sleep        = xSemaphoreCreateCounting(1, 0);
    sem_stop_playing = xSemaphoreCreateCounting(1, 0);;

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

        /*********** Listen Set up ***********/
        
        if (!WiFi.isConnected()){
            init_wifi();
        }
        Serial.printf("Total heap: %d\n", ESP.getHeapSize());
        Serial.printf("Free heap: %d\n", ESP.getFreeHeap());

        // start up the I2S input (from either an I2S microphone or Analogue microphone via the ADC)
        // Direct i2s input from INMP441 or the SPH0645
        I2SSampler *i2s_sampler = new I2SMicSampler(i2s_mic_pins);

        // start the i2s speaker output
        I2SOutput *i2s_output = new I2SOutput();
        i2s_output->start(I2S_NUM_1, i2s_speaker_pins);
        Speaker *speaker = new Speaker(i2s_output);

        // indicator light to show when we are listening
        IndicatorLight *indicator_light = new IndicatorLight();

        // and the intent processor
        IntentProcessor *intent_processor = new IntentProcessor(speaker);

        // create our application
        Listen *listen = new Listen(i2s_sampler, intent_processor, speaker, indicator_light);

        TaskHandle_t listenTaskHandle;
        xTaskCreate(listenTask, "Listening Task", 8192, listen, 1, &listenTaskHandle);
        
        start_listen_thread();
         // start sampling from i2s device - use I2S_NUM_0 as that's the one that supports the internal ADC
        i2s_sampler->start(I2S_NUM_0, i2sMemsConfigBothChannels, handle_listen);

        /*********** End Listen Set up ***********/

        //Wait for Semaphore
        while(xSemaphoreTake(sem_sleep, portMAX_DELAY) != pdTRUE){
            vTaskDelay(pdMS_TO_TICKS(1));
        }
        if( no_sleep ){ 
            continue;
        }
        
        Serial.println("Going To Sleep!");
        current_status = status_idle;
        update_status(current_status);

        esp_deep_sleep_start();
        
    }
}

void ListenThread(void* args){
    while(true){
        
        while(listening){
            vTaskDelay(10);
        }

        vTaskDelay(100);
        Serial.println("DEBUG!!!");
        Serial.println(listening);
        stop_timer();        
        current_status = status_playing;
        mqtt_update();
        
        while(playing){
            vTaskDelay(10);
        }
        vTaskDelay(100);
        Serial.println("DEBUG!!!");
        Serial.println(playing);
        
        start_timer();
    }
    handle_listen = NULL;
    vTaskDelete(NULL);
}

// This task does all the heavy lifting for our application
void listenTask(void *param){

    Listen *listen = static_cast<Listen *>(param);

    while (true){

        listen->run();
    }
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
        ListenThread,
        "Listen Thread",
        2048, 
        NULL, 
        5, 
        &handle_listen
    );
}
