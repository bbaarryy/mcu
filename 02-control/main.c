#define DEVICE_NAME "my-pico-device"
#define DEVICE_VRSN "v0.0.1"

#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "stdio-task/stdio-task.h"
#include "protocol-task/protocol-task.h"

const uint LED_PIN = 25;

void version_callback(const char* args)
{
	printf("device name: '%s', firmware version: %s\n", DEVICE_NAME, DEVICE_VRSN);
}

int main(){
    stdio_init_all();
    
    api_t device_api[] =
    {
	    {"version", version_callback, "get device name and firmware version"},
	    {NULL, NULL, NULL},
    };
    
    protocol_task_init(device_api);
    stdio_task_init();

    while(1){
        char* com = stdio_task_handle();
        if(com != NULL){
            protocol_task_handle(com);
        }
    }
}
