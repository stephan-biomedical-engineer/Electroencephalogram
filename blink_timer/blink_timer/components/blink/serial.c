#include "serial.h"
#include "driver/uart.h"
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define BUF_SIZE (128)
static char s_input_buffer[BUF_SIZE];
static int s_input_index = 0;

void serial_init()
{
    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };
    uart_param_config(UART_NUM_0, &uart_config);
    uart_driver_install(UART_NUM_0, BUF_SIZE * 2, 0, 0, NULL, 0);
}

char* serial_read_line()
{
    char c;
    s_input_index = 0;
    memset(s_input_buffer, 0, sizeof(s_input_buffer));

    while (1) {
        int len = uart_read_bytes(UART_NUM_0, (uint8_t*)&c, 1, portMAX_DELAY);
        if (len == 1) {
            uart_write_bytes(UART_NUM_0, &c, 1);

            if (c == '\r' || c == '\n') {
                if (s_input_index > 0) {
                    s_input_buffer[s_input_index] = '\0';
                    return s_input_buffer;
                }
            } else if (c == 8 || c == 127) { // Backspace/Delete
                if (s_input_index > 0) {
                    s_input_index--;
                    uart_write_bytes(UART_NUM_0, "\b \b", 3);
                }
            } else if (s_input_index < BUF_SIZE - 1 && 
                      ((c >= '0' && c <= '9') || c == '.' || c == '-')) {
                s_input_buffer[s_input_index++] = c;
            }
        }
    }
}
