/*
 * mh.c
 *
 *  Created on: Aug 16, 2025
 *      Author: stephan
 */

#include "mh.h"
#include "utl_crc16.h"
#include "cobs.h"
#include <string.h> // memset, memcpy

/* =====================================================================
 * Funções internas
 * ===================================================================== */
static void mh_reset(mh_msg_t *msg)
{
    msg->size = 0;
    memset(msg->payload, 0, sizeof(msg->payload));
    msg->checksum = 0;

    memset(msg->rx_buf, 0, sizeof(msg->rx_buf));
    msg->rx_size = 0;
    msg->can_decode = false;
    msg->next_frame_pos = 0;
}


/* =====================================================================
 * API pública
 * ===================================================================== */
bool mh_init(mh_msg_t *msg)
{
    if (!msg) return false;
    mh_reset(msg);
    return true;
}


mh_status_t mh_append(mh_msg_t *msg, uint8_t *data, size_t size)
{
    if ((msg->rx_size + size) > MH_MAX_BUFFER_SIZE)
        return MH_STATUS_ERROR_BUFFER_FULL;

    memcpy(&msg->rx_buf[msg->rx_size], data, size);
    msg->rx_size += size;

    for (size_t i = 0; i < msg->rx_size; i++)
    {
        if (msg->rx_buf[i] == 0x00)
        {
            msg->next_frame_pos = i + 1; // comprimento bruto do frame
            msg->can_decode = true;
            return MH_STATUS_DECODE_OK;
        }
    }
    return MH_STATUS_OK;
}


bool mh_encode(mh_msg_t *msg)
{
    if (msg->size > MH_MAX_UTIL_BUFFER_SIZE) return false;

    size_t raw_len = msg->size + sizeof(uint16_t);
    uint8_t raw_data[MH_MAX_UTIL_BUFFER_SIZE + sizeof(uint16_t)];

    memcpy(raw_data, msg->payload, msg->size);

    uint16_t crc = utl_crc16_data(msg->payload, msg->size, 0xFFFF);
    memcpy(&raw_data[msg->size], &crc, sizeof(crc));

    uint8_t cobs_encoded[MH_MAX_BUFFER_SIZE];
    size_t cobs_len = cobs_encode(raw_data, cobs_encoded, raw_len);

    memcpy(msg->payload, cobs_encoded, cobs_len);
    msg->payload[cobs_len] = 0x00;
    msg->size = cobs_len + 1;

    return true;
}


mh_status_t mh_decode(mh_msg_t *msg)
{
    if (!msg->can_decode || msg->next_frame_pos == 0)
        return MH_STATUS_ERROR;

    size_t frame_len = msg->next_frame_pos; // inclui o 0x00 final
    if (frame_len < 2) {
        mh_reset(msg);
        return MH_STATUS_ERROR_INVALID_COBS;
    }

    /* Decodificar (sem o 0x00) */
    uint8_t decoded[MH_MAX_UTIL_BUFFER_SIZE + sizeof(uint16_t)];
    size_t decoded_len = cobs_decode(msg->rx_buf, decoded, frame_len - 1);

    if (decoded_len <= sizeof(uint16_t) || decoded_len > (sizeof(decoded))) {
        mh_reset(msg);
        return MH_STATUS_ERROR_INVALID_COBS;
    }

    size_t payload_size = decoded_len - sizeof(uint16_t);

    uint16_t received_crc;
    memcpy(&received_crc, &decoded[payload_size], sizeof(uint16_t));

    uint16_t computed_crc = utl_crc16_data(decoded, payload_size, 0xFFFF);
    if (computed_crc != received_crc) {
        mh_reset(msg);
        return MH_STATUS_ERROR_INVALID_CRC;
    }

    if (payload_size > MH_MAX_UTIL_BUFFER_SIZE) {
        mh_reset(msg);
        return MH_STATUS_ERROR_BUFFER_FULL;
    }

    /* Entregar payload decodificado ao chamador */
    memcpy(msg->payload, decoded, payload_size);
    msg->size = payload_size;
    msg->checksum = received_crc;

    /* Preservar bytes remanescentes (possíveis próximos frames) em rx_buf */
    size_t remaining = (msg->rx_size > frame_len) ? (msg->rx_size - frame_len) : 0;
    if (remaining > 0)
        memmove(msg->rx_buf, &msg->rx_buf[frame_len], remaining);
    msg->rx_size = remaining;

    /* Procurar um novo frame no que sobrou */
    msg->can_decode = false;
    msg->next_frame_pos = 0;
    for (size_t i = 0; i < msg->rx_size; i++) {
        if (msg->rx_buf[i] == 0x00) {
            msg->next_frame_pos = i + 1;
            msg->can_decode = true;
            break;
        }
    }

    return MH_STATUS_DECODE_OK;
}
