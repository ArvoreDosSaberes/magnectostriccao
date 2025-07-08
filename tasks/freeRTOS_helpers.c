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
      oled_set_text_line(0, "***************");
      oled_set_text_line(1, "               ");
      oled_set_text_line(2, "    Erro na    ");
      oled_set_text_line(3, "   alocacao    ");
      oled_set_text_line(4, "  de memoria   ");
      oled_set_text_line(5, "               ");
      oled_set_text_line(6, "               ");
      oled_set_text_line(7, "***************");
      oled_render_text();
}

void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName) {
      oled_set_text_line(0, "***************");
      oled_set_text_line(1, "               ");
      oled_set_text_line(2, " stack overflow");
      oled_set_text_line(3, "               ");
      oled_set_text_line(4, pcTaskName);
      oled_set_text_line(5, "               ");
      oled_set_text_line(6, "               ");
      oled_set_text_line(7, "***************");
      oled_render_text();
    while (1);
}