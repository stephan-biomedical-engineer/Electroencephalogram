/*
 * hal_uart_dma.c
 *
 *  Created on: Sep 3, 2025
 *      Author: stephan
 */

#include "stm32h7xx.h"
#include "stm32h7xx_ll_bus.h"
#include "stm32h7xx_ll_dma.h"
#include "stm32h7xx_ll_usart.h"
#include "stm32h7xx_ll_dmamux.h"
#include "hal_uart_dma.h"
#include "string.h"
#include <stdbool.h>
#include <stdint.h>

static void (*dma_completion_cb[3])(int dev) = {0};

// ---- Mapeamento real da UART8 TX ----
#define TX_USART                UART8
#define TX_DMA                  DMA1
#define TX_DMA_STREAM           LL_DMA_STREAM_1
#define TX_DMAMUX_REQ           LL_DMAMUX1_REQ_UART8_TX
#define TX_DMA_IRQn             DMA1_Stream1_IRQn


#define UART_TX_QUEUE_LEN 8   // até 8 pacotes enfileirados
#define UART_TX_BUF_SIZE 256  // tamanho máximo de cada pacote

typedef struct {
    uint8_t data[UART_TX_BUF_SIZE];
    uint16_t len;
} uart_tx_item_t;

static uart_tx_item_t tx_queue[UART_TX_QUEUE_LEN];
static volatile int tx_head = 0;
static volatile int tx_tail = 0;
static volatile bool tx_busy = false;

// -------------------------------------

static inline uint32_t usart_tdr_addr(USART_TypeDef *u) {
    return LL_USART_DMA_GetRegAddr(u, LL_USART_DMA_REG_DATA_TRANSMIT);
}

void usart_dma_register_callback_h7(int dev, void (*cb)(int)){
    if (dev >=0 && dev < 3) dma_completion_cb[dev] = cb;
}

void usart_dma_init_h7(void)
{
    // Clocks do DMA + DMAMUX
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA1);
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA1);
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA2);

    // Garante stream OFF
    LL_DMA_DisableStream(TX_DMA, TX_DMA_STREAM);

    // Seleciona request via DMAMUX
    LL_DMA_SetPeriphRequest(TX_DMA, TX_DMA_STREAM, TX_DMAMUX_REQ);

    // Direção e modos
    LL_DMA_SetDataTransferDirection(TX_DMA, TX_DMA_STREAM, LL_DMA_DIRECTION_MEMORY_TO_PERIPH);
    LL_DMA_SetStreamPriorityLevel(TX_DMA, TX_DMA_STREAM, LL_DMA_PRIORITY_HIGH);
    LL_DMA_SetMode(TX_DMA, TX_DMA_STREAM, LL_DMA_MODE_NORMAL);

    LL_DMA_SetPeriphIncMode(TX_DMA, TX_DMA_STREAM, LL_DMA_PERIPH_NOINCREMENT);
    LL_DMA_SetMemoryIncMode(TX_DMA, TX_DMA_STREAM, LL_DMA_MEMORY_INCREMENT);

    LL_DMA_SetPeriphSize(TX_DMA, TX_DMA_STREAM, LL_DMA_PDATAALIGN_BYTE);
    LL_DMA_SetMemorySize(TX_DMA, TX_DMA_STREAM, LL_DMA_MDATAALIGN_BYTE);

    // ITs de TX complete + error
    LL_DMA_EnableIT_TC(TX_DMA, TX_DMA_STREAM);
    LL_DMA_EnableIT_TE(TX_DMA, TX_DMA_STREAM);

    NVIC_SetPriority(TX_DMA_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 5, 0));
    NVIC_EnableIRQ(TX_DMA_IRQn);
}

bool usart_dma_start_tx_h7(const uint8_t *buf, uint16_t len)
{
    if (!buf || !len) return false;

    // Se o D-Cache estiver ativo, flush da área de memória
    SCB_CleanDCache_by_Addr((uint32_t*)buf, len);

    // Garante stream OFF antes de configurar
    LL_DMA_DisableStream(TX_DMA, TX_DMA_STREAM);

    // Endereços
    LL_DMA_SetPeriphAddress(TX_DMA, TX_DMA_STREAM, usart_tdr_addr(TX_USART));
    LL_DMA_SetMemoryAddress(TX_DMA, TX_DMA_STREAM, (uint32_t)buf);
    LL_DMA_SetDataLength(TX_DMA, TX_DMA_STREAM, len);

    // Habilita requisição de TX por DMA no USART
    LL_USART_EnableDMAReq_TX(TX_USART);

    // Dispara
    LL_DMA_EnableStream(TX_DMA, TX_DMA_STREAM);
    return true;
}

void usart_dma_stop_tx_h7(void)
{
    LL_DMA_DisableStream(TX_DMA, TX_DMA_STREAM);
    LL_USART_DisableDMAReq_TX(TX_USART);
}


void usart_dma_irq_handler_h7(void)
{
    if (LL_DMA_IsActiveFlag_TC1(TX_DMA)) {
        LL_DMA_ClearFlag_TC1(TX_DMA);

        // anda com fila
        tx_tail = (tx_tail + 1) % UART_TX_QUEUE_LEN;

        if (tx_tail != tx_head) {
            // ainda tem pacotes na fila → dispara próximo
            usart_dma_start_tx_h7(tx_queue[tx_tail].data, tx_queue[tx_tail].len);
        } else {
            // fila vazia
            tx_busy = false;
        }
    }

    if (LL_DMA_IsActiveFlag_TE1(TX_DMA)) {
        LL_DMA_ClearFlag_TE1(TX_DMA);
        // TODO: tratar erro
    }
}




bool usart_dma_enqueue_tx(const uint8_t *buf, uint16_t len)
{
    if (!buf || !len || len > UART_TX_BUF_SIZE) return false;

    int next_head = (tx_head + 1) % UART_TX_QUEUE_LEN;
    if (next_head == tx_tail) {
        // fila cheia
        return false;
    }

    // copia para fila
    memcpy(tx_queue[tx_head].data, buf, len);
    tx_queue[tx_head].len = len;
    tx_head = next_head;

    // se DMA não está ocupado, dispara já
    if (!tx_busy) {
        tx_busy = true;
        usart_dma_start_tx_h7(tx_queue[tx_tail].data, tx_queue[tx_tail].len);
    }

    return true;
}


