#include "Speaker.h"
#include "I2SOutput.h"
#include "WAVFileReader.h"

Speaker::Speaker(I2SOutput *i2s_output)
{
    m_i2s_output = i2s_output;
    m_ok = new WAVFileReader("/ok.wav");
    m_ready_ping = new WAVFileReader("/ready_ping.wav");
    m_cantdo = new WAVFileReader("/cantdo.wav");
    m_joke = new WAVFileReader("/joke.wav");
    m_music = new WAVFileReader("/music.wav");
}

Speaker::~Speaker()
{
    delete m_ok;
    delete m_ready_ping;
    delete m_cantdo;
    delete m_joke;
    delete m_music;
}

void Speaker::playOK()
{
    m_ok->reset();
    m_i2s_output->setSampleGenerator(m_ok);
}

void Speaker::playReady()
{
    m_ready_ping->reset();
    m_i2s_output->setSampleGenerator(m_ready_ping);
}

void Speaker::playCantDo()
{
    m_cantdo->reset();
    m_i2s_output->setSampleGenerator(m_cantdo);
}

void Speaker::playJoke()
{
    m_joke->reset();
    m_i2s_output->setSampleGenerator(m_joke);
}

void Speaker::playMusic()
{
    m_music->reset();
    m_i2s_output->setSampleGenerator(m_music);
}

void Speaker::stop()
{
    m_i2s_output->setSampleGenerator(NULL);
}