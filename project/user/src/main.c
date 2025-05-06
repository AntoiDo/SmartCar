/*********************************************************************************************************************
 * RT1064DVL6A Opensourec Library 即（RT1064DVL6A 开源库）是一个基于官方 SDK 接口的第三方开源库
 * Copyright (c) 2022 SEEKFREE 逐飞科技
 *
 * 本文件是 RT1064DVL6A 开源库的一部分
 *
 * RT1064DVL6A 开源库 是免费软件
 * 您可以根据自由软件基金会发布的 GPL（GNU General Public License，即 GNU通用公共许可证）的条款
 * 即 GPL 的第3版（即 GPL3.0）或（您选择的）任何后来的版本，重新发布和/或修改它
 *
 * 本开源库的发布是希望它能发挥作用，但并未对其作任何的保证
 * 甚至没有隐含的适销性或适合特定用途的保证
 * 更多细节请参见 GPL
 *
 * 您应该在收到本开源库的同时收到一份 GPL 的副本
 * 如果没有，请参阅<https://www.gnu.org/licenses/>
 *
 * 额外注明：
 * 本开源库使用 GPL3.0 开源许可证协议 以上许可申明为译文版本
 * 许可申明英文版在 libraries/doc 文件夹下的 GPL3_permission_statement.txt 文件中
 * 许可证副本在 libraries 文件夹下 即该文件夹下的 LICENSE 文件
 * 欢迎各位使用并传播本程序 但修改内容时必须保留逐飞科技的版权声明（即本声明）
 *
 * 文件名称          main
 * 公司名称          成都逐飞科技有限公司
 * 版本信息          查看 libraries/doc 文件夹内 version 文件 版本说明
 * 开发环境          IAR 8.32.4 or MDK 5.33
 * 适用平台          RT1064DVL6A
 * 店铺链接          https://seekfree.taobao.com/
 *
 * 修改记录
 * 日期              作者                备注
 * 2022-09-21        SeekFree            first version
 ********************************************************************************************************************/

#include "zf_common_headfile.h"

// 打开新的工程或者工程移动了位置务必执行以下操作
// 第一步 关闭上面所有打开的文件
// 第二步 project->clean  等待下方进度条走完

// 本例程是开源库移植用空工程
int16 encoder_data_R = 0;
int16 encoder_data_B = 0;
int16 encoder_data_L = 0;
int Lspeed, Bspeed, Rspeed;

uint8 pwmL;
uint8 pwmR;
uint16 BaseSpeed = 100;
#define UART_INDEX (DEBUG_UART_INDEX)
#define UART_BAUDRATE (DEBUG_UART_BAUDRATE)
#define UART_TX_PIN (DEBUG_UART_TX_PIN)
#define UART_RX_PIN (DEBUG_UART_RX_PIN)

/**
 * @brief 发送数据到串口
 * @note 对应串口是COM7
 */
void Vofa_data(int data1, int data2, int data3, int data4, int data5)
{
  float data[5];
  uint8 tail[4] = {0x00, 0x00, 0x80, 0x7f};
  data[0] = (float)data1;
  data[1] = (float)data2;
  data[2] = (float)data3;
  data[3] = (float)data4;
  data[4] = (float)data5;

  uart_write_buffer(UART_INDEX, data, sizeof(float) * 5); // 通过串口传输数据，前面开多大的数组，后面占多大长度
  uart_write_buffer(UART_INDEX, tail, 4);                 // 结尾固定的数据包尾
}

int main(void)
{
  clock_init(SYSTEM_CLOCK_600M); // 不可删除
  debug_init();                  // 调试端口初始化
  Key_init(KEY_MAX);
  switch_init();
  BEEP_init();
  encoderInit();
  servoInit();
  motorInit();
  uartInitAllCamPeriph();                                                                 // 初始化所有串口
  uart_init(DEBUG_UART_INDEX, DEBUG_UART_BAUDRATE, DEBUG_UART_TX_PIN, DEBUG_UART_RX_PIN); // 串口初始化
  uart_rx_interrupt(UART_1, ZF_ENABLE);                                                   // 打开串口接收中断
  mt9v03x_init();
  imu963ra_init();   // IMU初始化
  zeroPointDetect(); // 零点漂移检测
  pit_ms_init(PIT_CH0, 10);
  ips200_set_dir(IPS200_CROSSWISE);
  ips200_set_font(IPS200_8X16_FONT);
  ips200_set_color(RGB565_RED, RGB565_BLACK);
  ips200_init(IPS200_TYPE_SPI);
  // 此处编写用户代码 例如外设初始化代码等
  ips200_show_string(0, 0, "seekfree");
  system_delay_ms(1000);
  MainMenu_Set();
  // setLeftMotorSpeed(BaseSpeed);                 // 设置电机速度
  gpio_set_level(MotorL_DIR, 0); // 测试电机
  // gpio_set_level(MotorR_DIR, 0);
  pwm_set_duty(MotorL_PWM, 700);
  // pwm_set_duty(MotorR_PWM, 1000);
  // setServoAngle(1000); // 给的是逆时针
  int Threshold;
  float error = 0;
  // 此处编写用户代码 例如外设初始化代码等
  while (1)
  {
    //   ips200_show_string(0, 0, "Encoder1: ");
    //   ips200_show_int(0, 16, encoder_data_R, 4);

    //   ips200_show_string(0, 32, "Encoder2: ");
    //   ips200_show_int(0, 48, encoder_data_B, 4);

    //   ips200_show_string(0, 64, "Encoder3: ");
    //   ips200_show_int(0, 80, encoder_data_L, 4);
    // uart_write_string(DEBUG_UART_INDEX, "FayzGaming Presents\r\n");
    // ips200_show_int(0, 96, FJ_Angle, 4); // 显示陀螺仪角度
    uartOpenMVReceiveData();
    // 此处编写需要循环执行的代码

    // 此处编写需要循环执行的代码
  }
}

void pit_handler(void)
{
  key_IRQHandler();
  if (get_key_msg(&keymsg) == 1)
  {
    BEEP_on();
  }
  else
  {
    BEEP_off();
  }

  encoder_data_R = encoder_get_count(ENCODER_1); // 获取编码器计数
  encoder_data_B = encoder_get_count(ENCODER_2); // 获取编码器计数
  encoder_data_L = encoder_get_count(ENCODER_3); // 获取编码器计数

  // Vofa_data(pwmL, BaseSpeed, 0, 0, 0);     // 发送数据到串口
  // Vofa_data(pwmL, BaseSpeed, encoder_data_L, 0, 0); // 发送数据到串口

  encoder_clear_count(ENCODER_1); // 清空编码器计数
  encoder_clear_count(ENCODER_2); // 清空编码器计数
  encoder_clear_count(ENCODER_3); // 清空编码器计数

  gyroscopeGetData();
  getGyroscopeAngle(); // 获取陀螺仪角度
}

void openmv_rx_handler(void)
{
  // 接收数据
  uart_query_byte(OPENMV_UART_INDEX, &get_data);           // 接收数据
  fifo_write_buffer(&openmv_uart_data_fifo, &get_data, 1); // 将接收到的数据写入FIFO
}