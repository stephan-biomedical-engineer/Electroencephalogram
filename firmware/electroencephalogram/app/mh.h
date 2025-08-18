/*
 * mh.h
 *
 *  Created on: Aug 17, 2025
 *      Author: stephan
 */

#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "cobs.h"

#define MAX_EEG_CHANNELS 8
#define BYTES_PER_CHANNEL 2

// Tamanho máximo do payload em bytes
#define EEG_PAYLOAD_MAX_SIZE (MAX_EEG_CHANNELS * BYTES_PER_CHANNEL)

// Buffer máximo para payload + CRC + overhead do COBS
#define MH_MAX_UTIL_BUFFER_SIZE EEG_PAYLOAD_MAX_SIZE
#define MH_MAX_BUFFER_SIZE      COBS_OVERHEAD_SIZE(MH_MAX_UTIL_BUFFER_SIZE) + sizeof(uint16_t) + 1

typedef struct __attribute__((packed)) mh_msg_s
{
    /* --- Saída decodificada / uso TX --- */
    size_t  size;                              // tamanho do payload válido em 'payload' (decodificado) OU, após encode, tamanho do frame codificado
    uint8_t payload[MH_MAX_BUFFER_SIZE];       // buffer para payload (decodificado) ou frame (codificado)
    uint16_t checksum;                         // CRC16 do payload decodificado

    /* --- Estado de recepção (RX) --- */
    uint8_t rx_buf[MH_MAX_BUFFER_SIZE];        // acumulador de bytes recebidos (frames colados entram aqui)
    size_t  rx_size;                           // quantos bytes válidos há em rx_buf
    bool    can_decode;                        // achou um 0x00 (fim de frame) em rx_buf
    size_t  next_frame_pos;                    // posição do fim de frame (i+1) em rx_buf
} mh_msg_t;


typedef enum mh_status_s
{
    MH_STATUS_OK = 0,
    MH_STATUS_DECODE_OK,
    MH_STATUS_ERROR,
    MH_STATUS_ERROR_BUFFER_FULL,
    MH_STATUS_ERROR_INVALID_CRC,
    MH_STATUS_ERROR_INVALID_COBS
} mh_status_t;

bool mh_init(mh_msg_t *msg);

mh_status_t mh_append(mh_msg_t *msg, uint8_t *data, size_t size);

bool mh_encode(mh_msg_t *msg);

mh_status_t mh_decode(mh_msg_t *msg);

