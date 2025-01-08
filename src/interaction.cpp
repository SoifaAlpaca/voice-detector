#include "interaction.h"

#include "config.h"
#include "RTOSTask.h"
#include "MQTT.h"

String com_sleep     = "SLEEP";
String com_volume    = "VOLUME";
String com_set_sleep = "TIME_SLEEP";
String com_no_sleep  = "NO_SLEEP";
String com_set_led   = "LED";
String com_set_mqtt  = "MQTT";

unsigned int get_number       (String s);
void         update_sleep_time(unsigned int Time);
void         set_volume       (unsigned int volume );
void         set_led          (int i);
void         set_sleep();

void update_sleep_time(unsigned int Time){
    if( Time <= 0 || Time*1000 > TIME_MAX){
        return;
    };
    sleep_time = Time*1000;    
    restart_timer();
}

void set_volume( unsigned int v ){
    float vol = (float)((float)v)/100;
    vol = vol < 0 ? 0 : vol;
    vol = vol > 1 ? 1 : vol;
    volume = vol;
    Serial.print("Volume was changed to: ");
    Serial.println(v);
}

void set_led(int i){
    led_status = i ? HIGH : LOW;
    digitalWrite(PIN_LED, led_status);

}

void set_sleep(){

    xSemaphoreGive(sem_sleep);
}

void read_command( String command ){
    Serial.print("Command Received: ");
    Serial.println(command);
    if( command.startsWith(com_sleep)){
        set_sleep();   
    }else if( command.startsWith(com_volume) ){
        
        set_volume( get_number(command) );
        mqtt_update();
    }else if( command.startsWith(com_set_sleep) ){
        
        update_sleep_time(get_number(command));
    }else if( command.startsWith(com_no_sleep) ){
        
        int i = get_number(command);
        if(i){
            no_sleep = true;
            stop_timer();

        }else{
            no_sleep = false;
            start_timer();            
        }
    }else if( command.startsWith(com_set_led) ){
        
        set_led(get_number(command));
        mqtt_update();
    }else if( command.startsWith(com_set_mqtt) ){       
        
        no_mqtt = get_number(command) ? LOW : HIGH;
        if(no_mqtt){
            client.disconnect();
        }else{
            init_mqtt();
        }

    }
}

unsigned int get_number(String s){
    // Check if the string ends with digits
    int len = s.length();
    int i = len - 1;

    while( s[i] == '\n' )
        --i;

    // Find the position where the digits start from the end
    while (i >= 0 && isDigit(s[i])) {
        i--;
    }

    // Extract the number part
    if (i < len - 1) {
        String numberPart = s.substring(i + 1);
        return numberPart.toInt(); // Convert to integer
    }
    return 0;
}
