#include "FreeRTOS.h"
#include "task.h"

#include "stdlib.h"
#include "stdio.h"
#include "string.h"

#include "oled.h"
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

void vApplicationMallocFailedHook( void ){
      oled_set_text_line(0, "***************", OLED_ALIGN_CENTER);
      oled_clear_text_line(1);
      oled_set_text_line(2, "Erro na", OLED_ALIGN_CENTER);
      oled_set_text_line(3, "alocacao", OLED_ALIGN_CENTER);
      oled_set_text_line(4, "de memoria", OLED_ALIGN_CENTER);
      oled_clear_text_line(5);
      oled_clear_text_line(6);
      oled_set_text_line(7, "***************", OLED_ALIGN_CENTER);
      oled_render_text();
}

void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName) {
      oled_set_text_line(0, "***************", OLED_ALIGN_CENTER);
      oled_clear_text_line(1);
      oled_set_text_line(2, "stack overflow", OLED_ALIGN_CENTER);
      oled_clear_text_line(3);
      oled_set_text_line(4, pcTaskName, OLED_ALIGN_CENTER);
      oled_clear_text_line(5);
      oled_clear_text_line(6);
      oled_set_text_line(7, "***************", OLED_ALIGN_CENTER);
      oled_render_text();
    while (1);
}