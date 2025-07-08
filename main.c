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
#include "hardware/clocks.h"

#include "hardware/adc.h"
#include "hardware/dma.h"

#include "lwip/tcp.h"

#include "oled.h"

#include <stdbool.h>
#include <stdint.h>

#include "inc/neopixel.h"

#include "inc/magnectostriccao.h"   // Carrega parametros gerais do sistema
#include "tasks/tasks_paramiters.h" // parametros para criação das tasks

#include "tasks/task_adc_with_dma.h"  // carrega tarefas do ADC
#include "tasks/task_display_oled.h"  // Carrega tarefas do display
#include "tasks/task_drone_control.h" // Carrega tarefas do controle de drone
#include "tasks/task_tinyML.h"        // Carrega tarefas do TinyML
#include "tasks/task_fft_filter.h"    // Carrega tarefas do filtro FFT
#include "tasks/task_http_server.h"   // Carrega tarefas do http server
#include "tasks/task_fft_filter.h"    // Carrega tarefas do filtro FFT
#include "tasks/task_vu_leds.h"       // Carrega tarefas do leds de volume
#include "main.h"                     // carrega cabeçalhos do main

// buffer de texto para o display oled
// definido no arquivo de tarefas do display oled
// tasks/task_display_oled.h

// Preparar área de renderização para o display (ssd1306_width pixels por ssd1306_n_pages páginas)

// zera o display inteiro


// Buffer de amostras do ADC.
static uint32_t last_time;
// flag que indica se o controle do drone é local ou remoto
bool modo_local = false;
// aceleração obitida no servidor restfull
int aceleration_x;
int aceleration_y;

QueueHandle_t xFFT_Buffer_Queue = NULL;
QueueHandle_t xTinyML_Buffer_Queue = NULL;
QueueHandle_t xIntensity_Buffer_Queue = NULL;

int main()
{
  stdio_init_all(); // Inicializa os tipos stdio padrão presentes ligados ao binário

  init_display_oled();

  show_intro();

  sleep_ms(INTER_SCREEN_DELAY);

  if (!start_VU_LED())
    return 1;

  sleep_ms(INTER_SCREEN_DELAY);

  start_bottons_control();

  sleep_ms(INTER_SCREEN_DELAY);

  if (!start_ADC_with_DMA())
    return 1;

  sleep_ms(INTER_SCREEN_DELAY);

  if (!start_tinyML())
    return 1;

  sleep_ms(INTER_SCREEN_DELAY);

  if (!start_fft_filter())
    return 1;

  sleep_ms(INTER_SCREEN_DELAY);

  if (!start_network_infrastructure())
    return 1;

  sleep_ms(INTER_SCREEN_DELAY);

  if (!start_gpio_and_drone_control())
    return 1;

  sleep_ms(INTER_SCREEN_DELAY);

  if (!start_display_oled())
    return 1;

  sleep_ms(INTER_SCREEN_DELAY);

  oled_set_text_line(0, "               ");
  oled_set_text_line(1, "Inicializando o");
  oled_set_text_line(2, "               ");
  oled_set_text_line(3, "  escalonador  ");
  oled_set_text_line(4, "               ");
  oled_set_text_line(5, "      de       ");
  oled_set_text_line(6, "    rarefas    ");
  oled_set_text_line(7, "               ");
  oled_render_text();

  vTaskStartScheduler();

  return 0;
}

bool start_display_oled()
{
  BaseType_t xReturn = xTaskCreate(
      task_display_oled,
      "Task que mantem o display OLED atualizado",
      TASK_DISPLAY_OLED_STACK_SIZE,
      NULL,
      TASK_DISPLAY_OLED_PRIORITY,
      NULL);

  if (xReturn != pdPASS)
  {
    oled_set_text_line(0, "     FALHA     ");
    oled_set_text_line(1, " AO CRIAR TASK ");
    oled_set_text_line(2, "      RTOS     ");
    oled_set_text_line(3, "               ");
    oled_set_text_line(4, "               ");
    oled_set_text_line(5, " DISPLAY OLED  ");
    oled_set_text_line(6, "               ");
    oled_set_text_line(7, "               ");
    oled_render_text();

    sleep_ms(INTER_SCREEN_DELAY * 6);
    return false;
  }
  return true;
}

void init_display_oled()
{
  oled_init();

  oled_clear();

  for (char i = '0'; i <= '9'; i++) {  
    oled_draw_big_char(56, 16, i);
    oled_render();
    sleep_ms(100);
  }

  // Print all printable ASCII characters on OLED for testing
  const int first_char = 32;
  const int last_char = 126;
  char buf[max_text_columns + 1];
  for (int base = first_char; base <= last_char; base += max_text_columns) {
    for (int i = 0; i < max_text_columns; ++i) {
      int ch = base + i;
      buf[i] = (ch <= last_char) ? (char)ch : ' ';
    }
    buf[max_text_columns] = '\0';

    // Clear and render buffer
    oled_clear();
    oled_draw_string(0, 0, buf);
    oled_render();
    sleep_ms(500);
  }

  // Clear after test
  oled_clear();
}
/**
 * Mostra a tela de abertura do projeto.
 */
void show_intro()
{
  static const char *slides[][max_text_lines] = {
      {"    Projeto    ", "     Final     ", "               ", "  EmbarcaTech  ", "               ", "   Residencia  ", "     TIC 37    ", "               "},
      {"  Analizador   ", "  da Saude de  ", "               ", "Transformadores", "      por      ", "               ", "Magnetostriccao", "               "},
      {"  da Saude de  ", "               ", "Transformadores", "      por      ", "               ", "Magnetostriccao", "               ", "               "},
      {"               ", "Transformadores", "      por      ", "               ", "Magnetostriccao", "               ", "               ", "               "},
      {"Transformadores", "      por      ", "               ", "Magnetostriccao", "               ", "               ", "               ", "               "},
      {"      por      ", "               ", "Magnetostriccao", "               ", "               ", "               ", "               ", "               "},
      {"               ", "Magnetostriccao", "               ", "               ", "               ", "               ", "               ", "               "},
      {"Magnetostriccao", "               ", "               ", "               ", "               ", "               ", "               ", "               "}
  };
  const uint32_t delays[] = {
      INTER_SCREEN_DELAY * 3,
      (INTER_SCREEN_DELAY * 5) / 4,
      OLED_SCROOL_DELAY,
      OLED_SCROOL_DELAY,
      OLED_SCROOL_DELAY,
      OLED_SCROOL_DELAY,
      OLED_SCROOL_DELAY,
      OLED_SCROOL_DELAY
  };
  for (size_t slide = 0; slide < sizeof(slides)/sizeof(slides[0]); ++slide) {
      for (uint8_t i = 0; i < max_text_lines; ++i) {
          oled_set_text_line(i, slides[slide][i]);
      }
      oled_render_text();
      sleep_ms(delays[slide]);
  }
  oled_render();
}

/**
 * @brief Exibe mensagens de inicializacao e inicializa a TASKTinyML.
 */
bool start_tinyML()
{
  oled_set_text_line(0, "   Ativando    ");
  oled_set_text_line(1, "               ");
  oled_set_text_line(2, "               ");
  oled_set_text_line(3, "    TinyML     ");
  oled_set_text_line(4, "               ");
  oled_set_text_line(5, " Redes Neurais ");
  oled_set_text_line(6, "               ");
  oled_set_text_line(7, "               ");
  oled_render_text();

  BaseType_t xReturn = xTaskCreate(
      task_tinyML,
      "tinyML_task",
      TASK_TINYML_STACK_SIZE,
      NULL,
      TASK_TINYML_PRIORITY,
      NULL);

  if (xReturn != pdPASS)
  {
    oled_set_text_line(0, "     FALHA     ");
    oled_set_text_line(1, " AO CRIAR TASK ");
    oled_set_text_line(2, "      RTOS     ");
    oled_set_text_line(3, "               ");
    oled_set_text_line(4, "               ");
    oled_set_text_line(5, "     tinyML    ");
    oled_set_text_line(6, "               ");
    oled_set_text_line(7, "               ");
    oled_render_text();

    sleep_ms(INTER_SCREEN_DELAY * 6);
    return false;
  }
  return true;
}

/**
 * @brief Exibe mensagens de inicializacao e inicializa a TASKTinyML.
 */
bool start_fft_filter()
{
  oled_set_text_line(0, "   Ativando    ");
  oled_set_text_line(1, "               ");
  oled_set_text_line(2, "               ");
  oled_set_text_line(3, "               ");
  oled_set_text_line(4, "               ");
  oled_set_text_line(5, "   FFT filter  ");
  oled_set_text_line(6, "               ");
  oled_set_text_line(7, "               ");
  oled_render_text();

  xTinyML_Buffer_Queue = xQueueCreate(TINYML_QUEUE_LENGTH, (UBaseType_t)ADC_SAMPLES * sizeof(uint16_t));

  BaseType_t xReturn = xTaskCreate(
      task_fft_filter,
      "FFT Filter task",
      TASK_FFT_FILTER_STACK_SIZE,
      NULL,
      TASK_FFT_FILTER_PRIORITY,
      NULL);

  if (xReturn != pdPASS)
  {
    oled_set_text_line(0, "     FALHA     ");
    oled_set_text_line(1, " AO CRIAR TASK ");
    oled_set_text_line(2, "      RTOS     ");
    oled_set_text_line(3, "               ");
    oled_set_text_line(4, "               ");
    oled_set_text_line(5, "   FFT filter  ");
    oled_set_text_line(6, "               ");
    oled_set_text_line(7, "               ");
    oled_render_text();

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
bool start_ADC_with_DMA()
{
  oled_set_text_line(0, "   Ativando    ");
  oled_set_text_line(1, "               ");
  oled_set_text_line(2, "      ADC      ");
  oled_set_text_line(3, "               ");
  oled_set_text_line(4, "    Por DMA    ");
  oled_set_text_line(5, "               ");
  oled_set_text_line(6, "               ");
  oled_set_text_line(7, "               ");
  oled_render_text();

  // ssd1306_write_array(ssd, &frame_area, &text);
  uint8_t y = 0;
  oled_clear();
  oled_render_text();

  xFFT_Buffer_Queue = xQueueCreate(ADC_QUEUE_LENGTH, (UBaseType_t)ADC_SAMPLES * sizeof(uint16_t));
  xIntensity_Buffer_Queue = xQueueCreate(ADC_INTENSITY_QUEUE_LENGTH, (UBaseType_t)sizeof(uint));
  BaseType_t xReturn = xTaskCreate(
      task_adc_with_dma,
      "Task ADC com DMA",
      TASK_ADC_DMA_STACK_SIZE,
      NULL,
      TASK_ADC_DMA_PRIORITY,
      NULL);
  if (xReturn != pdPASS)
  {
    oled_set_text_line(0, "     FALHA     ");
    oled_set_text_line(1, " AO CRIAR TASK ");
    oled_set_text_line(2, "      RTOS     ");
    oled_set_text_line(3, "               ");
    oled_set_text_line(4, "               ");
    oled_set_text_line(5, "  ADC COM DMA  ");
    oled_set_text_line(6, "               ");
    oled_set_text_line(7, "               ");
    oled_render_text();

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
bool start_VU_LED()
{
  oled_set_text_line(0, "   Ativando    ");
  oled_set_text_line(1, "               ");
  oled_set_text_line(2, "               ");
  oled_set_text_line(3, "  VU de LEDs   ");
  oled_set_text_line(4, "               ");
  oled_set_text_line(5, "               ");
  oled_set_text_line(6, "               ");
  oled_set_text_line(7, "               ");
  oled_render_text();

  // ssd1306_write_array(ssd, &frame_area, &text);
  uint8_t y = 0;
  oled_clear();
  oled_render_text();

  npInit(MATRIZ_LED_PIN, MATRIZ_LED_COUNT);

  printf("VU de LEDs inicializada \n");
  // acente os para diagnostico da task com vu
  npSetLED(12, 0, 0, 80); // Acende apenas o centro.
  printf("Acende apenas o centro.\n");
  npWrite();
  sleep_ms((500));

  npSetLED(12, 0, 0, 120); // Acente o centro.
  printf("Acente o centro.\n");
  
  // Primeiro anel.
  npSetLED(7, 0, 0, 80);
  npSetLED(11, 0, 0, 80);
  npSetLED(13, 0, 0, 80);
  npSetLED(17, 0, 0, 80);
  printf("Primeiro anel.\n");
  npWrite();
  sleep_ms((500));

  // Centro.
  npSetLED(12, 60, 60, 0);
  printf("Centro.\n");

  // Primeiro anel.
  npSetLED(7, 0, 0, 120);
  npSetLED(11, 0, 0, 120);
  npSetLED(13, 0, 0, 120);
  npSetLED(17, 0, 0, 120);
  printf("Primeiro anel.\n");

  // Segundo anel.
  npSetLED(2, 0, 0, 80);
  npSetLED(6, 0, 0, 80);
  npSetLED(8, 0, 0, 80);
  npSetLED(10, 0, 0, 80);
  npSetLED(14, 0, 0, 80);
  npSetLED(16, 0, 0, 80);
  npSetLED(18, 0, 0, 80);
  npSetLED(22, 0, 0, 80);
  printf("Segundo anel.\n");
  npWrite();
  sleep_ms((500));

  // Centro.
  npSetLED(12, 80, 0, 0);
  printf("Centro.\n");

  // Primeiro anel.
  npSetLED(7, 60, 60, 0);
  npSetLED(11, 60, 60, 0);
  npSetLED(13, 60, 60, 0);
  npSetLED(17, 60, 60, 0);
  printf("Primeiro anel.\n");

  // Segundo anel.
  npSetLED(2, 0, 0, 120);
  npSetLED(6, 0, 0, 120);
  npSetLED(8, 0, 0, 120);
  npSetLED(10, 0, 0, 120);
  npSetLED(14, 0, 0, 120);
  npSetLED(16, 0, 0, 120);
  npSetLED(18, 0, 0, 120);
  npSetLED(22, 0, 0, 120);
  printf("Segundo anel.\n");

  // Terceiro anel.
  npSetLED(1, 0, 0, 80);
  npSetLED(3, 0, 0, 80);
  npSetLED(5, 0, 0, 80);
  npSetLED(9, 0, 0, 80);
  npSetLED(15, 0, 0, 80);
  npSetLED(19, 0, 0, 80);
  npSetLED(21, 0, 0, 80);
  npSetLED(23, 0, 0, 80);
  printf("Terceiro anel.\n");
  npWrite();
  sleep_ms((500));

  npClear();
  npWrite();

  BaseType_t xReturn = xTaskCreate(
      task_vu_leds,
      "Task de VU LEDs",
      TASK_VU_LEDS_STACK_SIZE,
      NULL,
      TASK_VU_LEDS_PRIORITY,
      NULL);

  if (xReturn != pdPASS)
  {
    oled_set_text_line(0, "     FALHA     ");
    oled_set_text_line(1, " AO CRIAR TASK ");
    oled_set_text_line(2, "      RTOS     ");
    oled_set_text_line(3, "               ");
    oled_set_text_line(4, "               ");
    oled_set_text_line(5, "     VU LEDs   ");
    oled_set_text_line(6, "               ");
    oled_set_text_line(7, "               ");
    oled_render_text();

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
void start_bottons_control()
{
  oled_set_text_line(0, "   Ativando    ");
  oled_set_text_line(1, "               ");
  oled_set_text_line(2, "               ");
  oled_set_text_line(3, " configuracoes ");
  oled_set_text_line(4, "               ");
  oled_set_text_line(5, "  dos botoes   ");
  oled_set_text_line(6, "               ");
  oled_set_text_line(7, "               ");
  oled_render_text();

  gpio_init(BUTTON_A_PIN);
  gpio_set_dir(BUTTON_A_PIN, GPIO_IN);
  gpio_pull_up(BUTTON_A_PIN);
  gpio_set_irq_enabled_with_callback(BUTTON_A_PIN, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, true, &monitor_buttons_callback);

  oled_set_text_line(7, "   Botao A   ");
  oled_render_text();

  sleep_ms(500);
  gpio_init(BUTTON_B_PIN);
  gpio_set_dir(BUTTON_B_PIN, GPIO_IN);
  gpio_pull_up(BUTTON_B_PIN);
  gpio_set_irq_enabled_with_callback(BUTTON_B_PIN, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, true, &monitor_buttons_callback);

  oled_set_text_line(7, "   Botao B   ");
  oled_render_text();
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
bool start_gpio_and_drone_control()
{
  oled_set_text_line(0, " Inicializando ");
  oled_set_text_line(1, "               ");
  oled_set_text_line(2, "               ");
  oled_set_text_line(3, "     GPIO      ");
  oled_set_text_line(4, "               ");
  oled_set_text_line(5, " DRONE CONTROL ");
  oled_set_text_line(6, "               ");
  oled_set_text_line(7, "               ");
  oled_render_text();

  /////////////////////////////////////////////////////////
  // inicializa e verifica atuadores e sensores do drone //
  /////////////////////////////////////////////////////////

  // ativa adc do joistick
  adc_gpio_init(26);
  adc_gpio_init(27);

  BaseType_t xReturn = xTaskCreate(
      task_drone_control,
      "Task Drone Ctrl",
      TASK_DRONE_CONTROL_STACK_SIZE,
      NULL,
      TASK_DRONE_CONTROL_PRIORITY,
      NULL);
  if (xReturn != pdPASS)
  {
    oled_set_text_line(0, "     FALHA     ");
    oled_set_text_line(1, " AO CRIAR TASK ");
    oled_set_text_line(2, "      RTOS     ");
    oled_set_text_line(3, "               ");
    oled_set_text_line(4, "               ");
    oled_set_text_line(5, " CONTROLE DRONE");
    oled_set_text_line(6, "               ");
    oled_set_text_line(7, "               ");
    oled_render_text();

    sleep_ms(INTER_SCREEN_DELAY * 6);
    return false;
  }
  return true;
}

/**
 * Inicia a infraestrutura de rede.
 */
bool start_network_infrastructure()
{
  if (cyw43_arch_init_with_country(CYW43_COUNTRY_BRAZIL))
  {
    oled_set_text_line(0, "    ATENCAO    ");
    oled_set_text_line(1, "    FALHA NA   ");
    oled_set_text_line(2, "               ");
    oled_set_text_line(3, " Infraestrutra ");
    oled_set_text_line(4, "    de Rede    ");
    oled_set_text_line(5, "               ");
    oled_set_text_line(6, "     Wi-FI     ");
    oled_set_text_line(7, "               ");
    oled_render_text();

    return false;
  }

  cyw43_arch_enable_sta_mode();
  uint8_t count = 0;
  while (count < 3 && cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 10000) != 0)
  {
    oled_set_text_line(0, "    ATENCAO    ");
    oled_set_text_line(1, "    FALHA NA   ");
    oled_set_text_line(2, "               ");
    oled_set_text_line(3, " Infraestrutra ");
    oled_set_text_line(4, "    de Rede    ");
    oled_set_text_line(5, "     Wi-FI     ");
    oled_set_text_line(6, " NAO CONECTADO ");
    char strbuffer[max_text_columns + 1];
    sprintf(strbuffer, "Tentativa: %02d", ++count);
    oled_set_text_line(7, strbuffer);
    oled_render_text();
    sleep_ms(INTER_SCREEN_DELAY / (1.0 / 3));
  }

  if (count == 3)
    return false;

  // Read the ip address in a human readable way
  uint8_t *ip_address = (uint8_t *)&(cyw43_state.netif[0].ip_addr.addr);
  oled_set_text_line(0, " INICIALIZANDO ");
  oled_set_text_line(1, "               ");
  oled_set_text_line(2, " Infraestrutra ");
  oled_set_text_line(3, "    de Rede    ");
  oled_set_text_line(4, "     Wi-FI     ");
  oled_set_text_line(5, "   CONECTADO   ");
  oled_set_text_line(6, "               ");
  char strbuffer[max_text_columns+1];
  sprintf(strbuffer, "%03d.%03d.%03d.%03d", ip_address[0], ip_address[1], ip_address[2], ip_address[3]);
  oled_set_text_line(7, strbuffer);
  oled_render_text();

  // Inicia o servidor HTTP
  start_http_server();
  BaseType_t xReturn = xTaskCreate(
      task_http_server,
      "Task HTTP Server",
      TASK_HTTP_SERVER_STACK_SIZE,
      NULL,
      TASK_HTTP_SERVER_PRIORITY,
      NULL);

  if (xReturn != pdPASS)
  {
    oled_set_text_line(0, "     FALHA     ");
    oled_set_text_line(1, " AO CRIAR TASK ");
    oled_set_text_line(2, "      RTOS     ");
    oled_set_text_line(3, "               ");
    oled_set_text_line(4, "               ");
    oled_set_text_line(5, " INFRAESTRUTURA");
    oled_set_text_line(6, "     REDE      ");
    oled_set_text_line(7, "               ");
    oled_render_text();

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
    oled_set_text_line(0, " INICIALIZANDO ");
    oled_set_text_line(1, "               ");
    oled_set_text_line(2, " Infraestrutra ");
    oled_set_text_line(3, "    de Rede    ");
    oled_set_text_line(4, "     Wi-FI     ");
    oled_set_text_line(5, "               ");
    oled_set_text_line(6, " Erro criando  ");
    oled_set_text_line(7, "      PCB      ");
    oled_render_text();
    return;
  }

  // Liga o servidor na porta 80
  if (tcp_bind(pcb, IP_ADDR_ANY, 80) != ERR_OK)
  {
    oled_set_text_line(0, " INICIALIZANDO ");
    oled_set_text_line(1, "               ");
    oled_set_text_line(2, " Infraestrutra ");
    oled_set_text_line(3, "    de Rede    ");
    oled_set_text_line(4, "     Wi-FI     ");
    oled_set_text_line(5, "               ");
    oled_set_text_line(6, " Erro conectar ");
    oled_set_text_line(7, " serv. porta 80");
    oled_render_text();
    return;
  }

  pcb = tcp_listen(pcb);                // Coloca o PCB em modo de escuta
  oled_set_text_line(0, " INICIALIZANDO ");
  oled_set_text_line(1, "               ");
  oled_set_text_line(2, " Infraestrutra ");
  oled_set_text_line(3, "    de Rede    ");
  oled_set_text_line(4, "     Wi-FI     ");
  oled_set_text_line(5, "               ");
  oled_set_text_line(6, " SERRVIDOR WEB ");
  oled_set_text_line(7, "   CONECTADO   ");
  oled_render_text();
  return;
}

// Estado dos botões (inicialmente sem mensagens)
char button_A_message[50] = "Nenhum evento no botão 1";
char button_B_message[50] = "Nenhum evento no botão 2";

// Buffer para resposta HTTP
char http_response[1024];
char json_response[1024 * 8]; // tamanho a ser definido

/**
 * Função para criar a resposta JSON
 */

void create_json_update_response()
{
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
void create_json_noise_response()
{
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
           "  }\r\n"
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
 * Função para processar (callback) as requisições HTTP
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
  }
  else if (strstr(request, "POST /aceleration/xy"))
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
  }
  else if (strstr(request, "POST /aceleration/x"))
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
    
      sprintf(button_A_message, "%03d.%03d.%03d.%03d", ip_address[0], ip_address[1], ip_address[2], ip_address[3]);
      oled_set_text_line(7, button_A_message);
    }
    else
    {
      snprintf(button_A_message, sizeof(button_A_message), "Botão A foi solto!");
      oled_set_text_line(7, "               ");
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
        oled_set_text_line(7, "   modo local  ");
      else
        oled_set_text_line(7, "  modo remoto  ");
    }
    else
    {
      snprintf(button_B_message, sizeof(button_B_message), "Botão B foi solto!");
      oled_set_text_line(7, "               ");
    }
  }
}