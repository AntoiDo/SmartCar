#ifndef UART_H
#define UART_H

#include "zf_common_headfile.h"

#define OPENMV_UART_INDEX UART_1
#define OPENMV_UART_BAUDRATE 115200
#define OPENMV_UART_TX_PIN UART1_TX_B12
#define OPENMV_UART_RX_PIN UART1_RX_B13

#define MCX_UART_INDEX UART_4
#define MCX_UART_BAUDRATE 115200
#define MCX_UART_TX_PIN UART4_TX_C16
#define MCX_UART_RX_PIN UART4_RX_C17

//uint8 uart_get_data[64]; // 串口接收数据缓冲区
//uint8 fifo_get_data[64]; // fifo 输出读出缓冲区

//uint8 get_data = 0;         // 接收数据变量
//uint32 fifo_data_count = 0; // fifo 数据个数

//fifo_struct uart_data_fifo;

void uartInitAll();
void uartMCXSendString(const char *str);
void uartMCXSendByte(uint8 dat);
void uartMCXSendData(uint8 *dat, uint32 len);

void uartOpenMVSendString(const char *str);

#endif