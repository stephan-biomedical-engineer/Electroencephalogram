/*
 * hal_ser.c
 *
 *  Created on: Aug 15, 2025
 *      Author: stephan
 */

#include "hal.h"

static hal_ser_driver_t *drv = &HAL_SER_DRIVER;

void hal_ser_configure(hal_ser_dev_t dev, uint32_t baud_rate, hal_ser_data_size_t data_size, hal_ser_parity_t parity, hal_ser_stop_bit_t stop_bits, hal_ser_flow_control_t flow_control)
{
    drv->configure(dev,baud_rate,data_size,parity,stop_bits,flow_control);
}

void hal_ser_flush(hal_ser_dev_t dev)
{
    drv->flush(dev);
}

uint16_t hal_ser_bytes_available(hal_ser_dev_t dev)
{
    return drv->bytes_available(dev);
}

uint16_t hal_ser_read(hal_ser_dev_t dev, uint8_t *buffer, uint16_t size)
{
    return drv->read(dev,buffer,size);
}

uint16_t hal_ser_read_byte(hal_ser_dev_t dev, uint8_t *c)
{
    return drv->read_byte(dev,c);
}

void hal_ser_write(hal_ser_dev_t dev, uint8_t *buffer, uint16_t len)
{
    drv->write(dev, buffer, len);
}

void hal_ser_write_byte(hal_ser_dev_t dev, uint8_t c)
{
    drv->write_byte(dev,c);
}

void hal_ser_open(hal_ser_dev_t dev)
{
    drv->open(dev);
}

void hal_ser_close(hal_ser_dev_t dev)
{
    drv->close(dev);
}

void hal_ser_interrupt_set(hal_ser_dev_t dev,hal_ser_interrupt_t fun)
{
    drv->interrupt_set(dev,fun);
}

void hal_ser_init(void)
{
    drv->init();
}

void hal_ser_deinit(void)
{
    drv->deinit();
}
