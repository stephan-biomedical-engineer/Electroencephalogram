/*
 * cbf.h
 *
 *  Created on: Aug 15, 2025
 *      Author: stephan
 */

#pragma once

#include <stddef.h>
#include <stdint.h>

#define CBF_DECLARE(name, _size)	\
	static uint8_t name##buffer[_size + 1];	\
	static cbf_t name = {	\
			.head = 0,	\
			.tail = 0,	\
			.size = _size+1,	\
			.buffer = name##buffer,	\
	}


typedef enum cbf_status_s
{
	CBF_OK = 0,
	CBF_FULL,
	CBF_EMPTY,
	CBF_TIMEOUT
} cbf_status_t;


typedef struct cbf_s
{
	volatile size_t head;
	volatile size_t tail;
	uint16_t size;
	uint8_t *buffer;
} cbf_t;


uint16_t cbf_bytes_available(cbf_t *cb);

cbf_status_t cbf_flush(cbf_t *cb);

cbf_status_t cbf_get(cbf_t *cb, uint8_t *c);

cbf_status_t cbf_put(cbf_t *cb, uint8_t c);

cbf_status_t cbf_init(cbf_t *cb, uint8_t *area, uint16_t size);

cbf_status_t cbf_preview(cbf_t *cb, uint8_t *c, uint8_t shift);

