#ifndef SLEEP_H
#define SLEEP_H

#include <esp_sleep.h>

#define WAKEUP_PIN GPIO_NUM_33

void init_sleep(){

    esp_sleep_enable_ext0_wakeup(WAKEUP_PIN, 1); 
    esp_deep_sleep_start();
}


#endif