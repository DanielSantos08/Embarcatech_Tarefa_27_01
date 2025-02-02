// Definição de tipo da estrutura que irá controlar a cor dos LED's
typedef struct {
    double red;
    double green;
    double blue;
}Led_config;

typedef Led_config RGB_cod;

// Definição de tipo da matriz de leds
typedef Led_config Matriz_leds_config[5][5]; 

uint32_t gerar_binario_cor(double red, double green, double blue);

extern const int NUMEROS[10][5][5];

void obter_matriz_numero(Matriz_leds_config configuracao, int numero);

uint configurar_matriz(PIO pio);

void imprimir_desenho(Matriz_leds_config configuracao, PIO pio, uint sm);

