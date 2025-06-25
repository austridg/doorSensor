// door sensor
#include <stdio.h>
#include <stdbool.h>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"

#define SENSOR_PIN 4
#define COOLDOWN_MS 300

bool doorState;
int openCounter = 0;
int closeCounter = 0;
TickType_t lastDetectionTime = 0;

void setup_pin()
{
    gpio_reset_pin(SENSOR_PIN);
    gpio_set_direction(SENSOR_PIN,GPIO_MODE_INPUT);
    gpio_pullup_en(SENSOR_PIN);
    printf("Sensor Pin is Setup!\n");
}

const char* get_state(bool state)
{
    return state ? "Open" : "Closed";
}

void sensor_task(void *pvParameters)
{
    doorState = gpio_get_level(SENSOR_PIN);

    while(1)
    {
        bool currentState = gpio_get_level(SENSOR_PIN);
        TickType_t currentTime = xTaskGetTickCount() * portTICK_PERIOD_MS;

        if (currentState == 1 && doorState == 0)
        {
            if ((currentTime - lastDetectionTime) > COOLDOWN_MS) 
            {
                openCounter++;
                printf("DETECTION: Door %s\n", get_state(currentState));
                lastDetectionTime = currentTime;
            }
        }
        if (currentState == 0 && doorState == 1)
        {
            if ((currentTime - lastDetectionTime) > COOLDOWN_MS) 
            {
                closeCounter++;
                printf("DETECTION: Door %s\n",get_state(currentState));
                lastDetectionTime = currentTime;
            }
        }
        doorState = currentState;
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}

void debug_menu_task(void *pvParameters)
{
    const uart_port_t uart_num = UART_NUM_0; // default usb port
    uint8_t data[16];

    while(1)
    {
        int input = uart_read_bytes(uart_num, data, 16, 100 / portTICK_PERIOD_MS);
        if(input > 0)
        {
            switch (data[0])
            {
                case 'i':
                    printf("***** SENSOR INFO *****\n");
                    printf("Door State: %s\n", get_state(doorState));
                    printf("Door opened %d times\n", openCounter);
                    printf("Door closed %d times\n", closeCounter);
                    break;
                case 's':
                    printf("Current State: %s\n",get_state(doorState));
                    break;
                case 'o':
                    printf("Total Open Detections: %d\n",openCounter);
                    break;
                case 'c':
                    printf("Total Close Detections: %d\n",closeCounter);
                    break;
                case 'r':
                    openCounter = 0;
                    closeCounter = 0;
                    printf("Detections Reset");
                    break;
                case 'w':
                    printf("Entering Watch Mode: Press 'Q' to Exit.");

                    while(true)
                    {
                        doorState = gpio_get_level(SENSOR_PIN);
                        printf("State: %s | Opens: %d | Closes: %d\n", get_state(doorState), openCounter, closeCounter);
                        int exit = uart_read_bytes(uart_num, data, 16, 250 / portTICK_PERIOD_MS);
                        if (exit > 0 && data[0] == 'q')
                        {
                            printf("Exiting Watch Mode\n");
                            break;
                        }
                    }
                    break;
                case 'h':
                    printf("***** Command List *****\n");
                    printf("i - Full Snsor info\n");
                    printf("s - Sensor State\n");
                    printf("o - Total Open Detections\n");
                    printf("c - Total Close Detections\n");
                    printf("r - Reset Open and Close Detections\n");
                    printf("w - Watch Mode\n");
                    printf("q - Quit Watch Mode (while in watch mode)\n");
                    printf("h - Command List\n");
                    break;
                default:
                    printf("Unknown Command - Type 'h' For Command List\n");
                    break;
            }
        }
    }
    
}

void app_main()
{
    printf("Starting...\n");
    uart_driver_install(UART_NUM_0, 1024, 0, 0, NULL, 0);
    setup_pin();
    xTaskCreate(sensor_task,"Door Sensor",2048,NULL,5,NULL);
    xTaskCreate(debug_menu_task, "Debug Menu", 2048, NULL, 5, NULL);
}

/*
TODO
1 - reduce power consumption when idle & logic rework for this
2 - fetch date and time of each detection from server
3 - send detection logs to backend
*/