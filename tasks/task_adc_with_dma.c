#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"

#include "hardware/vreg.h"
#include "hardware/adc.h"
#include "hardware/dma.h"

#include "inc/neopixel.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "inc/magnectostriccao.h"
#include "tasks_paramiters.h"
#include "task_adc_with_dma.h"

extern QueueHandle_t xFFT_Buffer_Queue;
extern QueueHandle_t xTinyML_Buffer_Queue;
extern QueueHandle_t xIntensity_Buffer_Queue;

// buffer de texto para o display oled
extern char text_line_oled[max_text_lines][max_text_columns];

// buffer de amostragem do ADC, microfone que coleta o audio
// os samples devem ser no comprimento equivalente aos segundos
// necessários para uma amostragem adequada
static uint16_t adc_buffer[ADC_SAMPLES];

static uint dma_channel;
static dma_channel_config dma_cfg;

static void sample_mic();
static float mic_power();
static uint8_t get_intensity(float v);

void task_adc_with_dma(void *pvParameters)
{

  (void)pvParameters;
  printf("Task ADC DMA\n");
  // O código abaixo deve ser transportado para a TASK (FreeRTOS) que
  // faz o processamento da amostragem via DMA

  adc_init();
  adc_gpio_init(MIC_PIN);
  adc_select_input(MIC_CHANNEL);

  // Tomando posse de canal do DMA.
  dma_channel = dma_claim_unused_channel(true);

  // Configurações do DMA.
  dma_cfg = dma_channel_get_default_config(dma_channel);

  channel_config_set_transfer_data_size(&dma_cfg, DMA_SIZE_16); // Tamanho da transferência é 16-bits (usamos uint16_t para armazenar valores do ADC)
  channel_config_set_read_increment(&dma_cfg, false);           // Desabilita incremento do ponteiro de leitura (lemos de um único registrador)
  channel_config_set_write_increment(&dma_cfg, true);           // Habilita incremento do ponteiro de escrita (escrevemos em um array/buffer)

  channel_config_set_dreq(&dma_cfg, DREQ_ADC); // Usamos a requisição de dados do ADC

  dma_channel_configure(
    dma_channel, &dma_cfg,
    adc_buffer,  // Destino: buffer de amostras
    &adc_hw->fifo,  // Fonte: FIFO do ADC
    ADC_SAMPLES,  // Número de transferências
    false  // Não inicia automaticamente
  );

  adc_fifo_setup(
      true,  // Habilitar FIFO
      true,  // Habilitar request de dados do DMA
      1,     // Threshold para ativar request DMA é 1 leitura do ADC
      false, // Não usar bit de erro
      false  // Não fazer downscale das amostras para 8-bits, manter 12-bits.
  );

  adc_set_clkdiv(ADC_CLOCK_DIV);

  TickType_t xLastWakeTime = xTaskGetTickCount();
  while (1)
  {

    printf("Início Loop Task ADC DMA %d\n", xLastWakeTime);
    // Realiza uma amostragem do microfone.
    sample_mic();
    printf("Passou sample mic\n");
    
    if(xQueueSendToBack(xFFT_Buffer_Queue, adc_buffer, ADC_TO_FFT_QUEUE_TIMEOUT) != pdPASS)
    {
      printf("ADC: Erro ao enviar buffer de amostragem\n");
    };

    // realiza a exibição do status da coleta de ruidos
    // Pega a potência média da amostragem do microfone.
    float avg = mic_power();
    avg = 2.f * abs(ADC_ADJUST(avg)); // Ajusta para intervalo de 0 a 3.3V. (apenas magnitude, sem sinal)
    printf("AVG: %f\n", avg);
    float db = 20.f * log((avg+0.00001/ADC_MAX)); // Calcula o volume em decibels.
    printf("AVG: %f Volume: %f dB\n", avg, db);
    uint intensity = get_intensity(avg); // Calcula intensidade a ser mostrada na matriz de LEDs.
    printf("Intensidade: %d\n", intensity);

    if(xQueueSendToBack(xIntensity_Buffer_Queue, &intensity, ADC_INTENSITY_QUEUE_TIMEOUT) != pdPASS)
    {
      printf("ADC: Erro ao enviar buffer de intensidade\n");
    };

    // A depender da intensidade do som, acende LEDs específicos.
    switch (intensity)
    {
    case 0:
      memcpy(text_line_oled[3], "    Sem Som    ", max_text_columns);
      printf("Sem Som\n");
      break; // Se o som for muito baixo, não acende nada.
    case 1:
      memcpy(text_line_oled[3], "   Pouco Som   ", max_text_columns);
      break;
    case 2:
      memcpy(text_line_oled[3], "  Nivel ideal  ", max_text_columns);
      break;
    case 3:
      memcpy(text_line_oled[3], "    Alto Som   ", max_text_columns);
      break;
    case 4:
      memcpy(text_line_oled[3], "Muito Alto Som ", max_text_columns);
      break;
    }
    sprintf(text_line_oled[5], "  %02.2f dB    ", db);
    
    printf("Antes da pausa Loop Task ADC DMA %d\n", xLastWakeTime);
    xTaskDelayUntil(&xLastWakeTime, TASK_ADC_DMA_DELAY );
    printf("FIM LOOP Task ADC DMA %d\n", xLastWakeTime);

    UBaseType_t stackLeft = uxTaskGetStackHighWaterMark(NULL);
    printf("ADC DMA Espaço de pilha livre: %u bytes\n", stackLeft);
  }
}

/**
 * Realiza as leituras do ADC e armazena os valores no buffer.
 */
void sample_mic()
{
  adc_fifo_drain(); // Limpa o FIFO do ADC.

  dma_channel_start(dma_channel);  // Inicia DMA
  adc_run(true);  // Inicia ADC
  //dma_channel_wait_for_finish_blocking(dma_channel); // esta função parece não fazer nada
  vTaskDelay(pdMS_TO_TICKS(((ADC_SAMPLES * 125)/1000)));  // Aguarda a captura (~8kHz)
  adc_run(false);  // Para ADC
}

/**
 * Calcula a potência média das leituras do ADC. (Valor RMS)
 */
float mic_power()
{
  float avg = 0.f;

  for (uint i = 0; i < ADC_SAMPLES; ++i)
    avg += adc_buffer[i] * adc_buffer[i];

  avg /= ADC_SAMPLES;
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
