#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "matriz_leds.h"

// Arquivo .pio para controle da matriz
#include "pio_matrix.pio.h"

#define OUT_PIN 7

// Gera o binário que controla a cor de cada célula do LED
uint32_t gerar_binario_cor(double red, double green, double blue) {
    unsigned char RED, GREEN, BLUE;
    RED = red * 255.0;
    GREEN = green * 255.0;
    BLUE = blue * 255.0;
    return (GREEN << 24) | (RED << 16) | (BLUE << 8);
}

// Definição dos números 0-9 em um formato digital
const int NUMEROS[10][5][5] = {
    { // Número 0
        {1, 1, 1, 1, 1},
        {1, 0, 0, 0, 1},
        {1, 0, 0, 0, 1},
        {1, 0, 0, 0, 1},
        {1, 1, 1, 1, 1}
    },
    { // Número 1
        {0, 0, 1, 0, 0},
        {0, 1, 1, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 0, 1, 0, 0},
        {0, 1, 1, 1, 0}
    },
    { // Número 2
        {1, 1, 1, 1, 1},
        {0, 0, 0, 0, 1},
        {1, 1, 1, 1, 1},
        {1, 0, 0, 0, 0},
        {1, 1, 1, 1, 1}
    },
    { // Número 3
        {1, 1, 1, 1, 1},
        {0, 0, 0, 0, 1},
        {0, 1, 1, 1, 1},
        {0, 0, 0, 0, 1},
        {1, 1, 1, 1, 1}
    },
    { // Número 4
        {1, 0, 0, 1, 0},
        {1, 0, 0, 1, 0},
        {1, 1, 1, 1, 1},
        {0, 0, 0, 1, 0},
        {0, 0, 0, 1, 0}
    },
    { // Número 5
        {1, 1, 1, 1, 1},
        {1, 0, 0, 0, 0},
        {1, 1, 1, 1, 1},
        {0, 0, 0, 0, 1},
        {1, 1, 1, 1, 1}
    },
    { // Número 6
        {1, 1, 1, 1, 1},
        {1, 0, 0, 0, 0},
        {1, 1, 1, 1, 1},
        {1, 0, 0, 0, 1},
        {1, 1, 1, 1, 1}
    },
    { // Número 7
        {1, 1, 1, 1, 1},
        {0, 0, 0, 0, 1},
        {0, 0, 0, 1, 0},
        {0, 0, 1, 0, 0},
        {0, 1, 0, 0, 0}
    },
    { // Número 8
        {1, 1, 1, 1, 1},
        {1, 0, 0, 0, 1},
        {1, 1, 1, 1, 1},
        {1, 0, 0, 0, 1},
        {1, 1, 1, 1, 1}
    },
    { // Número 9
        {1, 1, 1, 1, 1},
        {1, 0, 0, 0, 1},
        {1, 1, 1, 1, 1},
        {0, 0, 0, 0, 1},
        {1, 1, 1, 1, 1}
    }
};

// Função para obter a matriz do número correspondente
void obter_matriz_numero(Matriz_leds_config matriz, int numero) {
    for (int linha = 0; linha < 5; linha++) {
        for (int coluna = 0; coluna < 5; coluna++) {
            if (NUMEROS[numero][linha][coluna]) {
                matriz[linha][coluna] = (RGB_cod){1.0, 1.0, 1.0}; // Branco
            } else {
                matriz[linha][coluna] = (RGB_cod){0.0, 0.0, 0.0}; // Preto (LED apagado)
            }
        }
    }
}

// Função para configurar a matriz de LEDs
uint configurar_matriz(PIO pio) {
    bool ok = set_sys_clock_khz(128000, false);
    stdio_init_all();

    printf("Iniciando a transmissão PIO");
    if (ok) printf("clock set to %ld\n", clock_get_hz(clk_sys));

    uint offset = pio_add_program(pio, &pio_matrix_program);
    uint sm = pio_claim_unused_sm(pio, true);
    pio_matrix_program_init(pio, sm, offset, OUT_PIN);

    return sm;
}

// Função para exibir um número na matriz de LEDs
void imprimir_desenho(Matriz_leds_config matriz, PIO pio, uint sm) {
    for (int linha = 4; linha >= 0; linha--) {
        if (linha % 2) {
            for (int coluna = 0; coluna < 5; coluna++) {
                uint32_t cor_binaria = gerar_binario_cor(
                    matriz[linha][coluna].red,
                    matriz[linha][coluna].green,
                    matriz[linha][coluna].blue
                );
                pio_sm_put_blocking(pio, sm, cor_binaria);
            }
        } else {
            for (int coluna = 4; coluna >= 0; coluna--) {
                uint32_t cor_binaria = gerar_binario_cor(
                    matriz[linha][coluna].red,
                    matriz[linha][coluna].green,
                    matriz[linha][coluna].blue
                );
                pio_sm_put_blocking(pio, sm, cor_binaria);
            }
        }
    }
}