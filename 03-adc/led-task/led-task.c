#include "led-task.h"
#include "stdio.h"
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "stdint.h" 

const uint LED_PIN = 25;
uint LED_BLINK_PERIOD_US = 500000;

uint64_t led_ts;
led_state_t led_state;
int last = 0;

void led_task_init(){
    led_ts=0;
    led_state = LED_STATE_BLINK;
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
}

void led_task_handle(){
    switch (led_state){
    case LED_STATE_OFF:
        gpio_put(LED_PIN, 0);
        break;
    case LED_STATE_ON:
        gpio_put(LED_PIN, 1);
        break;
    case LED_STATE_BLINK:
        if (time_us_64() > led_ts){
            led_ts = time_us_64() + (LED_BLINK_PERIOD_US / 2);
            if(last == 0){
                last = 1;
                gpio_put(LED_PIN, 1);
            }       
            else{
                last=0;
                gpio_put(LED_PIN, 0);
            } // ваш код
        }
        break;
    default:
        break;
    }
}

void led_task_state_set(led_state_t state){
    led_state = state;
}

void led_task_set_blink_period_ms(int per){
    LED_BLINK_PERIOD_US = per * 1000;
}

