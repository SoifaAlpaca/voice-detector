#ifndef ADC_H
#define ADC_H
#include <driver/i2s.h>

// I2S
#define I2S_SAMPLE_RATE (10000)
#define TIME_SAMPLE (1 / (I2S_SAMPLE_RATE))
#define I2S_DMA_BUF_LEN (1024)
#define ADC_MAX (1 << 12)
#define ANALOG_ADC (ADC1_CHANNEL_6)
#define GAIN (1)
// Values in mV
#define OFFSET (0)
#define ADC_MAX_VOLT ((double)950)
#define ADC_MIN_VOLT ((double)150)

double uint12_to_Volt(uint16_t val);
void i2sInit();

void i2sInit()
{

    i2s_config_t i2s_config = {
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX | I2S_MODE_ADC_BUILT_IN),
        .sample_rate = I2S_SAMPLE_RATE,               // The format of the signal using ADC_BUILT_IN
        .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT, // is fixed at 12bit, stereo, MSB
        .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
        .communication_format = i2s_comm_format_t(I2S_COMM_FORMAT_STAND_I2S), // I2S_COMM_FORMAT_I2S_MSB,
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
        .dma_buf_count = 8,
        .dma_buf_len = I2S_DMA_BUF_LEN,
        .use_apll = false,
        .tx_desc_auto_clear = false,
        .fixed_mclk = 0};

    // adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(ANALOG_ADC, ADC_ATTEN_DB_0);
    i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
    i2s_set_adc_mode(ADC_UNIT_1, ANALOG_ADC);
    i2s_adc_enable(I2S_NUM_0);
}

// val must be 12 bit number
double uint12_to_Volt(uint16_t val)
{

    val = val & 0x0FFF; // uint16 -> uint12
    // Real Voltage in ADC mV
    double adcV = ADC_MIN_VOLT + ((double)val) * ((ADC_MAX_VOLT - ADC_MIN_VOLT) / (double)ADC_MAX);
    return 1e3 /*To Micro Volt*/ * (adcV - OFFSET) / GAIN /*AFE Gain and Offset*/;
}

#endif