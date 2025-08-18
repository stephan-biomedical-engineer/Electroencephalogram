	/*
	 * hw_uart.c
	 *
	 *  Created on: Jun 15, 2025
	 *      Author: stephan
	 */

	#include "cbf.h"
	#include "hal_ser.h"
	#include "main.h"
	#include "stm32h730xx.h"
	#include "stm32h7xx_ll_usart.h"
	#include <stdbool.h>


	#define PORT_UART_BUFFER_SIZE 256

	CBF_DECLARE(cb, PORT_UART_BUFFER_SIZE);

	USART_TypeDef *uart[] = {UART8};

	cbf_t *uart_cbf[] = {&cb};

	IRQn_Type uart_irq[] = {UART8_IRQn};

	hal_ser_interrupt_t interrupt_cbk[] = {0};

	uint8_t irq_priority[] = {3};


	static void port_serial_interrupt_init(hal_ser_dev_t dev)
	{
		USART_TypeDef *Instance = uart[dev];

		/* Enable the UART Error Interrupt: (Frame error, noise error, overrun error) */
		Instance->CR3 |= USART_CR3_EIE;
		/* Enable the UART parity error interrupt and RXNE interrupt */
		Instance->CR1 |= (USART_CR1_PEIE | USART_CR1_RXNEIE);
	}


	static void port_serial_interrupt_state_set(hal_ser_dev_t dev, bool state)
	{
		IRQn_Type irq = uart_irq[dev];

		if(state)
		{
			NVIC_ClearPendingIRQ(irq);
			NVIC_EnableIRQ(irq);

			// edit interrupt priority
			NVIC_SetPriority(irq, irq_priority[dev]);
		}
		else
			NVIC_DisableIRQ(irq);
	}


	static void port_serial_rx_interrupt(hal_ser_dev_t dev)
	{
		USART_TypeDef *Instance = uart[dev];
		cbf_t *cb = uart_cbf[dev];
		uint32_t isrflags = Instance->ISR;
		uint32_t errorflags;
		/* If no error occurs */
		errorflags = (isrflags & (uint32_t)(USART_ISR_PE | USART_ISR_FE | USART_ISR_ORE | USART_ISR_NE));

		if((errorflags == RESET) && ((isrflags & USART_ISR_RXFF) != RESET))
		{
			uint8_t c = Instance->RDR;

			if(interrupt_cbk[dev])
				interrupt_cbk[dev](c);
			else
				cbf_put(cb,c);

			return;
		}

		/* If some errors occur */
		while(errorflags != RESET)
		{
			isrflags = Instance->RDR;
			isrflags = Instance->ISR;

			if(isrflags & USART_ISR_PE)
				Instance->ICR = USART_ICR_PECF;

			if(isrflags & USART_ISR_FE)
				Instance->ICR = USART_ICR_FECF;

			if(isrflags & USART_ISR_ORE)
				Instance->ICR = USART_ICR_ORECF;

			if(isrflags & USART_ISR_NE)
				Instance->ICR = USART_ICR_NECF;

			errorflags = (isrflags & (uint32_t)(USART_ISR_PE | USART_ISR_FE | USART_ISR_ORE | USART_ISR_NE));
		}
	}


	void UART8_IRQHandler(void)
	{
		port_serial_rx_interrupt(HAL_SER_DEV_DEBUG);
	}


	static void port_serial_configure(hal_ser_dev_t dev, uint32_t baudrate, hal_ser_data_size_t data_size, hal_ser_parity_t parity, hal_ser_stop_bit_t stop_bits, hal_ser_flow_control_t flow_control)
	{
		LL_USART_InitTypeDef cfg = {0};
		USART_TypeDef *uart_instance = uart[dev];

		uint32_t data_size_types[] = {LL_USART_DATAWIDTH_7B, LL_USART_DATAWIDTH_8B, LL_USART_DATAWIDTH_9B};
		uint32_t stop_bits_types[] = {LL_USART_STOPBITS_1, LL_USART_STOPBITS_2};
		uint32_t parity_types[]    = {LL_USART_PARITY_NONE,LL_USART_PARITY_EVEN,LL_USART_PARITY_ODD};
		uint32_t flow_ctrl_types[] = {LL_USART_HWCONTROL_NONE,LL_USART_HWCONTROL_RTS_CTS};

		LL_USART_Disable(uart_instance);

		cfg.BaudRate = baudrate;
		cfg.DataWidth = data_size_types[data_size];
		cfg.StopBits = stop_bits_types[stop_bits];
		cfg.Parity = parity_types[parity];
		cfg.TransferDirection = LL_USART_DIRECTION_TX_RX;
		cfg.HardwareFlowControl = flow_ctrl_types[flow_control];
		cfg.OverSampling = LL_USART_OVERSAMPLING_16;

		LL_USART_Init(uart_instance, &cfg);
		LL_USART_ConfigAsyncMode(uart_instance);
		LL_USART_Enable(uart_instance);
	}


	static void port_serial_flush(hal_ser_dev_t dev)
	{
		if(interrupt_cbk[dev])
			return;

		cbf_flush(uart_cbf[dev]);
	}


	static uint16_t port_serial_bytes_available(hal_ser_dev_t dev)
	{
		uint16_t size;

		if(interrupt_cbk[dev])
			return 0;
		size = cbf_bytes_available(uart_cbf[dev]);
		return size;
	}


	static uint16_t port_serial_read_byte(hal_ser_dev_t dev, uint8_t *c)
	{
	    if(interrupt_cbk[dev])
	        return -1; // Or a different error code, as uint16_t can't be -1

	    if(port_serial_bytes_available(dev) == 0)
	        return 0;

	    cbf_get(uart_cbf[dev], c);

	    return 1;
	}


	static uint16_t port_serial_read(hal_ser_dev_t dev, uint8_t *buffer, uint16_t size)
	{
	    uint16_t pos = 0;
	    uint8_t temp_c;

	    if(interrupt_cbk[dev])
	        return -1;

	    if(port_serial_bytes_available(dev) == 0)
	        return 0;

	    while(pos < size)
	    {
	        if(cbf_get(uart_cbf[dev], &temp_c) == CBF_EMPTY)
	            return pos;

	        buffer[pos] = (uint8_t)temp_c;
	        pos++;
	    }

	    return pos;
	}




	static void port_serial_write_byte(hal_ser_dev_t dev, uint8_t c)
	{
		USART_TypeDef *Instance = uart[dev];

		while( !(Instance->ISR & USART_ISR_TXFE))
		{}

		Instance->TDR = c;
	}


	static void port_serial_write(hal_ser_dev_t dev, uint8_t *buffer, uint16_t len)
	{
		USART_TypeDef *Instance = uart[dev];
		uint16_t pos;

		for(pos = 0 ; pos < len ; pos++)
		{
			port_serial_write_byte(dev, buffer[pos]);
		}

		while( !(Instance->ISR & USART_ISR_TC))
		{}
	}


	static void port_serial_open(hal_ser_dev_t dev)
	{
		port_serial_flush(dev);
		port_serial_interrupt_state_set(dev, true);
	}


	static void port_serial_close(hal_ser_dev_t dev)
	{
		port_serial_interrupt_state_set(dev, false);
	}


	void port_interrupt_set(hal_ser_dev_t dev, hal_ser_interrupt_t fun)
	{
		interrupt_cbk[dev] = fun;
	}


	static void port_serial_init(void)
	{
		port_serial_interrupt_state_set(HAL_SER_DEV_DEBUG, false);
		port_serial_interrupt_init(HAL_SER_DEV_DEBUG);
		port_serial_flush(HAL_SER_DEV_DEBUG);
	}


	extern int __io_putchar(int ch) __attribute__((weak));

	int __io_putchar(int ch)
	{
		while( !LL_USART_IsActiveFlag_TXE(UART8) ) {}
		UART8->TDR = ch;
		return 1;
	}


	hal_ser_driver_t HAL_SER_DRIVER =
	{
	    .configure = port_serial_configure,
	    .flush = port_serial_flush,
	    .bytes_available = port_serial_bytes_available,
	    .read = port_serial_read,
	    .read_byte = port_serial_read_byte,
	    .write = port_serial_write,
	    .write_byte = port_serial_write_byte,
	    .open = port_serial_open,
	    .close = port_serial_close,
	    .interrupt_set = port_interrupt_set,
	    .init = port_serial_init,
	};
