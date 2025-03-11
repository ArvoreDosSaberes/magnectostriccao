/* FreeRTOS includes. */
#include <stdio.h>

#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>
#include <timers.h>
#include <semphr.h>

//#include "pico/cyw43_arch.h"

#include "tasks_paramiters.h"
#include "task_http_server.h"

void task_http_server(void *pvParameters) {

   printf("Task HTTP Server\n");
   TickType_t xLastWakeTime = xTaskGetTickCount();

   while (1)
   {
//      cyw43_arch_poll(); // Necessário para manter o Wi-Fi ativo
      printf("Loop Task HTTP Server %d\n", xLastWakeTime);
      xTaskDelayUntil(&xLastWakeTime, TASK_HTTP_SERVER_DELAY);
      UBaseType_t stackLeft = uxTaskGetStackHighWaterMark(NULL);
      printf("HTTP Server Espaço de pilha livre: %u bytes\n", stackLeft);
  
   }
      
}
