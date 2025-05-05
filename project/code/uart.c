#include "uart.h"

void uartInitAll()
{
    // 初始化串口1
    uart_init(OPENMV_UART_INDEX, OPENMV_UART_BAUDRATE, OPENMV_UART_TX_PIN, OPENMV_UART_RX_PIN);
    // 初始化串口4
    uart_init(MCX_UART_INDEX, MCX_UART_BAUDRATE, MCX_UART_TX_PIN, MCX_UART_RX_PIN);

    // 初始化fifo
//    fifo_init(&uart_data_fifo, FIFO_DATA_8BIT, uart_get_data, 64);
}

void uartMCXSendString(const char *str)
{
    uart_write_string(MCX_UART_INDEX, str);
}

void uartMCXSendByte(uint8 dat)
{
    uart_write_byte(MCX_UART_INDEX, dat);
}

void uartMCXSendData(uint8 *dat, uint32 len)
{
    for (uint32 i = 0; i < len; i++)
    {
        uart_write_byte(MCX_UART_INDEX, dat[i]);
    }
}

void uartOpenMVSendString(const char *str)
{
    uart_write_string(OPENMV_UART_INDEX, str);
}