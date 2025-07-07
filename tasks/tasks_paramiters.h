#ifndef __TASKS_PARAMITERS_H__
#define __TASKS_PARAMITERS_H__

#include "FreeRTOS.h"
#include "task.h"

#define TASK_DISPLAY_OLED_STACK_SIZE   (configMINIMAL_STACK_SIZE)
#define TASK_DISPLAY_OLED_PRIORITY     (tskIDLE_PRIORITY + 1)
#define TASK_DISPLAY_OLED_DELAY        (pdMS_TO_TICKS(200))

#define TASK_VU_LEDS_STACK_SIZE        (configMINIMAL_STACK_SIZE)
#define TASK_VU_LEDS_PRIORITY          (tskIDLE_PRIORITY + 2)
#define TASK_VU_LEDS_DELAY             (pdMS_TO_TICKS(200))

#define TASK_DRONE_CONTROL_STACK_SIZE  (configMINIMAL_STACK_SIZE)
#define TASK_DRONE_CONTROL_PRIORITY    (tskIDLE_PRIORITY + 3)
#define TASK_DRONE_CONTROL_DELAY       (pdMS_TO_TICKS(150))

#define TASK_HTTP_SERVER_STACK_SIZE    (configMINIMAL_STACK_SIZE)
#define TASK_HTTP_SERVER_PRIORITY      (tskIDLE_PRIORITY + 2)
#define TASK_HTTP_SERVER_DELAY         (pdMS_TO_TICKS(300))

#define TASK_TINYML_STACK_SIZE         (configMINIMAL_STACK_SIZE)
#define TASK_TINYML_PRIORITY           (tskIDLE_PRIORITY + 4)
#define TASK_TINYML_DELAY              (pdMS_TO_TICKS(150))

#define TASK_FFT_FILTER_STACK_SIZE     (configMINIMAL_STACK_SIZE)
#define TASK_FFT_FILTER_PRIORITY       (tskIDLE_PRIORITY + 4)
#define TASK_FFT_FILTER_DELAY          (pdMS_TO_TICKS(150))

#define TASK_ADC_DMA_STACK_SIZE        (configMINIMAL_STACK_SIZE)
#define TASK_ADC_DMA_PRIORITY          (tskIDLE_PRIORITY + 4)
#define TASK_ADC_DMA_DELAY             (pdMS_TO_TICKS(150))

#define ADC_TO_FFT_QUEUE_TIMEOUT portMAX_DELAY // Tempo de espera para a fila do ADC.
#define ADC_INTENSITY_QUEUE_TIMEOUT portMAX_DELAY // Tempo de espera para a fila da intensidade.

#define ADC_TO_FFT_QUEUE_TIMEOUT portMAX_DELAY // Tempo de espera para a fila do ADC.
#define ADC_INTENSITY_QUEUE_TIMEOUT portMAX_DELAY // Tempo de espera para a fila da intensidade.

#define ADC_SAMPLES 256 // Número de amostras que serão feitas do ADC. deve ser potência de 2
#define ADC_QUEUE_LENGTH 30 // Número de amostras que serão armazenadas na fila do ADC.

#define OLED_INIT_QUEUE_TIMEOUT 10 // Tempo de espera para a fila da inicialização do OLED.
#define ADC_INTENSITY_QUEUE_TIMEOUT portMAX_DELAY // Tempo de espera para a fila da intensidade.
#define ADC_INTENSITY_QUEUE_LENGTH 1// 1 PARA PROCESAR O QUE FOR VIÁVEL // Número de amostras que são armazenadas na fila da intensidade.

#define FFT_TO_TINYML_QUEUE_TIMEOUT portMAX_DELAY // Tempo de espera para a fila da FFT.
#define TINYML_QUEUE_LENGTH 100 // Número de amostras que são armazenadas na fila da FFT.

#endif