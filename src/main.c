// door sensor code converted to c
#include <stdio.h>
#include <stdbool.h>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define SENSOR_PIN 4
#define COOLDOWN_MS 3000

int detectionCounter = 0;
bool previousState = 1;
TickType_t lastDetectionTime = 0;

void setup_pin()
{
    gpio_reset_pin(SENSOR_PIN);
    gpio_set_direction(SENSOR_PIN,GPIO_MODE_INPUT);
    gpio_pullup_en(SENSOR_PIN);
    printf("Sensor Pin is Setup!\n");
}

void sensor_task(void *pvParameters)
{
    while(1)
    {
        bool currentState = gpio_get_level(SENSOR_PIN);
        TickType_t currentTime = xTaskGetTickCount() * portTICK_PERIOD_MS;

        if (currentState == 0 && previousState == 1)
        {
            if ((currentTime - lastDetectionTime) > COOLDOWN_MS) 
            {
                detectionCounter++;
                printf("DETECTION - TOTAL DETECTIONS: %d\n", detectionCounter);
                lastDetectionTime = currentTime;
            }
        }
        previousState = currentState;
        printf("Current state: %d\n", currentState);
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}

void app_main()
{
    printf("Starting...\n");
    setup_pin();
    xTaskCreate(sensor_task,"Door Sensor",2048,NULL,5,NULL);
}

/*
TODO
1 - refine logic for detecting when door is opened or closed
2 - reduce power consumption when idle
3 - fetch date and time of each detection from server
4 - send detection logs to backend
*/