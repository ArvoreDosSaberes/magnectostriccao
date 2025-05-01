
#include "lwip/tcp.h"

#define I2C_SDA  14
#define I2C_SCL  15

#define MIC_CHANNEL 2
#define MIC_PIN (26 + MIC_CHANNEL)

// Parâmetros e macros do ADC.
#define ADC_CLOCK_DIV 96.f
#define SAMPLES 200                                   // Número de amostras que serão feitas do ADC.
#define ADC_ADJUST(x) (x * 3.3f / (1 << 12u) - 1.65f) // Ajuste do valor do ADC para Volts.
#define ADC_MAX 3.3f
#define ADC_STEP (3.3f / 5.f) // Intervalos de volume do microfone.

// Pino e número de LEDs da matriz de LEDs.
#define MATRIZ_LED_PIN 7
#define MATRIZ_LED_COUNT 25

#define LED_PIN_RED 11
#define LED_PIN_GREEN 12   
#define LED_PIN_BLUE 13

#define BUTTON_A_PIN 5      // Pino do botão A
#define BUTTON_B_PIN 6      // Pino do botão B

#define WIFI_SSID "ArvoreDosSaberes"
#define WIFI_PASSWORD "Arduino2022"

#define abs(x) ((x < 0) ? (-x) : (x))

// Buffer para respostas HTTP
#define HTTP_RESPONSE  "<!DOCTYPE html>" \
                   "<html>" \
                   "<head>" \
                   "  <meta charset=\"UTF-8\">" \
                   "  <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">" \
                   "  <title>Controle do LED e Botões</title>" \
                   "  <style>" \
                   "    body {" \
                   "      font-family: Arial, sans-serif;" \
                   "      background-color: #f4f4f9;" \
                   "      color: #333;" \
                   "      margin: 0;" \
                   "      padding: 0;" \
                   "      display: flex;" \
                   "      flex-direction: column;" \
                   "      align-items: center;" \
                   "    }" \
                   "    h1 {" \
                   "      background-color: #4CAF50;" \
                   "      color: white;" \
                   "      padding: 20px;" \
                   "      border-radius: 8px;" \
                   "      margin: 20px 0;" \
                   "      text-align: center;" \
                   "      width: 100%;" \
                   "      max-width: 600px;" \
                   "    }" \
                   "    p {" \
                   "      font-size: 1.2em;" \
                   "      margin: 10px 0;" \
                   "    }" \
                   "    a {" \
                   "      display: inline-block;" \
                   "      text-decoration: none;" \
                   "      background-color: #007BFF;" \
                   "      color: white;" \
                   "      padding: 10px 20px;" \
                   "      border-radius: 5px;" \
                   "      margin: 5px;" \
                   "      font-weight: bold;" \
                   "    }" \
                   "    a:hover {" \
                   "      background-color: #0056b3;" \
                   "    }" \
                   "    .button-state {" \
                   "      border: 1px solid #ddd;" \
                   "      border-radius: 8px;" \
                   "      padding: 15px;" \
                   "      max-width: 600px;" \
                   "      background-color: #fff;" \
                   "      margin: 20px auto;" \
                   "      box-shadow: 0 4px 6px rgba(0, 0, 0, 0.1);" \
                   "    }" \
                   "  </style>" \
                   "</head>" \
                   "<body>" \
                   "  <h1>Controle do LED e Botões</h1>" \
                   "  <div>" \
                   "    <p><a href=\"/led/on\">Ligar LED</a></p>" \
                   "    <p><a href=\"/led/off\">Desligar LED</a></p>" \
                   "    <p><a href=\"/update\">Atualizar Estado</a></p>" \
                   "  </div>" \
                   "  <div class=\"button-state\">" \
                   "    <h2>Estado dos Botões:</h2>" \
                   "    <p>Botão A: %s</p>" \
                   "    <p>Botão B: %s</p>" \
                   "  </div>" \
                   "</body>" \
                   "</html>"


void sample_mic();
float mic_power();
uint8_t get_intensity(float v);

static void start_http_server(void);
void create_http_response();
static err_t http_callback(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err);
static err_t connection_callback(void *arg, struct tcp_pcb *newpcb, err_t err);
static void monitor_buttons_callback(unsigned int gpio, long unsigned int events);