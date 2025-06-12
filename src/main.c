#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"

// --- Definições de Pinos ---
#define LED_R_PIN 13 // Pino para o componente Vermelho do LED RGB
#define LED_G_PIN 11 // Pino para o componente Verde do LED RGB
#define LED_B_PIN 12 // Pino para o componente Azul do LED RGB

#define BUZZER_A_PIN 21 // Pino de controle A do Buzzer

#define BUTTON_A_PIN 5 // Botão para controlar a tarefa do LED
#define BUTTON_B_PIN 6 // Botão para controlar a tarefa do Buzzer

// Handles para as tarefas
TaskHandle_t rgb_led_task_handle = NULL;
TaskHandle_t buzzer_task_handle = NULL;

// Estados das tarefas (para controle de suspensão/retomada)
volatile bool led_task_suspended = false;
volatile bool buzzer_task_suspended = false;

void rgb_led_task(void *pvParameters) {
    gpio_init(LED_R_PIN);
    gpio_set_dir(LED_R_PIN, GPIO_OUT);
    gpio_init(LED_G_PIN);
    gpio_set_dir(LED_G_PIN, GPIO_OUT);
    gpio_init(LED_B_PIN);
    gpio_set_dir(LED_B_PIN, GPIO_OUT);

    while (true) {
        // Vermelho
        gpio_put(LED_R_PIN, 1);
        gpio_put(LED_G_PIN, 0);
        gpio_put(LED_B_PIN, 0);
        vTaskDelay(pdMS_TO_TICKS(500));

        // Verde
        gpio_put(LED_R_PIN, 0);
        gpio_put(LED_G_PIN, 1);
        gpio_put(LED_B_PIN, 0);
        vTaskDelay(pdMS_TO_TICKS(500));

        // Azul
        gpio_put(LED_R_PIN, 0);
        gpio_put(LED_G_PIN, 0);
        gpio_put(LED_B_PIN, 1);
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

void buzzer_task(void *pvParameters) {
    gpio_init(BUZZER_A_PIN);
    gpio_set_dir(BUZZER_A_PIN, GPIO_OUT);

    while (true) {
        gpio_put(BUZZER_A_PIN, 1);
        vTaskDelay(pdMS_TO_TICKS(50)); // Beep curto
        gpio_put(BUZZER_A_PIN, 0);
        vTaskDelay(pdMS_TO_TICKS(950)); // Espera 1 segundo total
    }
}

void button_task(void *pvParameters) {
    gpio_init(BUTTON_A_PIN);
    gpio_set_dir(BUTTON_A_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_A_PIN); // Habilita pull-up interno

    gpio_init(BUTTON_B_PIN);
    gpio_set_dir(BUTTON_B_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_B_PIN); // Habilita pull-up interno

    bool last_button_a_state = true; // Assume que o botão começa solto (nível alto com pull-up)
    bool last_button_b_state = true;

    while (true) {
        bool current_button_a_state = gpio_get(BUTTON_A_PIN);
        bool current_button_b_state = gpio_get(BUTTON_B_PIN);

        // Botão A (controla LED) - detecta borda de descida (pressionado)
        if (last_button_a_state && !current_button_a_state) {
            if (led_task_suspended) {
                vTaskResume(rgb_led_task_handle);
                led_task_suspended = false;
                printf("LED Task Resumed\n");
            } else {
                vTaskSuspend(rgb_led_task_handle);
                led_task_suspended = true;
                // Apaga o LED ao suspender
                gpio_put(LED_R_PIN, 0);
                gpio_put(LED_G_PIN, 0);
                gpio_put(LED_B_PIN, 0);
                printf("LED Task Suspended\n");
            }
        }
        last_button_a_state = current_button_a_state;

        // Botão B (controla Buzzer) - detecta borda de descida (pressionado)
        if (last_button_b_state && !current_button_b_state) {
            if (buzzer_task_suspended) {
                vTaskResume(buzzer_task_handle);
                buzzer_task_suspended = false;
                printf("Buzzer Task Resumed\n");
            } else {
                vTaskSuspend(buzzer_task_handle);
                buzzer_task_suspended = true;
                // Desliga o buzzer ao suspender
                gpio_put(BUZZER_A_PIN, 0);
                printf("Buzzer Task Suspended\n");
            }
        }
        last_button_b_state = current_button_b_state;

        vTaskDelay(pdMS_TO_TICKS(100)); // Polling a cada 100ms
    }
}

int main() {
    stdio_init_all();
    printf("Iniciando sistema multitarefa...\n");

    xTaskCreate(rgb_led_task, "RGB_LED_Task", 256, NULL, 1, &rgb_led_task_handle);

    xTaskCreate(buzzer_task, "Buzzer_Task", 256, NULL, 1, &buzzer_task_handle);

    xTaskCreate(button_task, "Button_Task", 256, NULL, 2, NULL); 

    vTaskStartScheduler();

    while (1) {
    };
    return 0;
}