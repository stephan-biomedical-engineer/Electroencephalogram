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
static uint16_t sample_counter = 0;
extern uint8_t adc_samples[EEG_SAMPLE_SIZE];
extern volatile uint8_t adc_data_ready;
extern uint16_t adc_buffer[ADC_DMA_BUFFER_SIZE_SAMPLES];


static void app_prepare_eeg_packet(mh_msg_t *msg_out) {

	if(!adc_data_ready) return;										// Verifica se há novos dados do ADC

    mh_init(msg_out);												// Limpa a mensagem anterior

    uint32_t timestamp = (uint32_t)sample_counter;

    memcpy(msg_out->payload, &timestamp, sizeof(uint32_t));			// Monta o payload: [timestamp][dados dos canais]

    // Converte as amostras do ADC para o payload (12 bits -> 2 bytes cada)
    for(int i = 0; i < EEG_CHANNELS; i++) {
        uint16_t sample = adc_buffer[i]; // 16 bits
        memcpy(&msg_out->payload[EEG_PACKET_HEADER + (i * 2)], &sample, 2);
    }

    msg_out->size = EEG_PACKET_SIZE;
    sample_counter++;
    adc_data_ready = 0;
}


static void send_test_packet() {
    mh_msg_t test_msg;
    mh_init(&test_msg);

    uint16_t timestamp = 0xABCD;
    uint16_t channels[4] = {0x0FFF, 0x0AAA, 0x0555, 0x0000};

    memcpy(test_msg.payload, &timestamp, 2);
    memcpy(test_msg.payload + 2, channels, 8);
    test_msg.size = 10;

    if (mh_encode(&test_msg)) {
        hal_ser_write(DEBUG_SERIAL_DEV, test_msg.payload, test_msg.size);
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


void app_setup(void)
{
	hw_adc_init();
	hw_adc_start_acquisition();
	hal_ser_init();
	hal_ser_configure(DEBUG_SERIAL_DEV, 115200, HAL_SER_DATA_SIZE_8,HAL_SER_PARITY_NONE,
			HAL_SER_STOP_BITS_1, HAL_SER_FLOW_CONTROL_NONE);
	hal_ser_interrupt_set(DEBUG_SERIAL_DEV, app_uart_rx_cbk);
	hal_ser_open(DEBUG_SERIAL_DEV);
	send_test_packet();
}

void app_loop(void)
{
	// 1. Verifica mensagens recebidas
	    if(app_check_input(&uart_rx_cb, &mcu_msg_in))
	    {
	        // Processa mensagem recebida (se necessário)
	        // ...
	    }

	    // 2. Prepara e envia dados do EEG
	    app_prepare_eeg_packet(&mcu_msg_out);

	    if(mcu_msg_out.size > 0)
	    {
	        if(mh_encode(&mcu_msg_out))
	        {
	        	/*
	        	 --------------------------------FOMA DAS MENSAGENS----------------------------------------
	        	 *
	        	 TT: Timestamp		C1: Channel1	C2: Channel2	C3:	Channel3	C4: Channel4	CR: CRC
	        	 *
	        	 Pré-codificação: TT TT TT TT C1 C1 C2 C2 C3 C3 C4 C4 CR CR
	        	 *
	        	 Pós-codificação: [COBS-HEADER] TT TT TT TT C1 C1 C2 C2 C3 C3 C4 C4 CR CR [0x00]
	        	 *
	        	 * */
	            hal_ser_write(DEBUG_SERIAL_DEV, mcu_msg_out.payload, mcu_msg_out.size);
	            mcu_msg_out.size = 0; // Reset após envio
	        }
	    }

	    // Pequeno delay para não sobrecarregar a UART
	    HAL_Delay(1);
}
