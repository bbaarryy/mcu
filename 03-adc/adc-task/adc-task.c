#include "hardware/adc.h"
#include "adc-task.h"
#include "stdio.h"
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "stdint.h" 

#define GPIO_NUMBER 26
#define ADC_CHANEL_NUMBER 0
#define ADC_CHANEL_INNERTEMPRETURE 4


void adc_task_init()
{
    adc_init();
    adc_gpio_init(GPIO_NUMBER);
    adc_set_temp_sensor_enabled(true);
}

float get_adc_voltage()
{
    adc_select_input(ADC_CHANEL_NUMBER);
    uint16_t voltage_counts = adc_read();
    float voltage_V = voltage_counts * 3.3f/4096;
    return voltage_V;
}

float get_adc_temp()
{
    adc_select_input(ADC_CHANEL_INNERTEMPRETURE);
    uint16_t voltage_counts = adc_read();
    float voltage_V = voltage_counts * 3.3f/4096;
    float temp_C = 27.0f - (voltage_V - 0.706f) / 0.001721f;
    return temp_C;
}
