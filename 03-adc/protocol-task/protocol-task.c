#include "protocol-task.h"
#include "stdio.h"
#include "pico/stdlib.h"
#include "string.h"

static api_t* api = {0};
static int commands_count = 0;

void protocol_task_init(api_t* device_api){
    api = device_api;
    commands_count = 1;
};

void protocol_task_handle(char* command_string){
    // логика обработки полученной строки. Делим ее на команду и аргументы:
    char* command_name = command_string;
    char* command_args = NULL;

    char* space_symbol = strchr(command_string, ' ');

    if (space_symbol)
    {
        *space_symbol = '\0';
        command_args = space_symbol + 1;
    }
    else
    {   
        command_args = "";
    }


    printf("command: '%s'\n", command_name);
    printf("args: '%s'\n", command_args);


    int index = 0;
    bool ch = 0;
    while(api[index].command_name != NULL)
    {
        if(!strcmp(command_name, api[index].command_name)){
            api[index].command_callback(command_args);
            ch = 1;
            return;
        }

        index++;
    }

    if(!ch){
        printf("Command not found\n");
    }
    return;
}
