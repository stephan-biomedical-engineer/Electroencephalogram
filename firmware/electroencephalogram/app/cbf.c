/*
 * cbf.c
 *
 *  Created on: Aug 15, 2025
 *      Author: stephan
 */

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include "cbf.h"

#define CBF_INCREMENT(pos, size) ((((pos) + 1) >= (size)) ? 0 : (pos) + 1)

uint16_t cbf_bytes_available(cbf_t *cb)
{
	if(cb->head >= cb->tail)
	{
		return cb->head - cb->tail;
	}
	else
	{
		return cb->head + (cb->size - cb->tail);
	}
}


cbf_status_t cbf_flush(cbf_t *cb)
{
	cb->head = 0;
	cb->tail = 0;
	return CBF_OK;
}


cbf_status_t cbf_get(cbf_t *cb, uint8_t *c)
{
	if(cb->head == cb->tail)
	{
		return CBF_EMPTY;
	}
	else
	{
		*c = cb->buffer[cb->tail];
		cb->tail = CBF_INCREMENT(cb->tail, cb->size);
		return CBF_OK;
	}
}

cbf_status_t cbf_put(cbf_t *cb, uint8_t c)
{
	size_t next_head = CBF_INCREMENT(cb->head, cb->size);
	if(next_head == cb->tail)
	{
		return CBF_FULL;
	}
	else
	{
		cb->buffer[cb->head] = c;
		cb->head = next_head;
		return CBF_OK;
	}
}


cbf_status_t cbf_init(cbf_t *cb, uint8_t *area, uint16_t size)
{
	cb->buffer = area;
	cb->size = size;
	cb->head = cb->tail = 0;

	return CBF_OK;
}


cbf_status_t cbf_preview(cbf_t *cb, uint8_t *c, uint8_t shift)
{
	if(cb->head == cb->tail)
	{
		return CBF_EMPTY;
	}
	else
	{
		*c = cb->buffer[cb->tail + shift];
		return CBF_OK;
	}
}
