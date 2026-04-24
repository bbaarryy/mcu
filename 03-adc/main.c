#define DEVICE_NAME "my-pico-device"
#define DEVICE_VRSN "v0.0.1"

#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "stdio-task/stdio-task.h"
#include "protocol-task/protocol-task.h"
#include "led-task/led-task.h"
#include "stdint.h" 

//onst uint LED_PIN = 25;

void version_callback(const char* args)
{
	printf("device name: '%s', firmware version: %s\n", DEVICE_NAME, DEVICE_VRSN);
}

void led_on_callback(const char* args){
    led_task_state_set(LED_STATE_ON);
}

void led_off_callback(const char* args){
    led_task_state_set(LED_STATE_OFF);
}

void led_blink_callback(const char* args){
    led_task_state_set(LED_STATE_BLINK);
}

void led_blink_set_period_ms_callback(const char* args){
    int period_ms = 0;
    sscanf(args, "%u", &period_ms);
    if(period_ms == 0){
        return;
    }
    else{
        led_task_set_blink_period_ms(period_ms);
        led_task_state_set(LED_STATE_BLINK);
    }
}

void help_callback(const char* args){
    char* helps[] = {"version","on","off","blink","set_period","help"};
    char* infos[] = {"get version","tuen led on","turn led off","start blink","set period for blinking","help"};
    for(int i = 0 ; i < 6;i++){
        printf("command: '%s', help_info: '%s' \n", helps[i], infos[i]);
    }
}

void mem_callback(const char* args){
    uint32_t addr;
    sscanf(args, "%u", &addr);
    printf("%s",addr);


}

int main(){
    stdio_init_all();
    led_task_init();

    api_t device_api[] =
    {
        {"version", version_callback, "get device name and firmware version"},
        {"on", led_on_callback, "turn the lights on"},
        {"off", led_off_callback, "turn the lights off"},
        {"blink", led_blink_callback, "start blinking"},
        {"set_period", led_blink_set_period_ms_callback, "set period of blinking"},
        {"help", help_callback, "get help"},
        {"mem", mem_callback, "get mem"},
        {NULL, NULL, NULL},
    };

    protocol_task_init(device_api);
    stdio_task_init();

    while(1){
        char* com = stdio_task_handle();
        if(com != NULL){
            protocol_task_handle(com);
        }

        led_task_handle();
    }
}
