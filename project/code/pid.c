#include "pid.h"

float Island_P = 1.46;
float Island_D = 4.5; // 3.7

float P_L = 20;
float I_L = 0; // 1.6
float P_R = 1.5;
float I_R = 0; // 1.6

static float cameraError;

// 加权控制
const uint8 Weight[MT9V03X_H] = {
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1,        // 图像最远端00 ——09 行权重
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1,        // 图像最远端10 ——19 行权重
    1, 1, 1, 1, 1, 1, 1, 3, 4, 5,        // 图像最远端20 ——29 行权重
    6, 7, 9, 11, 13, 15, 17, 19, 20, 20, // 图像最远端30 ——39 行权重
    19, 17, 15, 13, 11, 9, 7, 5, 3, 1,   // 图像最远端40 ——49 行权重
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1,        // 图像最远端60 ——69 行权重
};
/**
 * @brief   从摄像头得到提供给舵机的中线误差
 * @param    none
 * @retval   none
 * @note     使用pd
 */
float cameraErrorSum(void)
{
    int i;
    float err = 0;
    float weight_count = 0;
    // 常规误差
    for (i = MT9V03X_H - 1; i >= MT9V03X_H - Search_Stop_Line - 1; i--) // 常规误差计算
    {
        err += (MT9V03X_W / 2 - ((Left_Line[i] + Right_Line[i]) >> 1)) * Weight[i]; // 右移1位，等效除2
        weight_count += Weight[i];
    }
    err = err / weight_count;
    cameraError = err;
    return cameraError;
}

/**
 * @brief    通过摄像头pd得到提供给舵机的pwm值
 * @param    cameraErrorSum得到的摄像头误差
 * @return   舵机打角值
 * @note     舵机使用的是位置式pd控制，先用纯pd控制，之后再加新东西。还有我们默认的expectedError应该是0
 * @note     位置pd公式: u(k) = Kp * err[k] + Kd * (err[k]-err[k-1])
 */
int PD_Camera(float error)
{
    volatile static float Current_Error, Prev_Error;

    volatile float Servo_P = 1.67;
    volatile float Servo_D = 8.25; // 1.632  0.75     8.25

    float ek, ek1; // e(k), e(k-1)

    float u;
    Current_Error = error - 0; // 这里的0是我们想要的expectError，可能以后要改
    ek = Current_Error;
    ek1 = Current_Error - Prev_Error;
    if (Island_State != 0) // 环岛单独的pd
    {
        Servo_P = Island_P;
        Servo_D = Island_D;
    }
    u = Servo_P * ek + Servo_D * ek1;
    // 更新误差
    Prev_Error = Current_Error;
    // 限幅处理
    //    if (u >= Left_Max)
    //        u = Left_Max;
    //    if (u <= Right_Max)
    //        u = Right_Max;
    return (int)u;
}

/**
 * @brief   增量式pi控制左电机速度
 * @param   期望速度值
 * @param   实际速度值
 * @return  输出给左电机的pwm值
 * @note    控制器输出:u(k) = u(k-1) + δu(k);
 * @note    增量计算:δu(k) = Kp * (e(k) - e(k-1)) + Ki * e(k)
 */

int PID_L(int setSpeed, int realSpeed)
{
    volatile static int out;
    volatile static int out_increment;
    volatile static int ek, ek1, lastek; // e(k), e(k-1)

    // 空转时候kp=1.5，ki=0.1的时候效果很好
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
 * @brief   增量式pi控制右电机速度
 * @param   期望速度值
 * @param   实际速度值
 * @return  输出给右电机的pwm值
 * @note    控制器输出:u(k) = u(k-1) + δu(k);
 * @note    增量计算:δu(k) = Kp * (e(k) - e(k-1)) + Ki * e(k)
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
