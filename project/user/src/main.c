/*********************************************************************************************************************
 * RT1064DVL6A Opensourec Library ����RT1064DVL6A ��Դ�⣩��һ�����ڹٷ� SDK �ӿڵĵ�������Դ��
 * Copyright (c) 2022 SEEKFREE ��ɿƼ�
 *
 * ���ļ��� RT1064DVL6A ��Դ���һ����
 *
 * RT1064DVL6A ��Դ�� ��������
 * �����Ը��������������ᷢ���� GPL��GNU General Public License���� GNUͨ�ù������֤��������
 * �� GPL �ĵ�3�棨�� GPL3.0������ѡ��ģ��κκ����İ汾�����·�����/���޸���
 *
 * ����Դ��ķ�����ϣ�����ܷ������ã�����δ�������κεı�֤
 * ����û�������������Ի��ʺ��ض���;�ı�֤
 * ����ϸ����μ� GPL
 *
 * ��Ӧ�����յ�����Դ���ͬʱ�յ�һ�� GPL �ĸ���
 * ���û�У������<https://www.gnu.org/licenses/>
 *
 * ����ע����
 * ����Դ��ʹ�� GPL3.0 ��Դ���֤Э�� �����������Ϊ���İ汾
 * �������Ӣ�İ��� libraries/doc �ļ����µ� GPL3_permission_statement.txt �ļ���
 * ���֤������ libraries �ļ����� �����ļ����µ� LICENSE �ļ�
 * ��ӭ��λʹ�ò����������� ���޸�����ʱ���뱣����ɿƼ��İ�Ȩ����������������
 *
 * �ļ�����          main
 * ��˾����          �ɶ���ɿƼ����޹�˾
 * �汾��Ϣ          �鿴 libraries/doc �ļ����� version �ļ� �汾˵��
 * ��������          IAR 8.32.4 or MDK 5.33
 * ����ƽ̨          RT1064DVL6A
 * ��������          https://seekfree.taobao.com/
 *
 * �޸ļ�¼
 * ����              ����                ��ע
 * 2022-09-21        SeekFree            first version
 ********************************************************************************************************************/

#include "zf_common_headfile.h"

// ���µĹ��̻��߹����ƶ���λ�����ִ�����²���
// ��һ�� �ر��������д򿪵��ļ�
// �ڶ��� project->clean  �ȴ��·�����������

// �������ǿ�Դ����ֲ�ÿչ���

int16 encoder_data_R = 0;
int16 encoder_data_B = 0;
int16 encoder_data_L = 0;

#define UART_INDEX (DEBUG_UART_INDEX)
#define UART_BAUDRATE (DEBUG_UART_BAUDRATE)
#define UART_TX_PIN (DEBUG_UART_TX_PIN)
#define UART_RX_PIN (DEBUG_UART_RX_PIN)

void error_handler(void)
{
  if (!Push_State && !Stable_State && !FJ_Angle && !encoder_data_L && !encoder_data_R && !encoder_data_B)
  {
    return;
  }
  ips200_show_string(0, 0, "error");
  while (1)
  {
    ips200_show_string(0, 16, "Push_State");
    ips200_show_int(128, 16, Push_State, 3);
    ips200_show_string(0, 32, "Stable_State");
    ips200_show_int(128, 32, Stable_State, 3);
    ips200_show_string(0, 48, "FJ_Angle");
    ips200_show_int(128, 48, FJ_Angle, 3);
    ips200_show_string(0, 64, "encoder_data_L");
    ips200_show_int(128, 64, encoder_data_L, 3);
    ips200_show_string(0, 80, "encoder_data_R");
    ips200_show_int(128, 80, encoder_data_R, 3);
    ips200_show_string(0, 96, "encoder_data_B");
    ips200_show_int(128, 96, encoder_data_B, 3);
  }
}
/**
 * @brief  ��ʾ��ɫ����ǵ�����
 *
 */
void showRedBlock()
{
  ips200_show_uint(0, 0, od_result[0].res_x1, 3);  // ��ʾ���յ�������
  ips200_show_uint(0, 16, od_result[0].res_y1, 3); // ��ʾ���յ�������
  ips200_show_uint(0, 32, od_result[0].res_x2, 3); // ��ʾ���յ�������
  ips200_show_uint(0, 48, od_result[0].res_y2, 3); // ��ʾ���յ�������
}
/**
 * @brief �������ݵ�����
 * @note ��Ӧ������COM7
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

  uart_write_buffer(UART_INDEX, data, sizeof(float) * 6); // ͨ�����ڴ������ݣ�ǰ�濪�������飬����ռ��󳤶�
  uart_write_buffer(UART_INDEX, tail, 4);                 // ��β�̶������ݰ�β
}

int main(void)
{
  clock_init(SYSTEM_CLOCK_600M); // ����ɾ��
  debug_init();                  // ���Զ˿ڳ�ʼ��
  Key_init(KEY_MAX);
  Push_State = 0; // ��״̬
  Stable_State = 0;
  switch_init();
  BEEP_init();
  encoderInit();
  servoInit();
  motorInit();
  uartInitAllCamPeriph();                                                                 // ��ʼ�����д���
  uart_init(DEBUG_UART_INDEX, DEBUG_UART_BAUDRATE, DEBUG_UART_TX_PIN, DEBUG_UART_RX_PIN); // ���ڳ�ʼ��
  uart_rx_interrupt(UART_1, ZF_ENABLE);                                                   // �򿪴��ڽ����ж�
  mt9v03x_init();
  imu963ra_init();   // IMU��ʼ��
  zeroPointDetect(); // ���Ư�Ƽ��
  ips200_set_dir(IPS200_CROSSWISE);
  ips200_set_font(IPS200_8X16_FONT);
  ips200_set_color(RGB565_RED, RGB565_BLACK);
  ips200_init(IPS200_TYPE_SPI);
  ips200_show_string(0, 0, "seekfree");
  pit_ms_init(PIT_CH0, 10);
  // �˴���д�û����� ���������ʼ�������
  system_delay_ms(1000);
  MainMenu_Set();
  error_handler();
  // �˴���д�û����� ���������ʼ�������
  while (1)
  {
    detectRedBlock();
    detectBlockStable();
    if (Push_State && !Stable_State) // Ҫ�Ʋ��Ҳ��ȶ�
    {
      Kinematic_Analysis(getCenterOffset_XAxis(), getCenterOffset_YAxis(), 0);
    }
    if (Stable_State)
    {
      Brake();
      system_delay_ms(1000);
      while (abs(FJ_Angle) < 85)
      {
        int reverse_power = PID_B(100, encoder_data_B);
        setServoAngle(reverse_power);
      }
    }
    ips200_show_int(0, 0, Push_State, 3);               // ��ʾ���յ�������
    ips200_show_int(0, 16, Stable_State, 3);            // ��ʾ���յ�������
    ips200_show_int(0, 32, getCenterOffset_XAxis(), 3); // ��ʾ���յ�������
    ips200_show_int(0, 48, getCenterOffset_YAxis(), 3); // ��ʾ���յ�������

    ips200_show_uint(0, 64, od_result[0].res_x1, 3);                                                                                // ��ʾ���յ�������
    ips200_show_uint(0, 80, od_result[0].res_y1, 3);                                                                                // ��ʾ���յ�������
    ips200_show_uint(0, 96, od_result[0].res_x2, 3);                                                                                // ��ʾ���յ�������
    ips200_show_uint(0, 112, od_result[0].res_y2, 3);                                                                               // ��ʾ���յ�������
    Vofa_data(Calculate_Speed_Left, Calculate_Speed_Right, Calculate_Speed_Buttom, encoder_data_L, encoder_data_R, encoder_data_B); // �������ݵ�����
    ips200_show_int(0, 128, FJ_Angle, 3);

    // �˴���д��Ҫѭ��ִ�еĴ���

    // �˴���д��Ҫѭ��ִ�еĴ���
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

  encoder_data_R = encoder_get_count(ENCODER_1);  // ��ȡ����������
  encoder_data_B = encoder_get_count(ENCODER_2);  // ��ȡ����������
  encoder_data_L = -encoder_get_count(ENCODER_3); // ��ȡ����������   ��֤��ǰ�ߵ�����Ϊ��ֵ
  if (Push_State && !Stable_State)
  {
    int Left_Speed = PID_L(Calculate_Speed_Left * 2, encoder_data_L);
    int Right_Speed = PID_R(Calculate_Speed_Right * 2, encoder_data_R);
    int Buttom_Speed = PID_B(Calculate_Speed_Buttom * 2, encoder_data_B);
    setLeftMotorSpeed(Left_Speed);
    setRightMotorSpeed(Right_Speed);
    setServoAngle(Buttom_Speed);
  }
  encoder_clear_count(ENCODER_1); // ��ձ���������
  encoder_clear_count(ENCODER_2); // ��ձ���������
  encoder_clear_count(ENCODER_3); // ��ձ���������
  if (Push_State && Stable_State)
  {
    gyroscopeGetData();
    getGyroscopeAngle(); // ��ȡ�����ǽǶ�
  }
}

void openmv_rx_handler(void)
{
  // ��������
  uint8 get_data = 0; // �������ݱ���
  uint32 temp_length = 0;
  uint8 od_num = 0;
  uart_query_byte(OPENMV_UART_INDEX, &get_data);
  {
    fifo_write_buffer(&openmv_uart_data_fifo, &get_data, 1);
  }

  if (102 == get_data)
  {
    // ��ȡ��1�����ݣ������ж�֡ͷ��ʹ�������������
    temp_length = 1;
    fifo_read_buffer(&openmv_uart_data_fifo, fifo_get_data, &temp_length, FIFO_READ_AND_CLEAN);
    if (101 == fifo_get_data[0])
    {
      // ��ȡ8�����ݣ����ڻ�ȡĿ�����ݣ�Ȼ��ת�Ƶ��ṹ��������
      temp_length = 4;
      fifo_read_buffer(&openmv_uart_data_fifo, fifo_get_data, &temp_length, FIFO_READ_AND_CLEAN);
      memcpy((uint8 *)(&od_result[od_num]), fifo_get_data, 4);
    }
    fifo_clear(&openmv_uart_data_fifo);
  }
}