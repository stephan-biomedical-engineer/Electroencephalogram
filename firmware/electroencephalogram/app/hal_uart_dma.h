/*
 * hal_uart_dma.h
 *
 *  Created on: Sep 3, 2025
 *      Author: stephan
 */

#ifndef HAL_UART_DMA_H_
#define HAL_UART_DMA_H_

#include <stdint.h>
#include <stdbool.h>

/**
 * @brief Inicializa DMA para transmissão via UART8
 */
void usart_dma_init_h7(void);

/**
 * @brief Inicia transmissão por DMA
 * @param buf ponteiro para o buffer
 * @param len tamanho em bytes
 * @return true se iniciado, false se inválido
 */
bool usart_dma_start_tx_h7(const uint8_t *buf, uint16_t len);

/**
 * @brief Para a transmissão DMA
 */
void usart_dma_stop_tx_h7(void);

/**
 * @brief Registra callback chamado quando a transmissão termina
 * @param dev índice do dispositivo (0..2)
 * @param cb função callback
 */
void usart_dma_register_callback_h7(int dev, void (*cb)(int));

/**
 * @brief Função chamada dentro do handler de interrupção
 */
void usart_dma_irq_handler_h7(void);

bool usart_dma_enqueue_tx(const uint8_t *buf, uint16_t len);


#endif /* HAL_UART_DMA_H_ */
