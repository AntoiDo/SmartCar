#include "uart.h"

uint8 uart_get_data[64]; // 串口接收数据缓冲区
uint8 fifo_get_data[64]; // fifo 输出读出缓冲区

uint32 fifo_data_count = 0; // fifo 数据个数

uint8 get_data = 0;
fifo_struct openmv_uart_data_fifo;

/**
 * @brief 串口初始化函数
 * @note  初始化所有串口
 * @param  none
 */
void uartInitAllCamPeriph()
{
    // 初始化串口1
    uart_init(OPENMV_UART_INDEX, OPENMV_UART_BAUDRATE, OPENMV_UART_TX_PIN, OPENMV_UART_RX_PIN);
    // 初始化串口4
    uart_init(MCX_UART_INDEX, MCX_UART_BAUDRATE, MCX_UART_TX_PIN, MCX_UART_RX_PIN);

    fifo_init(&openmv_uart_data_fifo, FIFO_DATA_8BIT, uart_get_data, 64); // 初始化fifo

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

/**
 * * @brief openmv串口接收数据处理函数
 * * @note  用来测试openmv和主控板之间的串口通信
 */
void uartOpenMVReceiveData()
{
    fifo_data_count = fifo_used(&openmv_uart_data_fifo); // 查看 fifo 是否有数据
    if (fifo_data_count != 0)                            // 读取到数据了
    {
        fifo_read_buffer(&openmv_uart_data_fifo, fifo_get_data, &fifo_data_count, FIFO_READ_AND_CLEAN); // 将 fifo 中数据读出并清空 fifo 挂载的缓冲
        // uart_write_string(OPENMV_UART_INDEX, "\r\nUART get data:");                                     // 输出测试信息
        // uart_write_buffer(OPENMV_UART_INDEX, fifo_get_data, fifo_data_count);                           // 将读取到的数据发送出去
        ips200_show_uint(0, 0, fifo_get_data[0], 3);  // 显示接收到的数据
        ips200_show_uint(0, 16, fifo_get_data[1], 3); // 显示接收到的数据
        ips200_show_uint(0, 32, fifo_get_data[2], 3); // 显示接收到的数据
        ips200_show_uint(0, 48, fifo_get_data[3], 3); // 显示接收到的数据
    }
}