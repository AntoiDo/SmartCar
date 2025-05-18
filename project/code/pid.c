#include "pid.h"

float Island_P = 1.46;
float Island_D = 4.5; // 3.7

float P_L = 20;
float I_L = 0; // 1.6
float P_R = 1.5;
float I_R = 0; // 1.6

static float cameraError;

// ��Ȩ����
const uint8 Weight[MT9V03X_H] = {
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1,        // ͼ����Զ��00 ����09 ��Ȩ��
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1,        // ͼ����Զ��10 ����19 ��Ȩ��
    1, 1, 1, 1, 1, 1, 1, 3, 4, 5,        // ͼ����Զ��20 ����29 ��Ȩ��
    6, 7, 9, 11, 13, 15, 17, 19, 20, 20, // ͼ����Զ��30 ����39 ��Ȩ��
    19, 17, 15, 13, 11, 9, 7, 5, 3, 1,   // ͼ����Զ��40 ����49 ��Ȩ��
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1,        // ͼ����Զ��60 ����69 ��Ȩ��
};
/**
 * @brief   ������ͷ�õ��ṩ��������������
 * @param    none
 * @retval   none
 * @note     ʹ��pd
 */
float cameraErrorSum(void)
{
    int i;
    float err = 0;
    float weight_count = 0;
    // �������
    for (i = MT9V03X_H - 1; i >= MT9V03X_H - Search_Stop_Line - 1; i--) // ����������
    {
        err += (MT9V03X_W / 2 - ((Left_Line[i] + Right_Line[i]) >> 1)) * Weight[i]; // ����1λ����Ч��2
        weight_count += Weight[i];
    }
    err = err / weight_count;
    cameraError = err;
    return cameraError;
}

/**
 * @brief    ͨ������ͷpd�õ��ṩ�������pwmֵ
 * @param    cameraErrorSum�õ�������ͷ���
 * @return   ������ֵ
 * @note     ���ʹ�õ���λ��ʽpd���ƣ����ô�pd���ƣ�֮���ټ��¶�������������Ĭ�ϵ�expectedErrorӦ����0
 * @note     λ��pd��ʽ: u(k) = Kp * err[k] + Kd * (err[k]-err[k-1])
 */
int PD_Camera(float error)
{
    volatile static float Current_Error, Prev_Error;

    volatile float Servo_P = 1.67;
    volatile float Servo_D = 8.25; // 1.632  0.75     8.25

    float ek, ek1; // e(k), e(k-1)

    float u;
    Current_Error = error - 0; // �����0��������Ҫ��expectError�������Ժ�Ҫ��
    ek = Current_Error;
    ek1 = Current_Error - Prev_Error;
    if (Island_State != 0) // ����������pd
    {
        Servo_P = Island_P;
        Servo_D = Island_D;
    }
    u = Servo_P * ek + Servo_D * ek1;
    // �������
    Prev_Error = Current_Error;
    // �޷�����
    //    if (u >= Left_Max)
    //        u = Left_Max;
    //    if (u <= Right_Max)
    //        u = Right_Max;
    return (int)u;
}

/**
 * @brief   ����ʽpi���������ٶ�
 * @param   �����ٶ�ֵ
 * @param   ʵ���ٶ�ֵ
 * @return  �����������pwmֵ
 * @note    ���������:u(k) = u(k-1) + ��u(k);
 * @note    ��������:��u(k) = Kp * (e(k) - e(k-1)) + Ki * e(k)
 */

int PID_L(int setSpeed, int realSpeed)
{
    volatile static int out;
    volatile static int out_increment;
    volatile static int ek, ek1, lastek; // e(k), e(k-1)

    // ��תʱ��kp=1.5��ki=0.1��ʱ��Ч���ܺ�
    float kp = 60;
    float ki = 0.6;
    float kd = 1;
    lastek = ek1;
    ek1 = ek;
    ek = setSpeed - realSpeed;
    out_increment = (int)(kp * (ek - ek1) + ki * ek + kd * (ek - 2 * ek1 + lastek));
    out += out_increment;

    if (out >= SPEED_MAX) // ????
        out = SPEED_MAX;
    else if (out <= SPEED_MIN)
        out = SPEED_MIN;
    return (int)out;
}

/**
 * @brief   ����ʽpi�����ҵ���ٶ�
 * @param   �����ٶ�ֵ
 * @param   ʵ���ٶ�ֵ
 * @return  ������ҵ����pwmֵ
 * @note    ���������:u(k) = u(k-1) + ��u(k);
 * @note    ��������:��u(k) = Kp * (e(k) - e(k-1)) + Ki * e(k)
 */

int PID_R(int setSpeed, int realSpeed)
{
    volatile static int out;
    volatile static int out_increment;
    volatile static int ek, ek1, lastek; // e(k), e(k-1)

    float kp = 60;
    float ki = 0.6;
    float kd = 1;
    lastek = ek1;
    ek1 = ek;
    ek = setSpeed - realSpeed;
    out_increment = (int)(kp * (ek - ek1) + ki * ek + kd * (ek - 2 * ek1 + lastek));
    out += out_increment;

    if (out >= SPEED_MAX) // ????
        out = SPEED_MAX;
    else if (out <= SPEED_MIN)
        out = SPEED_MIN;
    return (int)out;
}

int PID_B(int setSpeed, int realSpeed)
{
    volatile static int out;
    volatile static int out_increment;
    volatile static int ek, ek1, lastek; // e(k), e(k-1)
    // KP=50, KI=1.1
    float kp = 50;
    float ki = 1.1;
    float kd = 1;
    lastek = ek1;
    ek1 = ek;
    ek = setSpeed - realSpeed;
    out_increment = (int)(kp * (ek - ek1) + ki * ek + kd * (ek - 2 * ek1 + lastek));
    out += out_increment;

    if (out >= SPEED_MAX) // ????
        out = SPEED_MAX;
    else if (out <= SPEED_MIN)
        out = SPEED_MIN;
    return (int)out;
}
