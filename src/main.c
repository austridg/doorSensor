// door sensor
#include <stdio.h>
#include <stdbool.h>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define SENSOR_PIN 4
#define LED_PIN 5
#define COOLDOWN_MS 3000

int detectionCounter = 0;
int ledState = 0;
bool previousState = 1;
TickType_t lastDetectionTime = 0;

void setup_pin()
{
    gpio_reset_pin(SENSOR_PIN);
    gpio_set_direction(SENSOR_PIN,GPIO_MODE_INPUT);
    gpio_pullup_en(SENSOR_PIN);
    printf("Sensor Pin is Setup!\n");

    gpio_reset_pin(LED_PIN);
    gpio_set_direction(LED_PIN,GPIO_MODE_OUTPUT);
    printf("Led Pin is Setup!\n");
}

void sensor_task(void *pvParameters)
{
    while(1)
    {
        bool currentState = gpio_get_level(SENSOR_PIN);
        TickType_t currentTime = xTaskGetTickCount() * portTICK_PERIOD_MS;
        gpio_set_level(LED_PIN, currentState ? 0 : 1);

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