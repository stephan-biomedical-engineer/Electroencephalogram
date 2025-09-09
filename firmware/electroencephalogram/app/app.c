/*
 * app.c
 *
 *  Created on: May 25, 2025
 *      Author: stephan
 */

#include "hal.h"
#include "main.h"
#include "utl_dbg.h"
#include "hal_ser.h"
#include "cbf.h"
#include "mh.h"
#include "hw_adc.h"
#include "hal_uart_dma.h"
#include <stdint.h>
#include <stdbool.h>

#define DEBUG_SERIAL_DEV  HAL_SER_DEV_DEBUG
#define RX_CB_SIZE 1030
#define EEG_CHANNELS        4
#define EEG_SAMPLE_SIZE     (EEG_CHANNELS * 2)  // 2 bytes por canal (12 bits ADC)
#define EEG_PACKET_HEADER   sizeof(uint32_t)    // Timestamp (4 bytes)
#define EEG_PACKET_SIZE     (EEG_PACKET_HEADER + EEG_SAMPLE_SIZE)

CBF_DECLARE(uart_rx_cb, RX_CB_SIZE);


static mh_msg_t mcu_msg_in = { 0 } ;
static mh_msg_t mcu_msg_out = { 0 } ;
//static uint16_t sample_counter = 0;
extern uint8_t adc_samples[EEG_SAMPLE_SIZE];
extern uint16_t adc_buffer[ADC_DMA_BUFFER_SIZE_SAMPLES];


static uint64_t timestamp_ms64 = 0;
static uint32_t last_cycles = 0;

void update_timestamp(void)
{
    uint32_t now = DWT->CYCCNT;
    uint32_t diff = now - last_cycles; // lida com overflow automático
    last_cycles = now;

    // converte ciclos para ms acumulando em um contador de 64 bits
    timestamp_ms64 += diff / (SystemCoreClock / 1000U);
}

uint32_t get_timestamp_ms(void)
{
    return (uint32_t)timestamp_ms64; // ou usa full 64 bits se quiser
}


void process_adc_chunk(uint16_t *buf, int offset, int len)
{
    for (int i = 0; i < len; i += EEG_CHANNELS) {
        mh_init(&mcu_msg_out);

        // --- Timestamp em milissegundos ---
//        uint32_t timestamp_ms = DWT->CYCCNT / (SystemCoreClock / 1000U);
        update_timestamp();
        uint32_t timestamp_ms = get_timestamp_ms();
        memcpy(mcu_msg_out.payload, &timestamp_ms, sizeof(timestamp_ms));

        // 4 canais = 8 bytes
        memcpy(&mcu_msg_out.payload[EEG_PACKET_HEADER],
               &buf[offset + i], EEG_SAMPLE_SIZE);

        mcu_msg_out.size = EEG_PACKET_SIZE;

//        if (mh_encode(&mcu_msg_out)) {
//            usart_dma_start_tx_h7(mcu_msg_out.payload, mcu_msg_out.size);
//            mcu_msg_out.size = 0;
//        }
        if (mh_encode(&mcu_msg_out))
        {
            usart_dma_enqueue_tx(mcu_msg_out.payload, mcu_msg_out.size);
            mcu_msg_out.size = 0;
        }
    }
}


static bool app_check_input(cbf_t *cb, mh_msg_t *msg_in)
{
    bool process = false;

    uint16_t size = cbf_bytes_available(cb);

    if(size)
    {
        uint8_t data[size];

		for (uint16_t nb = 0; nb < size; nb++)
		{
			cbf_get(cb, &data[nb]);
		}

        mh_status_t st = mh_append(msg_in,data,size);

        if(st == MH_STATUS_DECODE_OK)
        {
            if(mh_decode(msg_in))
			{
                process = true;
			}
			else
			{
				cbf_flush(cb);
				mh_init(msg_in);
			}
        }
        else if (st != MH_STATUS_OK)
        {
			cbf_flush(cb);
			mh_init(msg_in);
		}
    }

    return process;
}


static void app_uart_rx_cbk(uint8_t c)
{
    static bool alert_printed = false;

    if(cbf_put(&uart_rx_cb,c) == CBF_FULL)
    {
        if(alert_printed == false)
        {
            alert_printed = true;
        }
    }
    else
        alert_printed = false;
}

static void uart_tx_complete_cb(int dev)
{
    (void)dev; // se não for usar o índice
    // Aqui você pode acender um LED de debug, ou sinalizar que buffer está livre
}

void app_setup(void)
{
    hw_adc_start_acquisition();

    // Inicializa UART pelo HAL normal (pinos, clock etc.)
    hal_ser_init();
    hal_ser_configure(DEBUG_SERIAL_DEV, 921600,
                      HAL_SER_DATA_SIZE_8, HAL_SER_PARITY_NONE,
                      HAL_SER_STOP_BITS_1, HAL_SER_FLOW_CONTROL_NONE);
    hal_ser_interrupt_set(DEBUG_SERIAL_DEV, app_uart_rx_cbk);
    hal_ser_open(DEBUG_SERIAL_DEV);

    // Inicializa DMA para UART
    usart_dma_init_h7();
    usart_dma_register_callback_h7(1, uart_tx_complete_cb);

    // --- Habilitar contador de ciclos (DWT) ---
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CYCCNT = 0;                          // zera contador
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;      // habilita contador

}

void app_loop(void)
{
    // 1. Verifica mensagens recebidas
    if(app_check_input(&uart_rx_cb, &mcu_msg_in))
    {
        // Processa mensagem recebida (se necessário)
    }

    // 2. Prepara e envia dados do EEG somente se houver dados novos

}


