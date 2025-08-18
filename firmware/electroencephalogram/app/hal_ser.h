/*
 * hal_ser.h
 *
 *  Created on: Aug 15, 2025
 *      Author: stephan
 */

#pragma once

typedef enum hal_ser_parity_e
{
	HAL_SER_PARITY_NONE = 0,
	HAL_SER_PARITY_EVEN,
	HAL_SER_PARITY_ODD,
} hal_ser_parity_t;


typedef enum hal_ser_stop_bit_e
{
	HAL_SER_STOP_BITS_1 = 0,
	HAL_SER_STOP_BITS_2,
} hal_ser_stop_bit_t;


typedef enum hal_ser_data_size_e
{
	HAL_SER_DATA_SIZE_7 = 0,
	HAL_SER_DATA_SIZE_8,
	HAL_SER_DATA_SIZE_9,
} hal_ser_data_size_t;


typedef enum hal_ser_flow_control_e
{
	HAL_SER_FLOW_CONTROL_NONE = 0,
	HAL_SER_FLOW_CONTROL_CTS_RTS,
} hal_ser_flow_control_t;


typedef enum hal_ser_baudrate_e
{
	HAL_SER_BAUDRATE_9600 = 0,
	HAL_SER_BAUDRATE_19200,
	HAL_SER_BAUDRATE_38400,
	HAL_SER_BAUDRATE_57600,
	HAL_SER_BAUDRATE_115200,
} hal_ser_baudrate_t;


typedef enum hal_ser_dev_e
{
	HAL_SER_DEV_DEBUG = 0,
	HAL_SER_DEV_DEV0,
	HAL_SER_DEV_DEV1,
} hal_ser_dev_t;


typedef void (*hal_ser_interrupt_t)(uint8_t c);


typedef struct hal_ser_config_s
{
	hal_ser_baudrate_t baudrate;
	hal_ser_parity_t parity;
	hal_ser_stop_bit_t stop_bits;
	hal_ser_data_size_t data_size;
	hal_ser_flow_control_t flow_control;
} hal_ser_config_t;


typedef struct hal_ser_driver_s
{
	void (*configure)(hal_ser_dev_t dev, uint32_t baudrate, hal_ser_data_size_t data_size,
			hal_ser_parity_t parity, hal_ser_stop_bit_t stop_bits, hal_ser_flow_control_t flow_control);
	void (*flush)(hal_ser_dev_t dev);
	uint16_t (*bytes_available)(hal_ser_dev_t dev);
	uint16_t (*read)(hal_ser_dev_t dev, uint8_t *buffer, uint16_t size);
	uint16_t (*read_byte)(hal_ser_dev_t dev, uint8_t *c);
    void (*write)(hal_ser_dev_t dev, uint8_t *buffer, uint16_t len);
	void (*write_byte)(hal_ser_dev_t dev, uint8_t c);
	void (*open)(hal_ser_dev_t dev);
	void (*close)(hal_ser_dev_t dev);
	void (*interrupt_set)(hal_ser_dev_t dev, hal_ser_interrupt_t fun);
	void (*init)(void);
	void (*deinit)(void);
} hal_ser_driver_t;


void hal_ser_configure(hal_ser_dev_t dev, uint32_t baudrate, hal_ser_data_size_t data_size,
hal_ser_parity_t parity, hal_ser_stop_bit_t stop_bits, hal_ser_flow_control_t flow_control);

void hal_ser_flush(hal_ser_dev_t dev);

uint16_t hal_ser_bytes_available(hal_ser_dev_t dev);

uint16_t hal_ser_read(hal_ser_dev_t dev, uint8_t *buffer, uint16_t size);

uint16_t hal_ser_read_byte(hal_ser_dev_t dev, uint8_t *c);

void hal_ser_write(hal_ser_dev_t dev, uint8_t *buffer, uint16_t len);

void hal_ser_open(hal_ser_dev_t dev);

void hal_ser_close(hal_ser_dev_t dev);

void hal_ser_interrupt_set(hal_ser_dev_t dev, hal_ser_interrupt_t fun);

void hal_ser_init(void);

void hal_ser_deinit(void);
