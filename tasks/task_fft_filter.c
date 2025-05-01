/**
 * @file tasks.c
 * @author Carlos Delfino
 * @brief Tarefas para uso no Captador Acustico inteligente
 */

/* FreeRTOS includes. */
#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>
#include <timers.h>
#include <semphr.h>

#include "stdio.h"
#include "string.h"
#include "pico/stdlib.h"

#include "tasks_paramiters.h"
#include "task_fft_filter.h"

extern QueueHandle_t xFFT_Buffer_Queue;
extern QueueHandle_t xTinyML_Buffer_Queue;

static uint16_t adc_buffer[ADC_SAMPLES];

/**
 * @brief Tarefa para uso do Fast Fourier Transform
 * 
 * @details 
 *  Filtra ruidos que não são interessantes.
 * 
 * @see task_adc_with_dma
 */
void task_fft_filter(void *pvParameters){
    /* Unused parameters. */
    ( void ) pvParameters;

    printf("Task FFT Filter\n");
        
    TickType_t xPreviousWakeTime = xTaskGetTickCount();
    while (1)    
    {
        printf("Inicio Loop Task FFT Filter %d\n", xPreviousWakeTime);
        
        if(xQueueReceive(xFFT_Buffer_Queue, adc_buffer, FFT_TO_TINYML_QUEUE_TIMEOUT) == pdPASS){
            printf("FFT: sucesso ao receber dado de xFFT_Buffer_Queue\n");
            // processa adc_buffer e deposita num queue xTinyML_Buffer_Queue
            // implementa código que filtra o audio captado pelo microfone
            // esta task está atrelada a task_adc_with_dma, que deverá emitir 
            // um evento ou depositar num queue a leitura feita
            if(xQueueSendToBack(xTinyML_Buffer_Queue, adc_buffer, FFT_TO_TINYML_QUEUE_TIMEOUT) != pdPASS){
                printf("Erro ao enviar dados para xTinyML_Buffer_Queue\n");
            }
        };
        printf("antes da pausa Loop Task FFT Filter %d\n", xPreviousWakeTime);
        xTaskDelayUntil(&xPreviousWakeTime, TASK_TINYML_DELAY); /* delay 100 ticks */
        printf("Fim Loop Task FFT Filter %d\n", xPreviousWakeTime);
    }
}