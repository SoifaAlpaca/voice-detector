#ifndef _intent_processor_h_
#define _intent_processor_h_

#include <map>
#include "WitAiChunkedUploader.h"

class Speaker;

enum IntentResult
{
    FAILED,
    SUCCESS,
    SILENT_SUCCESS // success but don't play ok sound
};

class IntentProcessor
{
private:
    IntentResult tellJoke();
    IntentResult playMusic();
    IntentResult stopMusic();

    Speaker *m_speaker;

public:
    IntentProcessor(Speaker *speaker);
    IntentResult processIntent(const Intent &intent);
};

#endif
