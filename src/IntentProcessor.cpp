#include <Arduino.h>
#include "IntentProcessor.h"
#include "Speaker.h"

IntentProcessor::IntentProcessor(Speaker *speaker)
{
    m_speaker = speaker;
}

IntentResult IntentProcessor::tellJoke()
{
    m_speaker->playJoke();
    return SUCCESS;
}

IntentResult IntentProcessor::playMusic()
{
    m_speaker->playMusic();
    return SUCCESS;
}

IntentResult IntentProcessor::stopMusic()
{
    m_speaker->stop();
    return SUCCESS;
}

IntentResult IntentProcessor::processIntent(const Intent &intent)
{
    if (intent.text.empty())
    {
        Serial.println("No text recognised");
        return FAILED;
    }
    Serial.printf("I heard \"%s\"\n", intent.text.c_str());
    if (intent.intent_name.empty())
    {
        Serial.println("Can't work out what you want...");
        return FAILED;
    }
    Serial.printf("Intent is %s\n", intent.intent_name.c_str());

    if (intent.intent_name == "Tell_joke")
    {
        return tellJoke();
    }
    if (intent.intent_name == "Play_music")
    {
        return playMusic();
    }
    if (intent.intent_name == "Stop_music")
    {
        return stopMusic();
    }

    return FAILED;
}
