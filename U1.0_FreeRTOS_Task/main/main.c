#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

TaskHandle_t TASK1_Handler;
TaskHandle_t TASK2_Handler;

void Task1(void *arg){
    int i = 0;
    while(1){
        printf("Task1: %d\n", i++);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        if(i == 5) vTaskSuspend(TASK2_Handler);
        if(i == 10) vTaskResume(TASK2_Handler);
        if(i == 15) vTaskDelete(TASK2_Handler);
    }
}

void Task2(void *arg){
    int i = 0;
    while(1){
        printf("Task2: %d\n", i++);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void app_main(void)
{
    xTaskCreate(Task1, "Task1", 4096, NULL, 0, &TASK1_Handler);
    xTaskCreate(Task2, "Task2", 4096, NULL, 10, &TASK2_Handler);
}