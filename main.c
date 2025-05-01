/* FreeRTOS includes. */
#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>
#include <timers.h>
#include <semphr.h>

/* Standard includes. */

#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "pico/binary_info.h"
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "hardware/vreg.h"
#include "hardware/clocks.h"

#include "hardware/adc.h"
#include "hardware/dma.h"

#include "hardware/i2c.h"

#include "lwip/tcp.h"

#include "inc/ssd1306.h"

#include "inc/neopixel.h"

#include "inc/magnectostriccao.h" // Carrega parametros gerais do sistema
#include "tasks/tasks_paramiters.h" // parametros para criação das tasks

#include "tasks/task_adc_with_dma.h" // carrega tarefas do ADC
#include "tasks/task_display_oled.h" // Carrega tarefas do display
#include "tasks/task_drone_control.h" // Carrega tarefas do controle de drone
#include "tasks/task_tinyML.h" // Carrega tarefas do TinyML
#include "tasks/task_http_server.h" // Carrega tarefas do http server
#include "tasks/task_fft_filter.h" // Carrega tarefas do filtro FFT
#include "tasks/task_vu_leds.h" // Carrega tarefas do leds de volume
#include "main.h" // carrega cabeçalhos do main

// buffer de texto para o display oled
extern char text_line_oled[max_text_lines][max_text_columns];

// Buffer de amostras do ADC.
static uint32_t last_time;
static bool modo_local = false;

// Preparar área de renderização para o display (ssd1306_width pixels por ssd1306_n_pages páginas)
static struct render_area frame_area = {
  start_column : 0,
  end_column : ssd1306_width - 1,
  start_page : 0,
  end_page : ssd1306_n_pages - 1
};

// zera o display inteiro
static uint8_t ssd[ssd1306_buffer_length];

QueueHandle_t xFFT_Buffer_Queue = NULL;
QueueHandle_t xTinyML_Buffer_Queue = NULL;
QueueHandle_t xIntensity_Buffer_Queue = NULL;


int main()
{
  stdio_init_all(); // Inicializa os tipos stdio padrão presentes ligados ao binário

  init_display_oled();

  show_intro();

  sleep_ms(INTER_SCREEN_DELAY);

  if(!start_VU_LED()) return 1;

  sleep_ms(INTER_SCREEN_DELAY);

  start_bottons_control();

  sleep_ms(INTER_SCREEN_DELAY);

  if(!start_ADC_with_DMA()) return 1;

  sleep_ms(INTER_SCREEN_DELAY);

  if(!start_tinyML()) return 1;

  sleep_ms(INTER_SCREEN_DELAY);

  if(!start_fft_filter()) return 1;

  sleep_ms(INTER_SCREEN_DELAY);

  if(!start_network_infrastructure()) return 1;

  sleep_ms(INTER_SCREEN_DELAY);
 
  if(!start_gpio_and_drone_control()) return 1;

  sleep_ms(INTER_SCREEN_DELAY);

  if(!start_display_oled()) return 1;
  
  sleep_ms(INTER_SCREEN_DELAY);

  strcpy(text_line_oled[0], "               ");
  strcpy(text_line_oled[1], "Inicializando o");
  strcpy(text_line_oled[2], "               ");
  strcpy(text_line_oled[3], "  escalonador  ");
  strcpy(text_line_oled[4], "               ");
  strcpy(text_line_oled[5], "      de       ");
  strcpy(text_line_oled[6], "    rarefas    ");
  strcpy(text_line_oled[7], "               ");

  uint8_t y = 0;
  for (uint i = 0; i < count_of(text_line_oled); i++)
  {
    ssd1306_draw_string(ssd, 5, y, text_line_oled[i]);
    y += ssd1306_line_height;
  }
  render_on_display(ssd, &frame_area);
  
  vTaskStartScheduler();

  return 0;
}

bool start_display_oled(){
  BaseType_t xReturn = xTaskCreate(
    task_display_oled,
    "Task que mantem o display OLED atualizado",
    TASK_DISPLAY_OLED_STACK_SIZE,
    NULL,
    TASK_DISPLAY_OLED_PRIORITY,
    NULL
  );

  if (xReturn != pdPASS) {
    strcpy(text_line_oled[0], "     FALHA     ");
    strcpy(text_line_oled[1], " AO CRIAR TASK ");
    strcpy(text_line_oled[2], "      RTOS     ");
    strcpy(text_line_oled[3], "               ");
    strcpy(text_line_oled[4], "               ");
    strcpy(text_line_oled[5], " DISPLAY OLED  ");
    strcpy(text_line_oled[6], "               ");
    strcpy(text_line_oled[7], "               ");

    uint8_t y = 0;
    for (uint i = 0; i < count_of(text_line_oled); i++)
    {
      ssd1306_draw_string(ssd, 5, y, text_line_oled[i]);
      y += ssd1306_line_height;
    }
    render_on_display(ssd, &frame_area);

    sleep_ms(INTER_SCREEN_DELAY * 6);
    return false;
  }
  return true;
}

void init_display_oled(){
  // Inicialização do i2c
  i2c_init(i2c1, ssd1306_i2c_clock * 1000);
  gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
  gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
  gpio_pull_up(I2C_SDA);
  gpio_pull_up(I2C_SCL);

  // Processo de inicialização completo do OLED SSD1306
  ssd1306_init();

  calculate_render_area_buffer_length(&frame_area);

  memset(ssd, 0, ssd1306_buffer_length);
  render_on_display(ssd, &frame_area);

}
/**
 * Mostra a tela de abertura do projeto.
 */
void show_intro(){
  strcpy(text_line_oled[0], "    Projeto    ");
  strcpy(text_line_oled[1], "     Final     ");
  strcpy(text_line_oled[2], "               ");
  strcpy(text_line_oled[3], "  EmbarcaTech  ");
  strcpy(text_line_oled[4], "               ");
  strcpy(text_line_oled[5], "   Residencia  ");
  strcpy(text_line_oled[6], "     TIC 37    ");
  strcpy(text_line_oled[7], "               ");

  uint8_t y = 0;
  for (uint i = 0; i < count_of(text_line_oled); i++)
  {
    ssd1306_draw_string(ssd, 5, y, text_line_oled[i]);
    y += ssd1306_line_height;
  }
  render_on_display(ssd, &frame_area);

  sleep_ms(INTER_SCREEN_DELAY * 3);

  strcpy(text_line_oled[0], "  Analizador   ");
  strcpy(text_line_oled[1], "  da Saude de  ");
  strcpy(text_line_oled[2], "               ");
  strcpy(text_line_oled[3], "Transformadores");
  strcpy(text_line_oled[4], "      por      ");
  strcpy(text_line_oled[5], "               ");
  strcpy(text_line_oled[6], "Magnetostriccao");
  strcpy(text_line_oled[7], "               ");

  y = 0;
  for (uint i = 0; i < count_of(text_line_oled); i++)
  {
    ssd1306_draw_string(ssd, 5, y, text_line_oled[i]);
    y += ssd1306_line_height;
  }
  render_on_display(ssd, &frame_area);

  sleep_ms(INTER_SCREEN_DELAY * 1.25);

  strcpy(text_line_oled[0], "  da Saude de  ");
  strcpy(text_line_oled[1], "               ");
  strcpy(text_line_oled[2], "Transformadores");
  strcpy(text_line_oled[3], "      por      ");
  strcpy(text_line_oled[4], "               ");
  strcpy(text_line_oled[5], "Magnetostriccao");
  strcpy(text_line_oled[6], "               ");
  strcpy(text_line_oled[7], "               ");

  y = 0;
  for (uint i = 0; i < count_of(text_line_oled); i++)
  {
    ssd1306_draw_string(ssd, 5, y, text_line_oled[i]);
    y += ssd1306_line_height;
  }
  render_on_display(ssd, &frame_area);

  sleep_ms(OLED_SCROOL_DELAY);

  strcpy(text_line_oled[0], "               ");
  strcpy(text_line_oled[1], "Transformadores");
  strcpy(text_line_oled[2], "      por      ");
  strcpy(text_line_oled[3], "               ");
  strcpy(text_line_oled[4], "Magnetostriccao");
  strcpy(text_line_oled[5], "               ");
  strcpy(text_line_oled[6], "               ");
  strcpy(text_line_oled[7], "               ");

  y = 0;
  for (uint i = 0; i < count_of(text_line_oled); i++)
  {
    ssd1306_draw_string(ssd, 5, y, text_line_oled[i]);
    y += ssd1306_line_height;
  }
  render_on_display(ssd, &frame_area);

  sleep_ms(OLED_SCROOL_DELAY);

  strcpy(text_line_oled[0], "Transformadores");
  strcpy(text_line_oled[1], "      por      ");
  strcpy(text_line_oled[2], "               ");
  strcpy(text_line_oled[3], "Magnetostriccao");
  strcpy(text_line_oled[4], "               ");
  strcpy(text_line_oled[5], "               ");
  strcpy(text_line_oled[6], "               ");
  strcpy(text_line_oled[7], "               ");

  y = 0;
  for (uint i = 0; i < count_of(text_line_oled); i++)
  {
    ssd1306_draw_string(ssd, 5, y, text_line_oled[i]);
    y += ssd1306_line_height;
  }
  render_on_display(ssd, &frame_area);

  sleep_ms(OLED_SCROOL_DELAY);

  strcpy(text_line_oled[0], "      por      ");
  strcpy(text_line_oled[1], "               ");
  strcpy(text_line_oled[2], "Magnetostriccao");
  strcpy(text_line_oled[3], "               ");
  strcpy(text_line_oled[4], "               ");
  strcpy(text_line_oled[5], "               ");
  strcpy(text_line_oled[6], "               ");
  strcpy(text_line_oled[7], "               ");

  y = 0;
  for (uint i = 0; i < count_of(text_line_oled); i++)
  {
    ssd1306_draw_string(ssd, 5, y, text_line_oled[i]);
    y += ssd1306_line_height;
  }
  render_on_display(ssd, &frame_area);

  sleep_ms(OLED_SCROOL_DELAY);

  strcpy(text_line_oled[0], "               ");
  strcpy(text_line_oled[1], "Magnetostriccao");
  strcpy(text_line_oled[2], "               ");
  strcpy(text_line_oled[3], "               ");
  strcpy(text_line_oled[4], "               ");
  strcpy(text_line_oled[5], "               ");
  strcpy(text_line_oled[6], "               ");
  strcpy(text_line_oled[7], "               ");

  y = 0;
  for (uint i = 0; i < count_of(text_line_oled); i++)
  {
    ssd1306_draw_string(ssd, 5, y, text_line_oled[i]);
    y += ssd1306_line_height;
  }
  render_on_display(ssd, &frame_area);

  sleep_ms(OLED_SCROOL_DELAY);

  strcpy(text_line_oled[0], "Magnetostriccao");
  strcpy(text_line_oled[1], "               ");
  strcpy(text_line_oled[2], "               ");
  strcpy(text_line_oled[3], "               ");
  strcpy(text_line_oled[4], "               ");
  strcpy(text_line_oled[5], "               ");
  strcpy(text_line_oled[6], "               ");
  strcpy(text_line_oled[7], "               ");

  y = 0;
  for (uint i = 0; i < count_of(text_line_oled); i++)
  {
    ssd1306_draw_string(ssd, 5, y, text_line_oled[i]);
    y += ssd1306_line_height;
  }
  render_on_display(ssd, &frame_area);

  sleep_ms(OLED_SCROOL_DELAY);

  memset(ssd, 0, ssd1306_buffer_length);
  render_on_display(ssd, &frame_area);

}

/**
 * @brief Exibe mensagens de inicializacao e inicializa a TASKTinyML.
 */
bool start_tinyML(){
  strcpy(text_line_oled[0], "   Ativando    ");
  strcpy(text_line_oled[1], "               ");
  strcpy(text_line_oled[2], "               ");
  strcpy(text_line_oled[3], "    TinyML     ");
  strcpy(text_line_oled[4], "               ");
  strcpy(text_line_oled[5], " Redes Neurais ");
  strcpy(text_line_oled[6], "               ");
  strcpy(text_line_oled[7], "               ");

  // ssd1306_write_array(ssd, &frame_area, &text);
  uint8_t y = 0;
  for (uint i = 0; i < count_of(text_line_oled); i++)
  {
    ssd1306_draw_string(ssd, 5, y, text_line_oled[i]);
    y += ssd1306_line_height;
  }
  render_on_display(ssd, &frame_area);

  BaseType_t xReturn = xTaskCreate(
    task_tinyML, 
    "tinyML_task", 
    TASK_TINYML_STACK_SIZE,
    NULL, 
    TASK_TINYML_PRIORITY, 
    NULL);

  if (xReturn != pdPASS) {
    strcpy(text_line_oled[0], "     FALHA     ");
    strcpy(text_line_oled[1], " AO CRIAR TASK ");
    strcpy(text_line_oled[2], "      RTOS     ");
    strcpy(text_line_oled[3], "               ");
    strcpy(text_line_oled[4], "               ");
    strcpy(text_line_oled[5], "     tinyML    ");
    strcpy(text_line_oled[6], "               ");
    strcpy(text_line_oled[7], "               ");

    uint8_t y = 0;
    for (uint i = 0; i < count_of(text_line_oled); i++)
    {
      ssd1306_draw_string(ssd, 5, y, text_line_oled[i]);
      y += ssd1306_line_height;
    }
    render_on_display(ssd, &frame_area);

    sleep_ms(INTER_SCREEN_DELAY * 6);
    return false;
  }
  return true;
 
}
/**
 * @brief Exibe mensagens de inicializacao e inicializa a TASKTinyML.
 */
bool start_fft_filter(){
  strcpy(text_line_oled[0], "   Ativando    ");
  strcpy(text_line_oled[1], "               ");
  strcpy(text_line_oled[2], "               ");
  strcpy(text_line_oled[3], "               ");
  strcpy(text_line_oled[4], "               ");
  strcpy(text_line_oled[5], "   FFT filter  ");
  strcpy(text_line_oled[6], "               ");
  strcpy(text_line_oled[7], "               ");

  // ssd1306_write_array(ssd, &frame_area, &text);
  uint8_t y = 0;
  for (uint i = 0; i < count_of(text_line_oled); i++)
  {
    ssd1306_draw_string(ssd, 5, y, text_line_oled[i]);
    y += ssd1306_line_height;
  }
  render_on_display(ssd, &frame_area);

  xTinyML_Buffer_Queue = xQueueCreate( TINYML_QUEUE_LENGTH, ( UBaseType_t ) ADC_SAMPLES * sizeof( uint16_t ) );

  BaseType_t xReturn = xTaskCreate(
    task_fft_filter, 
    "FFT Filter task", 
    TASK_FFT_FILTER_STACK_SIZE,
    NULL, 
    TASK_FFT_FILTER_PRIORITY, 
    NULL);

  if (xReturn != pdPASS) {
    strcpy(text_line_oled[0], "     FALHA     ");
    strcpy(text_line_oled[1], " AO CRIAR TASK ");
    strcpy(text_line_oled[2], "      RTOS     ");
    strcpy(text_line_oled[3], "               ");
    strcpy(text_line_oled[4], "               ");
    strcpy(text_line_oled[5], "   FFT filter  ");
    strcpy(text_line_oled[6], "               ");
    strcpy(text_line_oled[7], "               ");

    uint8_t y = 0;
    for (uint i = 0; i < count_of(text_line_oled); i++)
    {
      ssd1306_draw_string(ssd, 5, y, text_line_oled[i]);
      y += ssd1306_line_height;
    }
    render_on_display(ssd, &frame_area);

    sleep_ms(INTER_SCREEN_DELAY * 6);
    return false;
  }
  return true;
 
}

/**
 * @brief Exibe mensagens de inicializacao e inicializa o ADC com DMA.
 *
 * @details
 *  - Exibe mensagens de inicializacao na tela.
 *  - Inicializa o ADC para capturar amostras do sinal de audio via DMA.
 *
 * @see adc_gpio_init, adc_init, adc_select_input, adc_fifo_setup, adc_set_clkdiv, dma_claim_unused_channel, channel_config_set_transfer_data_size, channel_config_set_read_increment, channel_config_set_write_increment, channel_config_set_dreq, sample_mic
 */
bool start_ADC_with_DMA(){
  strcpy(text_line_oled[0], "   Ativando    ");
  strcpy(text_line_oled[1], "               ");
  strcpy(text_line_oled[2], "      ADC      ");
  strcpy(text_line_oled[3], "               ");
  strcpy(text_line_oled[4], "    Por DMA    ");
  strcpy(text_line_oled[5], "               ");
  strcpy(text_line_oled[6], "               ");
  strcpy(text_line_oled[7], "               ");

  // ssd1306_write_array(ssd, &frame_area, &text);
  uint8_t y = 0;
  for (uint i = 0; i < count_of(text_line_oled); i++)
  {
    ssd1306_draw_string(ssd, 5, y, text_line_oled[i]);
    y += ssd1306_line_height;
  }
  render_on_display(ssd, &frame_area);
   
  xFFT_Buffer_Queue = xQueueCreate( ADC_QUEUE_LENGTH, ( UBaseType_t ) ADC_SAMPLES * sizeof( uint16_t ) );
  xIntensity_Buffer_Queue = xQueueCreate( ADC_INTENSITY_QUEUE_LENGTH, ( UBaseType_t ) sizeof( uint ) );
  BaseType_t xReturn = xTaskCreate(
    task_adc_with_dma, 
    "Task ADC com DMA", 
    TASK_ADC_DMA_STACK_SIZE,
    NULL, 
    TASK_ADC_DMA_PRIORITY, 
    NULL);
  if (xReturn != pdPASS) {
    strcpy(text_line_oled[0], "     FALHA     ");
    strcpy(text_line_oled[1], " AO CRIAR TASK ");
    strcpy(text_line_oled[2], "      RTOS     ");
    strcpy(text_line_oled[3], "               ");
    strcpy(text_line_oled[4], "               ");
    strcpy(text_line_oled[5], "  ADC COM DMA  ");
    strcpy(text_line_oled[6], "               ");
    strcpy(text_line_oled[7], "               ");

    uint8_t y = 0;
    for (uint i = 0; i < count_of(text_line_oled); i++)
    {
      ssd1306_draw_string(ssd, 5, y, text_line_oled[i]);
      y += ssd1306_line_height;
    }
    render_on_display(ssd, &frame_area);

    sleep_ms(INTER_SCREEN_DELAY * 6);
    return false;
  }
  return true;

}

/**
 * @brief Exibe mensagens de inicializacao e inicializa o VU de LEDs.
 * 
 * @details
 * INicializa o VU de LEDs para que seja possível monitorar o nível de audio 
 * captado.
 *
 * @see npInit
 */
bool start_VU_LED(){
  strcpy(text_line_oled[0], "   Ativando    ");
  strcpy(text_line_oled[1], "               ");
  strcpy(text_line_oled[2], "               ");
  strcpy(text_line_oled[3], "  VU de LEDs   ");
  strcpy(text_line_oled[4], "               ");
  strcpy(text_line_oled[5], "               ");
  strcpy(text_line_oled[6], "               ");
  strcpy(text_line_oled[7], "               ");

  // ssd1306_write_array(ssd, &frame_area, &text);
  uint8_t y = 0;
  for (uint i = 0; i < count_of(text_line_oled); i++)
  {
    ssd1306_draw_string(ssd, 5, y, text_line_oled[i]);
    y += ssd1306_line_height;
  }
  render_on_display(ssd, &frame_area);

  npInit(MATRIZ_LED_PIN, MATRIZ_LED_COUNT);

  BaseType_t xReturn = xTaskCreate(
    task_vu_leds,
    "Task de VU LEDs",
    TASK_VU_LEDS_STACK_SIZE,
    NULL,
    TASK_VU_LEDS_PRIORITY,
    NULL
  );
  
  if (xReturn != pdPASS) {
    strcpy(text_line_oled[0], "     FALHA     ");
    strcpy(text_line_oled[1], " AO CRIAR TASK ");
    strcpy(text_line_oled[2], "      RTOS     ");
    strcpy(text_line_oled[3], "               ");
    strcpy(text_line_oled[4], "               ");
    strcpy(text_line_oled[5], "     VU LEDs   ");
    strcpy(text_line_oled[6], "               ");
    strcpy(text_line_oled[7], "               ");

    uint8_t y = 0;
    for (uint i = 0; i < count_of(text_line_oled); i++)
    {
      ssd1306_draw_string(ssd, 5, y, text_line_oled[i]);
      y += ssd1306_line_height;
    }
    render_on_display(ssd, &frame_area);

    return false;
  }
  return true;
}

/**
 * @brief Exibe mensagens de inicializacao e configura os botoes A e B
 *        para serem monitorados por interrupcoes.
 *
 * @details
 *  - Exibe mensagens de inicializacao na tela.
 *  - Configura os botoes A e B para serem monitorados por interrupcoes. 
 *
 * @see task_buttons_control
 */
void start_bottons_control(){
  strcpy(text_line_oled[0], "   Ativando    ");
  strcpy(text_line_oled[1], "               ");
  strcpy(text_line_oled[2], "               ");
  strcpy(text_line_oled[3], " configuracoes ");
  strcpy(text_line_oled[4], "               ");
  strcpy(text_line_oled[5], "  dos botoes   ");
  strcpy(text_line_oled[6], "               ");
  strcpy(text_line_oled[7], "               ");

  // ssd1306_write_array(ssd, &frame_area, &text);
  uint8_t y = 0;
  for (uint i = 0; i < count_of(text_line_oled); i++)
  {
    ssd1306_draw_string(ssd, 5, y, text_line_oled[i]);
    y += ssd1306_line_height;
  }
  render_on_display(ssd, &frame_area);

  gpio_init(BUTTON_A_PIN);
  gpio_set_dir(BUTTON_A_PIN, GPIO_IN);
  gpio_pull_up(BUTTON_A_PIN);
  gpio_set_irq_enabled_with_callback(BUTTON_A_PIN, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, true, &monitor_buttons_callback);

  strcpy(text_line_oled[7], "   Botao A   ");

  // ssd1306_write_array(ssd, &frame_area, &text);
  y = 0;
  for (uint i = 0; i < count_of(text_line_oled); i++)
  {
    ssd1306_draw_string(ssd, 5, y, text_line_oled[i]);
    y += ssd1306_line_height;
  }
  render_on_display(ssd, &frame_area);

  sleep_ms(500);
  gpio_init(BUTTON_B_PIN);
  gpio_set_dir(BUTTON_B_PIN, GPIO_IN);
  gpio_pull_up(BUTTON_B_PIN);
  gpio_set_irq_enabled_with_callback(BUTTON_B_PIN, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, true, &monitor_buttons_callback);

  strcpy(text_line_oled[7], "   Botao B   ");

  // ssd1306_write_array(ssd, &frame_area, &text);
  y = 0;
  for (uint i = 0; i < count_of(text_line_oled); i++)
  {
    ssd1306_draw_string(ssd, 5, y, text_line_oled[i]);
    y += ssd1306_line_height;
  }
  render_on_display(ssd, &frame_area);

}

/**
 * @brief Inicializa e verifica atuadores e sensores do drone.
 *
 * @details
 *  - Exibe mensagens de inicializacao na tela. 
 *  - Cria a task task_drone_control que ira controlar o drone.
 *
 * @see task_drone_control
 */
bool start_gpio_and_drone_control(){
  strcpy(text_line_oled[0], " Inicializando ");
  strcpy(text_line_oled[1], "               ");
  strcpy(text_line_oled[2], "               ");
  strcpy(text_line_oled[3], "     GPIO      ");
  strcpy(text_line_oled[4], "               ");
  strcpy(text_line_oled[5], " DRONE CONTROL ");
  strcpy(text_line_oled[6], "               ");
  strcpy(text_line_oled[7], "               ");

  uint8_t y = 0;
  for (uint i = 0; i < count_of(text_line_oled); i++)
  {
    ssd1306_draw_string(ssd, 5, y, text_line_oled[i]);
    y += ssd1306_line_height;
  }
  render_on_display(ssd, &frame_area);

  /////////////////////////////////////////////////////////
  // inicializa e verifica atuadores e sensores do drone //
  /////////////////////////////////////////////////////////
 
  // ativa adc do joistick
  adc_gpio_init(26); 
  adc_gpio_init(27);

  BaseType_t xReturn = xTaskCreate(
    task_drone_control,
    "Task Drone Control",
    TASK_DRONE_CONTROL_STACK_SIZE,
    NULL,
    TASK_DRONE_CONTROL_PRIORITY,
    NULL
  );
    if (xReturn != pdPASS) {
    strcpy(text_line_oled[0], "     FALHA     ");
    strcpy(text_line_oled[1], " AO CRIAR TASK ");
    strcpy(text_line_oled[2], "      RTOS     ");
    strcpy(text_line_oled[3], "               ");
    strcpy(text_line_oled[4], "               ");
    strcpy(text_line_oled[5], " CONTROLE DRONE");
    strcpy(text_line_oled[6], "               ");
    strcpy(text_line_oled[7], "               ");

    uint8_t y = 0;
    for (uint i = 0; i < count_of(text_line_oled); i++)
    {
      ssd1306_draw_string(ssd, 5, y, text_line_oled[i]);
      y += ssd1306_line_height;
    }
    render_on_display(ssd, &frame_area);

    sleep_ms(INTER_SCREEN_DELAY * 6);
    return false;
  }
  return true;

}

/**
 * Inicia a infraestrutura de rede.
 */
bool start_network_infrastructure(){
  if (cyw43_arch_init_with_country(CYW43_COUNTRY_BRAZIL))
  {
    strcpy(text_line_oled[0], "    ATENCAO    ");
    strcpy(text_line_oled[1], "    FALHA NA   ");
    strcpy(text_line_oled[2], "               ");
    strcpy(text_line_oled[3], " Infraestrutra ");
    strcpy(text_line_oled[4], "    de Rede    ");
    strcpy(text_line_oled[5], "               ");
    strcpy(text_line_oled[6], "     Wi-FI     ");
    strcpy(text_line_oled[7], "               ");

    uint8_t y = 0;
    for (uint i = 0; i < count_of(text_line_oled); i++)
    {
      ssd1306_draw_string(ssd, 5, y, text_line_oled[i]);
      y += ssd1306_line_height;
    }
    render_on_display(ssd, &frame_area);
    return false;
  }

  cyw43_arch_enable_sta_mode();
  uint8_t count = 0;
  while(count < 3 && cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 10000) != 0){
    strcpy(text_line_oled[0], "    ATENCAO    ");
    strcpy(text_line_oled[1], "    FALHA NA   ");
    strcpy(text_line_oled[2], "               ");
    strcpy(text_line_oled[3], " Infraestrutra ");
    strcpy(text_line_oled[4], "    de Rede    ");
    strcpy(text_line_oled[5], "     Wi-FI     ");
    strcpy(text_line_oled[6], " NAO CONECTADO ");
    sprintf(text_line_oled[7], " Tentativa: %02d", ++count);
    uint8_t y = 0;
    for (uint i = 0; i < count_of(text_line_oled); i++)
    {
      ssd1306_draw_string(ssd, 5, y, text_line_oled[i]);
      y += ssd1306_line_height;
    }
    render_on_display(ssd, &frame_area);
    sleep_ms(INTER_SCREEN_DELAY/(1.0/3));
  }
    
  if (count==3) return false;
  
  // Read the ip address in a human readable way
  uint8_t *ip_address = (uint8_t *)&(cyw43_state.netif[0].ip_addr.addr);
  strcpy(text_line_oled[0], " INICIALIZANDO ");
  strcpy(text_line_oled[1], "               ");
  strcpy(text_line_oled[3], " Infraestrutra ");
  strcpy(text_line_oled[4], "    de Rede    ");
  strcpy(text_line_oled[5], "     Wi-FI     ");
  strcpy(text_line_oled[6], "   CONECTADO   ");
  strcpy(text_line_oled[2], "               ");
  sprintf(text_line_oled[7], "%03d.%03d.%03d.%03d", ip_address[0], ip_address[1], ip_address[2], ip_address[3]);
  uint8_t y = 0;
  for (uint i = 0; i < count_of(text_line_oled); i++)
  {
    ssd1306_draw_string(ssd, 5, y, text_line_oled[i]);
    y += ssd1306_line_height;
  }
  render_on_display(ssd, &frame_area);

  // Inicia o servidor HTTP
  start_http_server();
  BaseType_t xReturn = xTaskCreate(
    task_http_server, 
    "Task HTTP Server", 
    TASK_HTTP_SERVER_STACK_SIZE, 
    NULL, 
    TASK_HTTP_SERVER_PRIORITY, 
    NULL);

  if (xReturn != pdPASS) {
    strcpy(text_line_oled[0], "     FALHA     ");
    strcpy(text_line_oled[1], " AO CRIAR TASK ");
    strcpy(text_line_oled[2], "      RTOS     ");
    strcpy(text_line_oled[3], "               ");
    strcpy(text_line_oled[4], "               ");
    strcpy(text_line_oled[5], " INFRAESTRUTURA");
    strcpy(text_line_oled[6], "     REDE      ");
    strcpy(text_line_oled[7], "               ");

    uint8_t y = 0;
    for (uint i = 0; i < count_of(text_line_oled); i++)
    {
      ssd1306_draw_string(ssd, 5, y, text_line_oled[i]);
      y += ssd1306_line_height;
    }
    render_on_display(ssd, &frame_area);

    sleep_ms(INTER_SCREEN_DELAY * 6);
    return false;
  }
  return true;

}

// #########################################
//                WIFI e WEB Server

// Função de setup do servidor TCP
static void start_http_server(void)
{
  struct tcp_pcb *pcb = tcp_new();
  if (!pcb)
  {
    strcpy(text_line_oled[0], " INICIALIZANDO ");
    strcpy(text_line_oled[1], "               ");
    strcpy(text_line_oled[2], " Infraestrutra ");
    strcpy(text_line_oled[3], "    de Rede    ");
    strcpy(text_line_oled[4], "     Wi-FI     ");
    strcpy(text_line_oled[5], "               ");
    strcpy(text_line_oled[6], " Erro criando  ");;
    strcpy(text_line_oled[7], "      PCB      ");

    uint8_t y = 0;
    for (uint i = 0; i < count_of(text_line_oled); i++)
    {
      ssd1306_draw_string(ssd, 5, y, text_line_oled[i]);
      y += ssd1306_line_height;
    }
    render_on_display(ssd, &frame_area);
    return;
  }

  // Liga o servidor na porta 80
  if (tcp_bind(pcb, IP_ADDR_ANY, 80) != ERR_OK)
  {
  strcpy(text_line_oled[0], " INICIALIZANDO ");
    strcpy(text_line_oled[1], "               ");
    strcpy(text_line_oled[2], " Infraestrutra ");
    strcpy(text_line_oled[3], "    de Rede    ");
    strcpy(text_line_oled[4], "     Wi-FI     ");
    strcpy(text_line_oled[5], "               ");
    strcpy(text_line_oled[6], " Erro conectar ");;
    strcpy(text_line_oled[7], " serv. porta 80");

    uint8_t y = 0;
    for (uint i = 0; i < count_of(text_line_oled); i++)
    {
      ssd1306_draw_string(ssd, 5, y, text_line_oled[i]);
      y += ssd1306_line_height;
    }
    render_on_display(ssd, &frame_area);
    return;
  }

  pcb = tcp_listen(pcb);                // Coloca o PCB em modo de escuta
  tcp_accept(pcb, connection_callback); // Associa o callback de conexão

    strcpy(text_line_oled[0], " INICIALIZANDO ");
    strcpy(text_line_oled[1], "               ");
    strcpy(text_line_oled[2], " Infraestrutra ");
    strcpy(text_line_oled[3], "    de Rede    ");
    strcpy(text_line_oled[4], "     Wi-FI     ");
    strcpy(text_line_oled[5], "               ");
    strcpy(text_line_oled[6], " SERRVIDOR WEB ");;
    strcpy(text_line_oled[7], "   CONECTADO   ");

    uint8_t y = 0;
    for (uint i = 0; i < count_of(text_line_oled); i++)
    {
      ssd1306_draw_string(ssd, 5, y, text_line_oled[i]);
      y += ssd1306_line_height;
    }
    render_on_display(ssd, &frame_area);
}

// Estado dos botões (inicialmente sem mensagens)
char button_A_message[50] = "Nenhum evento no botão 1";
char button_B_message[50] = "Nenhum evento no botão 2";

// Buffer para resposta HTTP
char http_response[1024];

// Função para criar a resposta HTTP
char json_response[1024 * 8]; // tamanho a ser definido

/**
 * Função para criar a resposta JSON
 */
void create_json_update_response(){
  // constroi a respsota json
  // a resposta deve conter a situação atual do sistema
  // isso inclue a localização obtida no gps do drone
  // aquecimento interno do processador
  // e a situação do acoplador acustico
  snprintf(http_response, sizeof(http_response),
           "HTTP/1.1 200 OK\r\nContent-Type: text/json; charset=UTF-8\r\n\r\n"
           "{\r\n"
           // em data vai os valores para cada amostra obtida
           "  \"localizacao\": {\r\n "
           "    \"latitude\": 0, \"longitude\": 0},\r\n"
           "  \"temperatura\": 0,\r\n"
           "  \"acoplador\": 0,\r\n"
           "}\r\n"
           "}\r\n"
           "\r\n");
}
void create_json_noise_response(){
  /// constroi resposta em sjson, 
  /// talvez cjson, ou uma solução mais simples já que será enviados 
  /// apenas uma sequência de números que representa as amostragem do ADC
  snprintf(http_response, sizeof(http_response),
           "HTTP/1.1 200 OK\r\nContent-Type: text/json; charset=UTF-8\r\n\r\n"
           "{\r\n"
           // em data vai os valores para cada amostra obtida
           "  \"amostragens\": {\r\n "
           "    \"order\": 0, \"data\":[123, 123, 123, 123, 123, 123, 123, 123, 123, 123]},\r\n"
           "    \"order\": 1, \"data\":[43, 43, 43, 43, 43, 43, 43, 43, 43, 43]},\r\n"
           "    \"order\": 2, \"data\":[43, 43, 43, 43, 43, 43, 43, 43, 43, 43]},\r\n"
           "    \"order\": 3, \"data\":[43, 43, 43, 43, 43, 43, 43, 43, 43, 43]},\r\n"
           "}\r\n"
           "}\r\n"
           "\r\n");
  
}

/**
 * Função para criar a resposta HTTP generica
 */
 void create_http_response()
{
  snprintf(http_response, sizeof(http_response), HTTP_RESPONSE,
           button_A_message, button_B_message);
}

/**
 * Função de callback para processar requisições HTTP
 */
static err_t http_callback(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err)
{
  if (p == NULL)
  {
    // Cliente fechou a conexão
    tcp_close(tpcb);
    return ERR_OK;
  }

  // Processa a requisição HTTP
  char *request = (char *)p->payload;

  // Verifica qual é a requisição
  if (strstr(request, "GET /acustic_angle/"))
  {
    /**
     * ajusta o angulo de captação do acoplador acustico
     */
  }else if (strstr(request, "POST /aceleration/xy"))
  {
    if (!modo_local)
    {
      // obtem na requisição a posição x e y desejada
      // aceleration_x = x;
      // aceleration_y = y;
    }

    // Envia a resposta HTTP
    create_http_response();
    tcp_write(tpcb, http_response, strlen(http_response), TCP_WRITE_FLAG_COPY);

  }else if (strstr(request, "POST /aceleration/x"))
  {
    // obtem na requisição o valor do eixo X
    if (!modo_local)
    {
      // obtem na requisição a posição x desejada
      // aceleration_x = x;
    }

    // Envia a resposta HTTP
    create_http_response();
    tcp_write(tpcb, http_response, strlen(http_response), TCP_WRITE_FLAG_COPY);
  }
  else if (strstr(request, "POST /aceleration/y"))
    {
      // obtem na requisição o valor do eixo Y
      if (!modo_local)
      {
        // obtem na requisição a posição y desejada
        // aceleration_y = y;
      }
      
    // Envia a resposta HTTP
    create_http_response();
    tcp_write(tpcb, http_response, strlen(http_response), TCP_WRITE_FLAG_COPY);
  }
  else if (strstr(request, "GET /update"))
  {
    // deve enviar a situação do drone
    // Envia a resposta HTTP
    create_json_update_response();
    tcp_write(tpcb, json_response, strlen(json_response), TCP_WRITE_FLAG_COPY);
  }
  else if (strstr(request, "GET /noise"))
  {
    // Envia a resposta HTTP
    // obtem o array de amostras de ruidos coletados

    create_json_noise_response();
    tcp_write(tpcb, json_response, strlen(json_response), TCP_WRITE_FLAG_COPY);
  }

  // Libera o buffer recebido
  pbuf_free(p);

  return ERR_OK;
}

// Callback de conexão: associa o http_callback à conexão
static err_t connection_callback(void *arg, struct tcp_pcb *newpcb, err_t err)
{
  tcp_recv(newpcb, http_callback); // Associa o callback HTTP
  return ERR_OK;
}

// Função para monitorar o estado dos botões
static void monitor_buttons_callback(unsigned int gpio, long unsigned int events)
{
  static bool button_A_last_state = false;
  static bool button_B_last_state = false;

  bool button_A_state = !gpio_get(BUTTON_A_PIN); // Botão pressionado = LOW
  bool button_B_state = !gpio_get(BUTTON_B_PIN);

  if (button_A_state != button_A_last_state)
  {
    button_A_last_state = button_A_state;
    if (button_A_state)
    {
      snprintf(button_A_message, sizeof(button_A_message), "Botão A foi pressionado!");
      uint8_t *ip_address = (uint8_t *)&(cyw43_state.netif[0].ip_addr.addr);
      sprintf(text_line_oled[7], "%03d.%03d.%03d.%03d", ip_address[0], ip_address[1], ip_address[2], ip_address[3]);
    }
    else
    {
      snprintf(button_A_message, sizeof(button_A_message), "Botão A foi solto!");
      strcpy(text_line_oled[7], "               ");
    }
    uint y = 0;
  }

  if (button_B_state != button_B_last_state)
  {
    button_B_last_state = button_B_state;
    if (button_B_state)
    {
      snprintf(button_B_message, sizeof(button_B_message), "Botão B foi pressionado!");
      modo_local = !modo_local;
      if (modo_local)
        strcpy(text_line_oled[7], "   modo local  ");
      else
        strcpy(text_line_oled[7], "  modo remoto  ");
    }
    else
    {
      snprintf(button_B_message, sizeof(button_B_message), "Botão B foi solto!");
    }
  }
}