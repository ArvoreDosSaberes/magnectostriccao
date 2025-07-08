/**
 * @file tasks.c
 * @author Carlos Delfino
 * @brief Tarefas para uso no Captador Acustico inteligente
 */

/* FreeRTOS includes. */
#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>

#include "oled.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "task_display_oled.h"
#include "tasks_paramiters.h"

extern QueueHandle_t xIntensity_Buffer_Queue;

/**
 * @brief Tarefa para uso do Display OLED
 *
 * @details
 *
 */
void task_display_oled(void *pvParameters)
{
    printf("Task Display OLED\n");
    /* Unused parameters. */
    (void)pvParameters;

    vTaskSuspendAll();
    oled_set_text_line(0, "               ");
    oled_set_text_line(1, " Nivel do Sinal");
    oled_set_text_line(2, "               ");
    oled_set_text_line(3, "               ");
    oled_set_text_line(4, "               ");
    oled_set_text_line(5, "               ");
    oled_set_text_line(6, "               ");
    oled_set_text_line(7, "               ");
    xTaskResumeAll();
        
    TickType_t xLastWakeTime = xTaskGetTickCount();
    while (1)
    {
        printf("inicio loop Task Display OLED %d\n", xLastWakeTime);
        
        vTaskSuspendAll();
        oled_render_text();
        xTaskResumeAll();
        
        xTaskDelayUntil(&xLastWakeTime, TASK_DISPLAY_OLED_DELAY); 
        printf("fim loop task display oled %d\n", xLastWakeTime);

        UBaseType_t stackLeft = uxTaskGetStackHighWaterMark(NULL);
        printf("Display OLED Espaço de pilha livre: %u bytes\n", stackLeft);
    }
}