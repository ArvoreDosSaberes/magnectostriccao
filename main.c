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
#include "inc/neopixel.c"

#include "main.h"

// Canal e configurações do DMA
static uint dma_channel;
static dma_channel_config dma_cfg;

// Buffer de amostras do ADC.
static uint16_t adc_buffer[SAMPLES];

static char text[max_text_lines][max_text_columns] = {0}; 

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

int main()
{
  stdio_init_all(); // Inicializa os tipos stdio padrão presentes ligados ao binário

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

  strcpy(text[0], "    Projeto    ");
  strcpy(text[1], "     Final     ");
  strcpy(text[2], "               ");
  strcpy(text[3], "  EmbarcaTech  ");
  strcpy(text[4], "               ");
  strcpy(text[5], "   Residencia  ");
  strcpy(text[6], "     TIC 37    ");
  strcpy(text[7], "               ");

  int y = 0;
  for (uint i = 0; i < count_of(text); i++)
  {
    ssd1306_draw_string(ssd, 5, y, text[i]);
    y += ssd1306_line_height;
  }
  render_on_display(ssd, &frame_area);

  sleep_ms(3500);

  strcpy(text[0], "  Analizador   ");
  strcpy(text[1], "  da Saude de  ");
  strcpy(text[2], "               ");
  strcpy(text[3], "Transformadores");
  strcpy(text[4], "      por      ");
  strcpy(text[5], "               ");
  strcpy(text[6], "Magnetostriccao");
  strcpy(text[7], "               ");

  y = 0;
  for (uint i = 0; i < count_of(text); i++)
  {
    ssd1306_draw_string(ssd, 5, y, text[i]);
    y += ssd1306_line_height;
  }
  render_on_display(ssd, &frame_area);

  sleep_ms(1500);

  strcpy(text[0], "  da Saude de  ");
  strcpy(text[1], "               ");
  strcpy(text[2], "Transformadores");
  strcpy(text[3], "      por      ");
  strcpy(text[4], "               ");
  strcpy(text[5], "Magnetostriccao");
  strcpy(text[6], "               ");
  strcpy(text[7], "               ");

  y = 0;
  for (uint i = 0; i < count_of(text); i++)
  {
    ssd1306_draw_string(ssd, 5, y, text[i]);
    y += ssd1306_line_height;
  }
  render_on_display(ssd, &frame_area);

  sleep_ms(400);

  strcpy(text[0], "               ");
  strcpy(text[1], "Transformadores");
  strcpy(text[2], "      por      ");
  strcpy(text[3], "               ");
  strcpy(text[4], "Magnetostriccao");
  strcpy(text[5], "               ");
  strcpy(text[6], "               ");
  strcpy(text[7], "               ");

  y = 0;
  for (uint i = 0; i < count_of(text); i++)
  {
    ssd1306_draw_string(ssd, 5, y, text[i]);
    y += ssd1306_line_height;
  }
  render_on_display(ssd, &frame_area);

  sleep_ms(400);

  strcpy(text[0], "Transformadores");
  strcpy(text[1], "      por      ");
  strcpy(text[2], "               ");
  strcpy(text[3], "Magnetostriccao");
  strcpy(text[4], "               ");
  strcpy(text[5], "               ");
  strcpy(text[6], "               ");
  strcpy(text[7], "               ");

  y = 0;
  for (uint i = 0; i < count_of(text); i++)
  {
    ssd1306_draw_string(ssd, 5, y, text[i]);
    y += ssd1306_line_height;
  }
  render_on_display(ssd, &frame_area);

  sleep_ms(400);

  strcpy(text[0], "      por      ");
  strcpy(text[1], "               ");
  strcpy(text[2], "Magnetostriccao");
  strcpy(text[3], "               ");
  strcpy(text[4], "               ");
  strcpy(text[5], "               ");
  strcpy(text[6], "               ");
  strcpy(text[7], "               ");

  y = 0;
  for (uint i = 0; i < count_of(text); i++)
  {
    ssd1306_draw_string(ssd, 5, y, text[i]);
    y += ssd1306_line_height;
  }
  render_on_display(ssd, &frame_area);

  sleep_ms(400);

  strcpy(text[0], "               ");
  strcpy(text[1], "Magnetostriccao");
  strcpy(text[2], "               ");
  strcpy(text[3], "               ");
  strcpy(text[4], "               ");
  strcpy(text[5], "               ");
  strcpy(text[6], "               ");
  strcpy(text[7], "               ");

  y = 0;
  for (uint i = 0; i < count_of(text); i++)
  {
    ssd1306_draw_string(ssd, 5, y, text[i]);
    y += ssd1306_line_height;
  }
  render_on_display(ssd, &frame_area);

  sleep_ms(400);

  strcpy(text[0], "Magnetostriccao");
  strcpy(text[1], "               ");
  strcpy(text[2], "               ");
  strcpy(text[3], "               ");
  strcpy(text[4], "               ");
  strcpy(text[5], "               ");
  strcpy(text[6], "               ");
  strcpy(text[7], "               ");

  y = 0;
  for (uint i = 0; i < count_of(text); i++)
  {
    ssd1306_draw_string(ssd, 5, y, text[i]);
    y += ssd1306_line_height;
  }
  render_on_display(ssd, &frame_area);

  sleep_ms(400);

  strcpy(text[0], " Inicializando ");
  strcpy(text[1], "               ");
  strcpy(text[2], "               ");
  strcpy(text[3], "      GPIO     ");
  strcpy(text[4], "               ");
  strcpy(text[5], "               ");
  strcpy(text[6], "               ");
  strcpy(text[7], "               ");

  y = 0;
  for (uint i = 0; i < count_of(text); i++)
  {
    ssd1306_draw_string(ssd, 5, y, text[i]);
    y += ssd1306_line_height;
  }
  render_on_display(ssd, &frame_area);

  
  sleep_ms(400);

  if (cyw43_arch_init_with_country(CYW43_COUNTRY_BRAZIL))
  {
    strcpy(text[0], "    ATENCAO    ");
    strcpy(text[1], "    FALHA NA   ");
    strcpy(text[2], "               ");
    strcpy(text[3], " Infraestrutra ");
    strcpy(text[4], "    de Rede    ");
    strcpy(text[5], "               ");
    strcpy(text[6], "     Wi-FI     ");
    strcpy(text[7], "               ");

    y = 0;
    for (uint i = 0; i < count_of(text); i++)
    {
      ssd1306_draw_string(ssd, 5, y, text[i]);
      y += ssd1306_line_height;
    }
    render_on_display(ssd, &frame_area);
    return 1;
  }

  cyw43_arch_enable_sta_mode();
  if (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 10000) != 0)
  {
    strcpy(text[0], "    ATENCAO    ");
    strcpy(text[1], "    FALHA NA   ");
    strcpy(text[2], "               ");
    strcpy(text[3], " Infraestrutra ");
    strcpy(text[4], "    de Rede    ");
    strcpy(text[5], "     Wi-FI     ");
    strcpy(text[6], " NAO CONECTADO ");
    strcpy(text[7], "               ");
    y = 0;
    for (uint i = 0; i < count_of(text); i++)
    {
      ssd1306_draw_string(ssd, 5, y, text[i]);
      y += ssd1306_line_height;
    }
    render_on_display(ssd, &frame_area);
    return 1;
  }
  else
  {
    // Read the ip address in a human readable way
    uint8_t *ip_address = (uint8_t *)&(cyw43_state.netif[0].ip_addr.addr);
    strcpy(text[0], " INICIALIZANDO ");
    strcpy(text[1], "               ");
    strcpy(text[2], "               ");
    strcpy(text[3], " Infraestrutra ");
    strcpy(text[4], "    de Rede    ");
    strcpy(text[5], "     Wi-FI     ");
    strcpy(text[6], "               ");
    sprintf(text[7], "%03d.%03d.%03d.%03d", ip_address[0], ip_address[1], ip_address[2], ip_address[3]);
    y = 0;
    for (uint i = 0; i < count_of(text); i++)
    {
      ssd1306_draw_string(ssd, 5, y, text[i]);
      y += ssd1306_line_height;
    }
    render_on_display(ssd, &frame_area);
    sleep_ms(1000);

    // Inicia o servidor HTTP
    start_http_server();
  }

  sleep_ms(1000);

  strcpy(text[0], "   Ativando    ");
  strcpy(text[1], "               ");
  strcpy(text[2], "               ");
  strcpy(text[3], " configuracoes ");
  strcpy(text[4], "               ");
  strcpy(text[5], "  dos botoes   ");
  strcpy(text[6], "               ");
  strcpy(text[7], "               ");

  // ssd1306_write_array(ssd, &frame_area, &text);
  y = 0;
  for (uint i = 0; i < count_of(text); i++)
  {
    ssd1306_draw_string(ssd, 5, y, text[i]);
    y += ssd1306_line_height;
  }
  render_on_display(ssd, &frame_area);

  gpio_init(BUTTON_A_PIN);
  gpio_set_dir(BUTTON_A_PIN, GPIO_IN);
  gpio_pull_up(BUTTON_A_PIN);
  gpio_set_irq_enabled_with_callback(BUTTON_A_PIN, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, true, &monitor_buttons_callback);

  strcpy(text[7], "   Botao A   ");

  // ssd1306_write_array(ssd, &frame_area, &text);
  y = 0;
  for (uint i = 0; i < count_of(text); i++)
  {
    ssd1306_draw_string(ssd, 5, y, text[i]);
    y += ssd1306_line_height;
  }
  render_on_display(ssd, &frame_area);

  sleep_ms(500);
  gpio_init(BUTTON_B_PIN);
  gpio_set_dir(BUTTON_B_PIN, GPIO_IN);
  gpio_pull_up(BUTTON_B_PIN);
  gpio_set_irq_enabled_with_callback(BUTTON_B_PIN, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, true, &monitor_buttons_callback);

  strcpy(text[7], "   Botao B   ");

  // ssd1306_write_array(ssd, &frame_area, &text);
  y = 0;
  for (uint i = 0; i < count_of(text); i++)
  {
    ssd1306_draw_string(ssd, 5, y, text[i]);
    y += ssd1306_line_height;
  }
  render_on_display(ssd, &frame_area);

  strcpy(text[0], "   Ativando    ");
  strcpy(text[1], "               ");
  strcpy(text[2], "               ");
  strcpy(text[3], "  VU de LEDs   ");
  strcpy(text[4], "               ");
  strcpy(text[5], "               ");
  strcpy(text[6], "               ");
  strcpy(text[7], "               ");

  // ssd1306_write_array(ssd, &frame_area, &text);
  y = 0;
  for (uint i = 0; i < count_of(text); i++)
  {
    ssd1306_draw_string(ssd, 5, y, text[i]);
    y += ssd1306_line_height;
  }
  render_on_display(ssd, &frame_area);

  npInit(MATRIZ_LED_PIN, MATRIZ_LED_COUNT);

  sleep_ms(1000);

  strcpy(text[0], "   Ativando    ");
  strcpy(text[1], "               ");
  strcpy(text[2], "      ADC      ");
  strcpy(text[3], "    Por DMA    ");
  strcpy(text[4], "               ");
  strcpy(text[5], "               ");
  strcpy(text[6], "               ");
  strcpy(text[7], "               ");

  // ssd1306_write_array(ssd, &frame_area, &text);
  y = 0;
  for (uint i = 0; i < count_of(text); i++)
  {
    ssd1306_draw_string(ssd, 5, y, text[i]);
    y += ssd1306_line_height;
  }
  render_on_display(ssd, &frame_area);

  // O código abaixo deve ser transportado para a TASK (FreeRTOS) que
  // faz o processamento da amostragem via DMA
  adc_gpio_init(MIC_PIN);
  adc_init();
  adc_select_input(MIC_CHANNEL);

  adc_fifo_setup(
      true,  // Habilitar FIFO
      true,  // Habilitar request de dados do DMA
      1,     // Threshold para ativar request DMA é 1 leitura do ADC
      false, // Não usar bit de erro
      false  // Não fazer downscale das amostras para 8-bits, manter 12-bits.
  );

  adc_set_clkdiv(ADC_CLOCK_DIV);
  // Tomando posse de canal do DMA.
  dma_channel = dma_claim_unused_channel(true);

  // Configurações do DMA.
  dma_cfg = dma_channel_get_default_config(dma_channel);

  channel_config_set_transfer_data_size(&dma_cfg, DMA_SIZE_16); // Tamanho da transferência é 16-bits (usamos uint16_t para armazenar valores do ADC)
  channel_config_set_read_increment(&dma_cfg, false);           // Desabilita incremento do ponteiro de leitura (lemos de um único registrador)
  channel_config_set_write_increment(&dma_cfg, true);           // Habilita incremento do ponteiro de escrita (escrevemos em um array/buffer)

  channel_config_set_dreq(&dma_cfg, DREQ_ADC); // Usamos a requisição de dados do ADC

  sample_mic();

  sleep_ms(500);

  strcpy(text[0], "   Ativando    ");
  strcpy(text[1], "               ");
  strcpy(text[2], "               ");
  strcpy(text[3], "   Controle    ");
  strcpy(text[4], "               ");
  strcpy(text[5], "   Do DRONE    ");
  strcpy(text[6], "  TERRESTRE    ");
  strcpy(text[7], "               ");

  // ssd1306_write_array(ssd, &frame_area, &text);
  y = 0;
  for (uint i = 0; i < count_of(text); i++)
  {
    ssd1306_draw_string(ssd, 5, y, text[i]);
    y += ssd1306_line_height;
  }
  render_on_display(ssd, &frame_area);

  // Aqui deve ser inicializada a task de controle do drone
  // #########################################
  // #########################################
  // #########################################
  // #########################################
  // #########################################

  sleep_ms(1000);
  
  strcpy(text[0], "   Ativando    ");
  strcpy(text[1], "               ");
  strcpy(text[2], "               ");
  strcpy(text[3], "    TinyML     ");
  strcpy(text[4], "               ");
  strcpy(text[5], " Redes Neurais ");
  strcpy(text[6], "               ");
  strcpy(text[7], "               ");

  // ssd1306_write_array(ssd, &frame_area, &text);
  y = 0;
  for (uint i = 0; i < count_of(text); i++)
  {
    ssd1306_draw_string(ssd, 5, y, text[i]);
    y += ssd1306_line_height;
  }
  render_on_display(ssd, &frame_area);

  // Aqui deve ser inicializado a TASKTinyML.
  // #########################################
  // #########################################
  // #########################################
  // #########################################
  // #########################################

  sleep_ms(1000);

  strcpy(text[0], "               ");
  strcpy(text[1], " Nivel do Sinal");
  strcpy(text[2], "               ");
  strcpy(text[4], "               ");
  strcpy(text[6], "               ");
  strcpy(text[7], "               ");

  // aqui deve ser iniciado o escalonador do freertos, o código asseguir será levado para as espectivas tasks
  
  // o código abaixo deve reestruturado para sepearar a infra de rede do processador do VU
  // 
  int32_t last_time = time_us_32();
  while (true)
  {
    // infra de rede
    cyw43_arch_poll(); // Necessário para manter o Wi-Fi ativo

    // Realiza uma amostragem do microfone. deve ser transportado para a TASK de amostragem ADC
    sample_mic();

    // realiza a exibição do status da coleta de ruidos
    // Pega a potência média da amostragem do microfone.
    float avg = mic_power();
    avg = 2.f * abs(ADC_ADJUST(avg)); // Ajusta para intervalo de 0 a 3.3V. (apenas magnitude, sem sinal)
    float dbv = 20.f * log(ADC_MAX / avg); // Converte para dBV.

    uint intensity = get_intensity(avg); // Calcula intensidade a ser mostrada na matriz de LEDs.

    // Limpa a matriz de LEDs.
    npClear();

    // A depender da intensidade do som, acende LEDs específicos.
    switch (intensity)
    {
    case 0:
      strcpy(text[3], "   Sem Som    ");
      break; // Se o som for muito baixo, não acende nada.
    case 1:
      strcpy(text[3], "   Pouco Som  ");
      npSetLED(12, 0, 0, 80); // Acende apenas o centro.
      break;
    case 2:
      strcpy(text[3], " Nivel ideal ");
      npSetLED(12, 0, 0, 120); // Acente o centro.

      // Primeiro anel.
      npSetLED(7, 0, 0, 80);
      npSetLED(11, 0, 0, 80);
      npSetLED(13, 0, 0, 80);
      npSetLED(17, 0, 0, 80);
      break;
    case 3:
      strcpy(text[3], "   Alto Som   ");
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
      strcpy(text[3], "Muito Alto Som");
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

    sprintf(text[5], "  %03.02f dBV   ", dbv);

    if (time_us_32() - last_time > 10000)
    {
      last_time = time_us_32();

      y = 0;
      for (uint i = 0; i < count_of(text); i++)
      {
        ssd1306_draw_string(ssd, 5, y, text[i]);
        y += ssd1306_line_height;
      }
      render_on_display(ssd, &frame_area);
    }
  }

  return 0;
}

/**
 * Realiza as leituras do ADC e armazena os valores no buffer.
 */
void sample_mic()
{
  adc_fifo_drain(); // Limpa o FIFO do ADC.
  adc_run(false);   // Desliga o ADC (se estiver ligado) para configurar o DMA.

  dma_channel_configure(dma_channel, &dma_cfg,
                        adc_buffer,      // Escreve no buffer.
                        &(adc_hw->fifo), // Lê do ADC.
                        SAMPLES,         // Faz SAMPLES amostras.
                        true             // Liga o DMA.
  );

  // Liga o ADC e espera acabar a leitura.
  adc_run(true);
  dma_channel_wait_for_finish_blocking(dma_channel);

  // Acabou a leitura, desliga o ADC de novo.
  adc_run(false);
}

/**
 * Calcula a potência média das leituras do ADC. (Valor RMS)
 */
float mic_power()
{
  float avg = 0.f;

  for (uint i = 0; i < SAMPLES; ++i)
    avg += adc_buffer[i] * adc_buffer[i];

  avg /= SAMPLES;
  return sqrt(avg);
}

/**
 * Calcula a intensidade do volume registrado no microfone, de 0 a 4, usando a tensão.
 */
uint8_t get_intensity(float v)
{
  uint count = 0;

  while ((v -= ADC_STEP / 20) > 0.f)
    ++count;

  return count;
}

// #########################################
//                WIFI e WEB Server

// Função de setup do servidor TCP
static void start_http_server(void)
{
  struct tcp_pcb *pcb = tcp_new();
  if (!pcb)
  {
    strcpy(text[0], " INICIALIZANDO ");
    strcpy(text[1], "               ");
    strcpy(text[2], " Infraestrutra ");
    strcpy(text[3], "    de Rede    ");
    strcpy(text[4], "     Wi-FI     ");
    strcpy(text[5], "               ");
    strcpy(text[6], " Erro criando  ");
    strcpy(text[7], "      PCB      ");

    uint8_t y = 0;
    for (uint i = 0; i < count_of(text); i++)
    {
      ssd1306_draw_string(ssd, 5, y, text[i]);
      y += ssd1306_line_height;
    }
    render_on_display(ssd, &frame_area);

    return;
  }

  // Liga o servidor na porta 80
  if (tcp_bind(pcb, IP_ADDR_ANY, 80) != ERR_OK)
  {
    strcpy(text[0], " INICIALIZANDO ");
    strcpy(text[1], "               ");
    strcpy(text[2], " Infraestrutra ");
    strcpy(text[3], "    de Rede    ");
    strcpy(text[4], "     Wi-FI     ");
    strcpy(text[5], "               ");
    strcpy(text[6], " Erro conectar ");
    strcpy(text[7], "    porta 80   ");

    uint8_t y = 0;
    for (uint i = 0; i < count_of(text); i++)
    {
      ssd1306_draw_string(ssd, 5, y, text[i]);
      y += ssd1306_line_height;
    }
    render_on_display(ssd, &frame_area);
    return;
  }

  pcb = tcp_listen(pcb);                // Coloca o PCB em modo de escuta
  tcp_accept(pcb, connection_callback); // Associa o callback de conexão

}

// Estado dos botões (inicialmente sem mensagens)
char button_A_message[50] = "Nenhum evento no botão 1";
char button_B_message[50] = "Nenhum evento no botão 2";

// Buffer para resposta HTTP
char http_response[1024];
char json_response[1024 * 8]; // buffer para envio da respsota em json com array de amostragem

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
// Função para criar a resposta HTTP
void create_http_response()
{
  snprintf(http_response, sizeof(http_response), HTTP_RESPONSE,
           button_A_message, button_B_message);
}

// Função de callback para processar requisições HTTP
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
<<<<<<< HEAD
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
    tcp_write(tpcb, http_response, strlen(http_response), TCP_WRITE_FLAG_COPY);
  }
  else if (strstr(request, "GET /noise"))
  {
    // Envia a resposta HTTP
    // obtem o array de amostras de ruidos coletados

    create_json_noise_response();
    tcp_write(tpcb, json_response, strlen(json_response), TCP_WRITE_FLAG_COPY);
  }

=======
    gpio_put(LED_PIN_BLUE, 1); // Liga o LED
    // Envia a resposta HTTP
    create_http_response();
    tcp_write(tpcb, http_response, strlen(http_response), TCP_WRITE_FLAG_COPY);
  }
  else if (strstr(request, "GET /led/off"))
  {
    gpio_put(LED_PIN_BLUE, 0); // Desliga o LED
    // Envia a resposta HTTP
    create_http_response();
    tcp_write(tpcb, http_response, strlen(http_response), TCP_WRITE_FLAG_COPY);
  }
  else if (strstr(request, "GET /update"))
  {
    // Envia a resposta HTTP
    create_http_response();
    tcp_write(tpcb, http_response, strlen(http_response), TCP_WRITE_FLAG_COPY);
  }

https://github.com/BitDogLab/BitDogLab-C/blob/main/wifi_button_and_led/pico_w_wifi_complete_example.c
>>>>>>> 77f627f (ajustes nas mensagens de)
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
      sprintf(text[7], "%03d.%03d.%03d.%03d", ip_address[0], ip_address[1], ip_address[2], ip_address[3]);
    }
    else
    {
      snprintf(button_A_message, sizeof(button_A_message), "Botão A foi solto!");
      strcpy(text[7], "               ");
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
        strcpy(text[7], "  modo local  ");
      else
<<<<<<< HEAD
       strcpy(text[7],  " modo remoto  ");
      
=======
        strcpy(text[7], "   modo remoto ");
>>>>>>> 77f627f (ajustes nas mensagens de)
    }
    else
    {
      snprintf(button_B_message, sizeof(button_B_message), "Botão B foi solto!");
    }
  }
}