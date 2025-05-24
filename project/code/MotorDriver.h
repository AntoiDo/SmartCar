#ifndef MOTORDRIVER_H
#define MOTORDRIVER_H

#include <zf_common_headfile.h>
#include <math.h>
#define MAX_DUTY (50) // 最大 MAX_DUTY% 占空比

extern int8_t Calculate_Speed_Left;
extern int8_t Calculate_Speed_Right;
extern int8_t Calculate_Speed_Buttom;

// PWM连接的是en口，DIR连的才是PH

// 板子上有两个端口，右边是当做后面轮子的电机，左边是给当做舵机的那个中间轮子的电机
#define MotorL_DIR (C9)
#define MotorL_PWM (PWM2_MODULE1_CHA_C8)

#define MotorR_DIR (C6)
#define MotorR_PWM (PWM2_MODULE0_CHB_C7)

#define MotorServo_DIR (D3)
#define MotorServo_PWM (PWM2_MODULE3_CHA_D2)

void servoInit();
void motorInit();
void setMotorSpeed(int stdSpeed, int speedBias);
void setLeftMotorSpeed(int givenSpeed);
void setRightMotorSpeed(int givenSpeed);
void setServoAngle(int angle);
void setAllMotorSpeed(int left, int right, int servo);

void Brake();
void Kinematic_Analysis(float Vx, float Vy, float V_angle);
#endif