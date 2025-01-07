#include "I2SMicSampler.h"
#include "driver/i2s.h"
#include "soc/i2s_reg.h"

I2SMicSampler::I2SMicSampler(i2s_pin_config_t &i2sPins) : I2SSampler()
{
    m_i2sPins = i2sPins;
}

void I2SMicSampler::configureI2S()
{
    i2s_set_pin(getI2SPort(), &m_i2sPins);
}

void I2SMicSampler::processI2SData(uint8_t *i2sData, size_t bytesRead)
{
    int32_t *samples = (int32_t *)i2sData;
    for (int i = 0; i < bytesRead / 4; i++)
    {
        addSample(samples[i] >> 11);
    }
}
