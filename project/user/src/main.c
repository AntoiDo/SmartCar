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

int16 cameraErr = 0;
int Push_Count = 0;
int BaseSpeed = 200;
int COLOR_CHANGE = 0;
#define UART_INDEX (DEBUG_UART_INDEX)
#define UART_BAUDRATE (DEBUG_UART_BAUDRATE)
#define UART_TX_PIN (DEBUG_UART_TX_PIN)
#define UART_RX_PIN (DEBUG_UART_RX_PIN)

void error_handler(void)
{
  if (car_state == STATE_TRACKING &&
      !FJ_Angle &&
      !encoder_data.encoder_data_L &&
      !encoder_data.encoder_data_R &&
      !encoder_data.encoder_data_B)
  {
    return;
  }
  ips200_show_string(0, 0, "error");
  while (1)
  {
    ips200_show_string(0, 0, "car_state");
    ips200_show_int(128, 0, car_state, 3);
    ips200_show_string(0, 48, "FJ_Angle");
    ips200_show_int(128, 48, FJ_Angle, 3);
    ips200_show_string(0, 64, "encoder_data_L");
    ips200_show_int(128, 64, encoder_data.encoder_data_L, 3);
    ips200_show_string(0, 80, "encoder_data_R");
    ips200_show_int(128, 80, encoder_data.encoder_data_R, 3);
    ips200_show_string(0, 96, "encoder_data_B");
    ips200_show_int(128, 96, encoder_data.encoder_data_B, 3);
  }
}
/**
 * @brief  显示红色方块角点坐标
 *
 */
void showRedBlock()
{
  ips200_show_uint(0, 0, od_result[0].res_x1, 3);  // 显示接收到的数据
  ips200_show_uint(0, 16, od_result[0].res_y1, 3); // 显示接收到的数据
  ips200_show_uint(0, 32, od_result[0].res_x2, 3); // 显示接收到的数据
  ips200_show_uint(0, 48, od_result[0].res_y2, 3); // 显示接收到的数据
}
/**
 * @brief 发送数据到串口
 * @note 对应串口是COM7
 */
void Vofa_data(int data1, int data2, int data3, int data4, int data5, int data6)
{
  float data[6];
  uint8 tail[4] = {0x00, 0x00, 0x80, 0x7f};
  data[0] = (float)data1;
  data[1] = (float)data2;
  data[2] = (float)data3;
  data[3] = (float)data4;
  data[4] = (float)data5;
  data[5] = (float)data6;

  uart_write_buffer(UART_INDEX, data, sizeof(float) * 6); // 通过串口传输数据，前面开多大的数组，后面占多大长度
  uart_write_buffer(UART_INDEX, tail, 4);                 // 结尾固定的数据包尾
}
int threshold = 0;
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
  ips200_set_dir(IPS200_CROSSWISE);
  ips200_set_font(IPS200_8X16_FONT);
  ips200_set_color(RGB565_RED, RGB565_BLACK);
  ips200_init(IPS200_TYPE_SPI);
  ips200_show_string(0, 0, "seekfree");
  pit_ms_init(PIT_CH0, 10);
  // 此处编写用户代码 例如外设初始化代码等
  system_delay_ms(1000);
  MainMenu_Set();
  error_handler();
  // 此处编写用户代码 例如外设初始化代码等
  while (1)
  {
    // Vofa_data(car_state, FJ_Angle, 0, 0, 0, 0);
    // detectRedBlock(); // 检测红色方块
    if (car_state == STATE_TRACKING)
    {
      if (mt9v03x_finish_flag)
      {
        threshold = otsuThreshold(mt9v03x_image[0], MT9V03X_W, MT9V03X_H);
        setImageTwoValue(threshold); // 二值化
        mt9v03x_finish_flag = 0;     // 清除标志位
      }
      longestWhiteColumnSearchLines(); // 识别白线
      crossDetect();                   // 识别十字路口
      islandDetect();
      cameraErr = PD_Camera(cameraErrorSum());
    }
    else if (car_state == STATE_COLOR_DETECT)
    {
      Kinematic_Analysis(getCenterOffset_XAxis(), getCenterOffset_YAxis(), 0);
      detectBlockStable();
      // if (++COLOR_CHANGE > 200)
      // {
      //   car_state = STATE_ROTATE;
      // }
    }
    else if (car_state == STATE_ROTATE)
    {
      COLOR_CHANGE = 0;
      Brake();
      system_delay_ms(1000);
      while (abs(FJ_Angle) < 85)
      {
        continue;
      }
      car_state = STATE_PUSH;
      clearGyroscopeAngle();
      system_delay_ms(500);
    }
    else if (car_state == STATE_PUSH)
    {
      while (Push_Count < 50)
      {
        Push_Count++;
      }
      Brake();
      car_state = STATE_RECOVER;
    }
    else if (car_state == STATE_RECOVER)
    {
      while (FJ_Angle != 0)
      {
        continue;
      }
    }
    // judgeCarStrategy();
    // ips200_show_int(0, 0, Push_State, 3);               // 显示接收到的数据
    // ips200_show_int(0, 16, Stable_State, 3);            // 显示接收到的数据
    // ips200_show_int(0, 32, getCenterOffset_XAxis(), 3); // 显示接收到的数据
    // ips200_show_int(0, 48, getCenterOffset_YAxis(), 3); // 显示接收到的数据

    // ips200_show_uint(0, 64, od_result[0].res_x1, 3);                                                                                // 显示接收到的数据
    // ips200_show_uint(0, 80, od_result[0].res_y1, 3);                                                                                // 显示接收到的数据
    // ips200_show_uint(0, 96, od_result[0].res_x2, 3);                                                                                // 显示接收到的数据
    // ips200_show_uint(0, 112, od_result[0].res_y2, 3);                                                                               // 显示接收到的数据
    // ips200_show_int(0, 128, FJ_Angle, 3);
    ips200_show_int(0, 144, cameraErr, 3);
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

  getAllEncoderCount();
  if (car_state == STATE_TRACKING)
  {
    motor_speed.Left_Speed = PID_L(BaseSpeed - cameraErr, encoder_data.encoder_data_L);
    motor_speed.Right_Speed = PID_R(BaseSpeed + cameraErr, encoder_data.encoder_data_R);
  }
  else if (car_state == STATE_COLOR_DETECT)
  {
    motor_speed.Left_Speed = PID_L(Calculate_Speed_Left * 2, encoder_data.encoder_data_L);
    motor_speed.Right_Speed = PID_R(Calculate_Speed_Right * 2, encoder_data.encoder_data_R);
    motor_speed.Buttom_Speed = PID_B(Calculate_Speed_Buttom * 2, encoder_data.encoder_data_B);
  }
  else if (car_state == STATE_ROTATE || car_state == STATE_PUSH)
  {
    gyroscopeGetData();
    getGyroscopeAngle(); // 获取陀螺仪角度
    if (car_state == STATE_ROTATE)
    {
      motor_speed.Left_Speed = 0;
      motor_speed.Right_Speed = 0;
      motor_speed.Buttom_Speed = PID_B(BaseSpeed, encoder_data.encoder_data_B);
    }
    else
    {
      motor_speed.Left_Speed = PID_L(BaseSpeed, encoder_data.encoder_data_L);
      motor_speed.Right_Speed = PID_R(BaseSpeed, encoder_data.encoder_data_R);
      motor_speed.Buttom_Speed = 0;
    }
  }
  else if (car_state == STATE_RECOVER)
  {
    gyroscopeGetData();
    getGyroscopeAngle(); // 获取陀螺仪角度
    motor_speed.Left_Speed = PID_L(-BaseSpeed, encoder_data.encoder_data_L);
    motor_speed.Right_Speed = PID_R(-BaseSpeed, encoder_data.encoder_data_R);
    motor_speed.Buttom_Speed = PID_B(-BaseSpeed, encoder_data.encoder_data_B);
  }
  setAllMotorSpeed(motor_speed.Left_Speed, motor_speed.Right_Speed, motor_speed.Buttom_Speed); // 设置电机速度
  // Vofa_data(motor_speed.Left_Speed, motor_speed.Right_Speed, encoder_data.encoder_data_L, encoder_data.encoder_data_R, cameraErr, 0); // 发送数据到串口
  clearAllEncoderCount();
}

void openmv_rx_handler(void)
{
  // 接收数据
  uint8 get_data = 0; // 接收数据变量
  uint32 temp_length = 0;
  uint8 od_num = 0;
  uart_query_byte(OPENMV_UART_INDEX, &get_data);
  {
    fifo_write_buffer(&openmv_uart_data_fifo, &get_data, 1);
  }

  if (102 == get_data)
  {
    // 读取第1个数据，用于判断帧头，使用完清除此数据
    temp_length = 1;
    fifo_read_buffer(&openmv_uart_data_fifo, fifo_get_data, &temp_length, FIFO_READ_AND_CLEAN);
    if (101 == fifo_get_data[0])
    {
      // 读取8个数据，用于获取目标数据，然后转移到结构体数组中
      temp_length = 4;
      fifo_read_buffer(&openmv_uart_data_fifo, fifo_get_data, &temp_length, FIFO_READ_AND_CLEAN);
      memcpy((uint8 *)(&od_result[od_num]), fifo_get_data, 4);
    }
    fifo_clear(&openmv_uart_data_fifo);
  }
}