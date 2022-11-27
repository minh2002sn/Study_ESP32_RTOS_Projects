#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
// #include "freertos/portmacro.h"
#include "driver/gpio.h"

#define ESP_INTR_FLAG_DEFAULT       0
#define CONFIG_BUTTON_PIN           16
#define CONFIG_LED_1_PIN            2
#define CONFIG_LED_2_PIN            19

#define portEND_SWITCHING_ISR(xSwitchRequired) if(xSwitchRequired) vPortYield()

TaskHandle_t LED_Task_Handler;
void LED_Change_State_Task(void *arg){
    bool LED_State = false;
    while(1){
        LED_State = !LED_State;
        gpio_set_level(CONFIG_LED_1_PIN, LED_State);
        printf("LED changed\n");
        vTaskSuspend(NULL);
    }
}

static void IRAM_ATTR BUTTON_ISR_Handle(void *arg){
    // printf("Button press\n");
    BaseType_t xHighPriority;
    xHighPriority = xTaskResumeFromISR(LED_Task_Handler);
    portEND_SWITCHING_ISR(xHighPriority);
    // static bool LED_State = false;
    // LED_State = !LED_State;
    // gpio_set_level(CONFIG_LED_1_PIN, LED_State);
}

// TaskHandle_t LED_Blink_Task_Handler = NULL;
// void LED_Blink_Task(void *arg){
//     bool LED_State = true;
//     while(1){
//         LED_State = !LED_State;
//         gpio_set_level(CONFIG_LED_2_PIN, LED_State);
//         vTaskDelay(500 / portTICK_PERIOD_MS);
//     }
// }

void app_main(void)
{
    gpio_pad_select_gpio(CONFIG_BUTTON_PIN);
    gpio_set_direction(CONFIG_BUTTON_PIN, GPIO_MODE_INPUT);
    gpio_set_pull_mode(CONFIG_BUTTON_PIN, GPIO_PULLUP_ONLY);
    gpio_set_intr_type(CONFIG_BUTTON_PIN, GPIO_INTR_NEGEDGE);
    gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);
    gpio_isr_handler_add(CONFIG_BUTTON_PIN, (gpio_isr_t)BUTTON_ISR_Handle, (void *)NULL);

    gpio_pad_select_gpio(CONFIG_LED_1_PIN);
    gpio_set_direction(CONFIG_LED_1_PIN, GPIO_MODE_OUTPUT);
    gpio_set_level(CONFIG_LED_1_PIN, false);

    // gpio_reset_pin(CONFIG_LED_2_PIN);
    // gpio_set_direction(CONFIG_LED_2_PIN, GPIO_MODE_OUTPUT);
    // gpio_set_level(CONFIG_LED_2_PIN, false);

    xTaskCreate(LED_Change_State_Task, "LED_Change_State_Task", 4096, NULL, 10, LED_Task_Handler);
    // xTaskCreate(LED_Blink_Task, "LED_Blink_Task", 4096, NULL, 10, LED_Blink_Task_Handler);
}