#ifndef INTERACTION_H
#define INTERACTION_H

#include <Arduino.h>

extern String com_sleep;    
extern String com_volume;   
extern String com_set_sleep;
extern String com_no_sleep; 
extern String com_set_led;  
extern String com_set_mqtt; 

void read_command(String command );

#endif