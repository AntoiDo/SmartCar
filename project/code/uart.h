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

extern uint8 uart_get_data[64]; // 串口接收数据缓冲区
extern uint8 fifo_get_data[64]; // fifo 输出读出缓冲区
extern fifo_struct openmv_uart_data_fifo;
extern uint8 get_data; // 接收数据

void uartInitAllCamPeriph();
void uartMCXSendString(const char *str);
void uartMCXSendByte(uint8 dat);
void uartMCXSendData(uint8 *dat, uint32 len);

void uartOpenMVReceiveData();
void uartOpenMVSendString(const char *str);

#endif