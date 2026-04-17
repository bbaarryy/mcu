#define DEVICE_NAME "my-pico-device"
#define DEVICE_VRSN "v0.0.1"

#include "pico/stdlib.h"
#include "hardware/gpio.h"

const uint LED_PIN = 25;

int main(){
    stdio_init_all();
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    // while(1){
    //     char symbol = getchar();
    //     printf("received char: %c [ ASCII code: %d ]\n", symbol, symbol);   
    // }

    while(1){
        char symbol = getchar();
        if(symbol == 'e'){
            gpio_put(LED_PIN, 1);
        }
        if(symbol == 'd'){
            gpio_put(LED_PIN, 0);
        }
        if(symbol == 'v'){
            printf("device name: '%s', firmware version: %s\n", DEVICE_NAME, DEVICE_VRSN);
        }
    }
}
