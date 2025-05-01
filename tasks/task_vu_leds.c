#include <stdio.h>

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"


#include "inc/neopixel.h"

#include "inc/magnectostriccao.h"
#include "tasks_paramiters.h"
#include "task_vu_leds.h"


extern QueueHandle_t xIntensity_Buffer_Queue;

void task_vu_leds(void *pvParameters)
{

  (void)pvParameters;
  printf("Task Vu LEDs\n");
  // O código abaixo deve ser transportado para a TASK (FreeRTOS) que
  // faz o processamento da amostragem via DMA

  TickType_t xLastWakeTime = xTaskGetTickCount();
  while (1)
  {

    printf("Início Loop Task VU LEDs %d\n", xLastWakeTime);

    // Limpa a matriz de LEDs.
    npClear();

    uint intensity;
    if(xQueueReceive(xIntensity_Buffer_Queue, &intensity, OLED_INIT_QUEUE_TIMEOUT) != pdPASS) {
      printf("Nao recebeu intensidade\n");
      continue;
    }
    // A depender da intensidade do som, acende LEDs específicos.
    switch (intensity)
    {
    case 0:
      break; // Se o som for muito baixo, não acende nada.
    case 1:
      npSetLED(12, 0, 0, 80); // Acende apenas o centro.
      break;
    case 2:
      npSetLED(12, 0, 0, 120); // Acente o centro.

      // Primeiro anel.
      npSetLED(7, 0, 0, 80);
      npSetLED(11, 0, 0, 80);
      npSetLED(13, 0, 0, 80);
      npSetLED(17, 0, 0, 80);
      break;
    case 3:
      // Centro.
      npSetLED(12, 60, 60, 0);

      // Primeiro anel.
      npSetLED(7, 0, 0, 120);
      npSetLED(11, 0, 0, 120);
      npSetLED(13, 0, 0, 120);
      npSetLED(17, 0, 0, 120);

      // Segundo anel.
      npSetLED(2, 0, 0, 80);
      npSetLED(6, 0, 0, 80);
      npSetLED(8, 0, 0, 80);
      npSetLED(10, 0, 0, 80);
      npSetLED(14, 0, 0, 80);
      npSetLED(16, 0, 0, 80);
      npSetLED(18, 0, 0, 80);
      npSetLED(22, 0, 0, 80);
      break;
    case 4:
      // Centro.
      npSetLED(12, 80, 0, 0);

      // Primeiro anel.
      npSetLED(7, 60, 60, 0);
      npSetLED(11, 60, 60, 0);
      npSetLED(13, 60, 60, 0);
      npSetLED(17, 60, 60, 0);

      // Segundo anel.
      npSetLED(2, 0, 0, 120);
      npSetLED(6, 0, 0, 120);
      npSetLED(8, 0, 0, 120);
      npSetLED(10, 0, 0, 120);
      npSetLED(14, 0, 0, 120);
      npSetLED(16, 0, 0, 120);
      npSetLED(18, 0, 0, 120);
      npSetLED(22, 0, 0, 120);

      // Terceiro anel.
      npSetLED(1, 0, 0, 80);
      npSetLED(3, 0, 0, 80);
      npSetLED(5, 0, 0, 80);
      npSetLED(9, 0, 0, 80);
      npSetLED(15, 0, 0, 80);
      npSetLED(19, 0, 0, 80);
      npSetLED(21, 0, 0, 80);
      npSetLED(23, 0, 0, 80);
      break;
    }
    // Atualiza a matriz.
    npWrite();

    printf("Antes da pausa Loop Task VU LEDs %d\n", xLastWakeTime);
    xTaskDelayUntil(&xLastWakeTime, TASK_ADC_DMA_DELAY );
    printf("FIM LOOP Task VU LEDs %d\n", xLastWakeTime);

    UBaseType_t stackLeft = uxTaskGetStackHighWaterMark(NULL);
    printf("VU LEDS - Espaço de pilha livre: %u bytes\n", stackLeft);
  }
}
