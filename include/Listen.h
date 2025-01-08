#ifndef _listen_h_
#define _listen_h_

#include "state_machine/States.h"

extern bool listening;

class I2SSampler;
class I2SOutput;
class State;
class IndicatorLight;
class Speaker;
class IntentProcessor;

class Listen
{
    public:
    
    State *m_detect_wake_word_state;
    State *m_recognise_command_state;
    State *m_current_state;


    Listen(I2SSampler *sample_provider, IntentProcessor *intent_processor, Speaker *speaker, IndicatorLight *indicator_light);
    ~Listen();
    void run();
};

#endif