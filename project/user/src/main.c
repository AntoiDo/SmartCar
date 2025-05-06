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
int Lspeed, Bspeed, Rspeed;

uint8 pwmL;
uint8 pwmR;
uint16 BaseSpeed = 100;
#define UART_INDEX (DEBUG_UART_INDEX)
#define UART_BAUDRATE (DEBUG_UART_BAUDRATE)
#define UART_TX_PIN (DEBUG_UART_TX_PIN)
#define UART_RX_PIN (DEBUG_UART_RX_PIN)

/**
 * @brief �������ݵ�����
 * @note ��Ӧ������COM7
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

  uart_write_buffer(UART_INDEX, data, sizeof(float) * 5); // ͨ�����ڴ������ݣ�ǰ�濪�������飬����ռ��󳤶�
  uart_write_buffer(UART_INDEX, tail, 4);                 // ��β�̶������ݰ�β
}

int main(void)
{
  clock_init(SYSTEM_CLOCK_600M); // ����ɾ��
  debug_init();                  // ���Զ˿ڳ�ʼ��
  Key_init(KEY_MAX);
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
  pit_ms_init(PIT_CH0, 10);
  ips200_set_dir(IPS200_CROSSWISE);
  ips200_set_font(IPS200_8X16_FONT);
  ips200_set_color(RGB565_RED, RGB565_BLACK);
  ips200_init(IPS200_TYPE_SPI);
  // �˴���д�û����� ���������ʼ�������
  ips200_show_string(0, 0, "seekfree");
  system_delay_ms(1000);
  MainMenu_Set();
  // setLeftMotorSpeed(BaseSpeed);                 // ���õ���ٶ�
  gpio_set_level(MotorL_DIR, 0); // ���Ե��
  // gpio_set_level(MotorR_DIR, 0);
  pwm_set_duty(MotorL_PWM, 700);
  // pwm_set_duty(MotorR_PWM, 1000);
  // setServoAngle(1000); // ��������ʱ��
  int Threshold;
  float error = 0;
  // �˴���д�û����� ���������ʼ�������
  while (1)
  {
    //   ips200_show_string(0, 0, "Encoder1: ");
    //   ips200_show_int(0, 16, encoder_data_R, 4);

    //   ips200_show_string(0, 32, "Encoder2: ");
    //   ips200_show_int(0, 48, encoder_data_B, 4);

    //   ips200_show_string(0, 64, "Encoder3: ");
    //   ips200_show_int(0, 80, encoder_data_L, 4);
    // uart_write_string(DEBUG_UART_INDEX, "FayzGaming Presents\r\n");
    // ips200_show_int(0, 96, FJ_Angle, 4); // ��ʾ�����ǽǶ�
    uartOpenMVReceiveData();
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

  encoder_data_R = encoder_get_count(ENCODER_1); // ��ȡ����������
  encoder_data_B = encoder_get_count(ENCODER_2); // ��ȡ����������
  encoder_data_L = encoder_get_count(ENCODER_3); // ��ȡ����������

  // Vofa_data(pwmL, BaseSpeed, 0, 0, 0);     // �������ݵ�����
  // Vofa_data(pwmL, BaseSpeed, encoder_data_L, 0, 0); // �������ݵ�����

  encoder_clear_count(ENCODER_1); // ��ձ���������
  encoder_clear_count(ENCODER_2); // ��ձ���������
  encoder_clear_count(ENCODER_3); // ��ձ���������

  gyroscopeGetData();
  getGyroscopeAngle(); // ��ȡ�����ǽǶ�
}

void openmv_rx_handler(void)
{
  // ��������
  uart_query_byte(OPENMV_UART_INDEX, &get_data);           // ��������
  fifo_write_buffer(&openmv_uart_data_fifo, &get_data, 1); // �����յ�������д��FIFO
}