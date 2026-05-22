#include "stdio.h"
#include "stdlib.h"
#include "pico/stdlib.h"
#include "stdio-task/stdio-task.h"
#include "protocol-task.h"
#include "led-task/led-task.h"
#include "mem-wmem-task/mem-wmem-task.h"
#include "bme280-driver.h"
#include "hardware/i2c.h"

// #include "pico/stdlib.h"
// #include "hardware/gpio.h"
// #include "stdio-task/stdio-task.h"
// #include "led-task/led-task.h"
// #include "adc-task/adc-task.h"
// #include "stdint.h" 

#define DEVICE_NAME "my-pico-device"
#define DEVICE_VRSN "v0.0.1"

void version_callback(const char* args)
{
	printf("device name: '%s', firmware version: %s\n", DEVICE_NAME, DEVICE_VRSN);
}

void led_on_callback(const char* args)
{
    led_task_state_set(LED_STATE_ON);
}

void led_off_callback(const char* args)
{
    led_task_state_set(LED_STATE_OFF);
}

void led_blink_callback(const char* args)
{
    led_task_state_set(LED_STATE_BLINK);
}

void led_blink_set_period_ms_callback(const char* args)
{
    uint period_ms = 0;
    sscanf(args, "%u", &period_ms);

    if(period_ms==0)
    {
        printf("Error: perios_ms is zero");
        return;
    }

    led_task_set_blink_period_ms(period_ms);
}

void help_callback(const char* args);

void mem_callback(const char* args)
{
    if (args == NULL || args[0] == '\0')
    {
        printf("error: usage: mem <hex_address>\n");
        return;
    }

    
    uint32_t address = (uint32_t)strtoul(args, NULL, 16);

    
    mem(address);
}

void wmem_callback(const char* args)
{
    if (args == NULL || args[0] == '\0')
    {
        printf("error: usage: wmem <hex_addr> <hex_value>\n");
        return;
    }

    char* end_ptr;
    
    uint32_t addr  = (uint32_t)strtoul(args,    &end_ptr, 16);

    uint32_t value = (uint32_t)strtoul(end_ptr, NULL,     16);

    wmem(addr, value);
    led_task_state_set(LED_STATE_BLINK);
}

void read_reg_callback(const char* args)
{
    if (args == NULL || args[0] == '\0')
    {
        printf("error: usage: read_reg <hex_addr> <hex_N>\n");
        return;
    }

    char* end_ptr;
    uint32_t addr = (uint32_t)strtoul(args,     &end_ptr, 16);
    uint32_t N    = (uint32_t)strtoul(end_ptr,  NULL,     16);

    if (addr > 0xFF)
    {
        printf("error: addr must be <= 0xFF\n");
        return;
    }
    if (N > 0xFF)
    {
        printf("error: N must be <= 0xFF\n");
        return;
    }
    if (addr + N > 0x100)
    {
        printf("error: addr + N must be <= 0x100\n");
        return;
    }

    uint8_t buffer[256] = {0};
    bme280_read_regs((uint8_t)addr, buffer, (uint8_t)N);

    for (int i = 0; i < (int)N; i++)
    {
        printf("bme280 register [0x%X] = 0x%X\n", (unsigned)(addr + i), buffer[i]);
    }
}

void rp2040_i2c_read(uint8_t* buffer, uint16_t length)
{
	i2c_read_timeout_us(i2c1, 0x76, buffer, length, false, 100000);
}

void rp2040_i2c_write(uint8_t* data, uint16_t size)
{
	i2c_write_timeout_us(i2c1, 0x76, data, size, false, 100000);
}

void write_reg_callback(const char* args)
{
    if (args == NULL || args[0] == '\0')
    {
        printf("error: usage: write_reg <hex_addr> <hex_value>\n");
        return;
    }

    char* end_ptr;
    uint32_t addr  = (uint32_t)strtoul(args,     &end_ptr, 16);
    uint32_t value = (uint32_t)strtoul(end_ptr,  NULL,     16);

    if (addr > 0xFF)
    {
        printf("error: addr must be <= 0xFF\n");
        return;
    }
    if (value > 0xFF)
    {
        printf("error: value must be <= 0xFF\n");
        return;
    }

    bme280_write_reg((uint8_t)addr, (uint8_t)value);
    printf("bme280 register [0x%X] written: 0x%X\n", (unsigned)addr, (unsigned)value);
}

void temp_raw_callback(const char* args)
{
    printf("temp_raw = %u\n", bme280_read_temp_raw());
}

void pres_raw_callback(const char* args)
{
    printf("pres_raw = %u\n", bme280_read_pres_raw());
}

void hum_raw_callback(const char* args)
{
    printf("hum_raw = %u\n", bme280_read_hum_raw());
}

void temp_callback(const char* args)
{
    int32_t t = bme280_read_temp();
    printf("temperature: %d.%02d C\n", t / 100, t % 100);
}

void pres_callback(const char* args)
{
    uint32_t p = bme280_read_pres();
    printf("pressure: %u.%02u hPa\n", p / 100, p % 100);
}

void hum_callback(const char* args)
{
    uint32_t h = bme280_read_hum();
    printf("humidity: %u.%01u %%\n", h / 1024, (h % 1024) * 100 / 1024);
}

api_t device_api[] =
{
	{"version", version_callback, "get device name and firmware version"},
    {"on", led_on_callback, "turn LED on"},
    {"off", led_off_callback, "turn LED off"},
    {"blink", led_blink_callback, "start LED blinking"},
    {"set_period", led_blink_set_period_ms_callback, "setting period of LED blinking"},
    {"help", help_callback, "get help about all avaliable commands"},
    {"mem", mem_callback, "get value from adress"},
    {"wmem",    wmem_callback,     "write memory: wmem <hex_addr> <hex_value>"},
    {"read_reg",   read_reg_callback,"read BME280 registers: read_reg <hex_addr> <hex_N>"},
    {"write_reg", write_reg_callback, "write BME280 register: write_reg <hex_addr> <hex_value>"},
    {"temp_raw", temp_raw_callback, "read raw temperature from BME280"},
    {"pres_raw", pres_raw_callback, "read raw pressure from BME280"},
    {"hum_raw",  hum_raw_callback,  "read raw humidity from BME280"},
    {"temp", temp_callback, "read temperature in цельсии"},
    {"pres", pres_callback, "read pressure in hPa"},
    {"hum",  hum_callback,  "read humidity in %"},
	{NULL, NULL, NULL},
};

void help_callback(const char* args)
{
    printf("Available commands:\n");
    for (int i = 0; device_api[i].command_name != NULL; i++)
    {
        printf("  %-10s — %s\n", device_api[i].command_name, device_api[i].command_help);
    }
}

int main()
{
    stdio_init_all();
    //led_task_init();
    //adc_task_init();

    //protocol_task_init(device_api);
    //stdio_task_init();

    //stdio_init_all();
    stdio_task_init();
    protocol_task_init(device_api);
    led_task_init();
    i2c_init(i2c1, 100000);
    gpio_set_function(14, GPIO_FUNC_I2C);
    gpio_set_function(15, GPIO_FUNC_I2C);
    bme280_init(rp2040_i2c_read, rp2040_i2c_write);
    

    while(1){
        char* com = stdio_task_handle();
        if(com != NULL){
            protocol_task_handle(com);
        }

        led_task_handle();
        //adc_task_handle();
    }
}