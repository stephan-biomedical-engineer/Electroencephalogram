#include "app.h"
#include "led.h"
#include "serial.h"
#include <stdio.h>
#include <stdlib.h>

void app_init(void)
{
    serial_init();
    led_init();
    
    // Define frequência inicial como 1Hz
    led_set_frequency(1.0f);
    
    printf("\n\n--- Pisca-pisca Controlado por Timer ---\n");
    printf("Instruções:\n");
    printf("1. Digite a frequência desejada em Hz\n");
    printf("2. Pressione ENTER para confirmar\n");
    printf("Exemplo: 1.5\n\n");
    printf("Digite a frequência inicial: ");
}

void app_loop()
{
    while (1) {
        char* input = serial_read_line();
        float freq = atof(input);
        if (freq > 0) {
            led_set_frequency(freq);
            printf("\nFrequência alterada para: %.2f Hz\n", freq);
        } else {
            printf("\nErro: Frequência deve ser maior que 0\n");
        }
        printf("\nDigite nova frequência: ");
    }
}