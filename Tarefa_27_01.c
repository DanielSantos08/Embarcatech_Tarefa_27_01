#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/pio.h"
#include "hardware/timer.h"
#include "matriz_leds.h"

// Definições de GPIOs
#define LED_GREEN 11
#define LED_BLUE 12
#define LED_RED 13
#define BOTAO_A 05
#define BOTAO_B 06

// Variável global para armazenar o número atual exibido
volatile int numero_atual = 0;
volatile bool atualizar_matriz = false;  // Flag para atualização da matriz

// Controle do LED vermelho piscando
bool toggle_led(struct repeating_timer *t) {
    gpio_put(LED_RED, !gpio_get(LED_RED));
    return true;
}

// Função de debounce e interrupção para verificar qual botão foi pressionado
void botao_irq(uint gpio, uint32_t events) {
    static absolute_time_t ultima_pressao = {0};
    absolute_time_t agora = get_absolute_time();

    if (absolute_time_diff_us(ultima_pressao, agora) > 200000) { 
        if (gpio == BOTAO_A) {
            printf("Botao A pressionado, numero_atual: %d\n", ((numero_atual+1)%10));  
            numero_atual = (numero_atual + 1) % 10; 
        } else if (gpio == BOTAO_B) {
            printf("Botao B pressionado, numero_atual: %d\n", ((numero_atual-1)%10));  
            numero_atual = (numero_atual - 1 + 10) % 10; 
        }
        atualizar_matriz = true;  
        ultima_pressao = agora;
    }
}

int main() {
    stdio_init_all();

    // Configurar GPIOs dos LEDs
    gpio_init(LED_RED);
    gpio_set_dir(LED_RED, GPIO_OUT);
    gpio_put(LED_RED, 0);

    gpio_init(LED_GREEN);
    gpio_set_dir(LED_GREEN, GPIO_OUT);
    gpio_put(LED_GREEN, 0);

    gpio_init(LED_BLUE);
    gpio_set_dir(LED_BLUE, GPIO_OUT);
    gpio_put(LED_BLUE, 0);

    // Configura GPIOs dos botões com resistores internos de pull-up
    gpio_init(BOTAO_A);
    gpio_set_dir(BOTAO_A, GPIO_IN);
    gpio_pull_up(BOTAO_A);

    gpio_init(BOTAO_B);
    gpio_set_dir(BOTAO_B, GPIO_IN);
    gpio_pull_up(BOTAO_B);

    // Configura uma única interrupção para ambos os botões
    gpio_set_irq_enabled_with_callback(BOTAO_A, GPIO_IRQ_EDGE_RISE, true, &botao_irq);
    gpio_set_irq_enabled_with_callback(BOTAO_B, GPIO_IRQ_EDGE_RISE, true, &botao_irq);

    // Configura a matriz de LEDs WS2812
    PIO pio = pio0;
    uint sm = configurar_matriz(pio);

    // obtem a matriz de LEDs para o número inicial
    Matriz_leds_config matriz;
    obter_matriz_numero(matriz, numero_atual);

    // exibi número inicial na matriz
    imprimir_desenho(matriz, pio, sm);

    // Configurar timer para piscar o LED vermelho a cada 200ms 
    struct repeating_timer timer;
    add_repeating_timer_ms(-200, toggle_led, NULL, &timer);

    while (1) {
        if (atualizar_matriz) {
            // Atualiza a matriz com o novo número
            obter_matriz_numero(matriz, numero_atual);
            imprimir_desenho(matriz, pio, sm);
            atualizar_matriz = false;  // Reseta flag após atualização
        }
        sleep_ms(10);  // Pequeno delay para otimizar processamento
    }

    return 0;
}
