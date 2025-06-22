#include <stdio.h>
#include <stdbool.h>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define SENSOR_PIN 2
#define COOLDOWN_MS 3000

int detectionCounter = 0;
bool previousState = 1;
TickType_t lastDetectionTime = 0;

void setup_pin()
{
    gpio_set_direction(SENSOR_PIN,GPIO_MODE_INPUT);
    printf("Sensor Pin is Setup!\n");
}

void sensor_task(void *pvParameters)
{
    while(1)
    {
        bool currentState = gpio_get_level(SENSOR_PIN);
        TickType_t currentTime = xTaskGetTickCount();

        if (currentState == 0 && previousState == 1)
        {
            if ((currentTime - lastDetectionTime) > COOLDOWN_MS) 
            {
                detectionCounter++;
                printf("DETECTION - TOTAL DETECTIONS: %d\n", detectionCounter);
                lastDetectionTime = currentTime;
            }
            previousState = currentState;
            vTaskDelay(100);
        }
    }
}

void app_main()
{
    printf("Starting...");
    setup_pin();
    xTaskCreate(sensor_task,"Door Sensor",2048,NULL,5,NULL);
}