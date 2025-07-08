/**
 * @brief Tarefa para manter o display OLED atualziado
 * 
 * 
 * @details 
 *  Renderiza no display OLED as linhas de texto definidas pela OLED facade, usando oled_set_text_line() e oled_render_text().
 * 
 * @see task_adc_with_dma
 */

#ifndef  __TASK_DISPLAY_OLED_H__
#define  __TASK_DISPLAY_OLED_H__


void task_display_oled(void *pvParameters);

#endif // TASK_TINY_ML_H