/*
 * Gyro.c
 *
 *  Created on: 2024年10月5日
 *      Author: cjhdd
 */
#include "Gyro.h"

int ZeroDrift_gyro_y = 0;
int ZeroDrift_gyro_z = 0;

int16 FJ_gyro_z = 0, FJ_gyro_y = 0;
volatile float FJ_Angle = 0; // 最后引出的变量
volatile float FJ_Pitch = 0; // 最后引出的变量
volatile float FJ_PitchSpeed = 0, FJ_LastPitchSpeed = 0;
volatile float FJ_LastAngleSpeed = 0, FJ_AngleSpeed = 0;
volatile float FJ_Angle_Max = 360;
volatile float FJ_Angle_Min = -360;
volatile float AngleSpeed = 0;

void zeroPointDetect()
{
    uint8 i;
    int zero_point_y_accu = 0;
    int zero_point_z_accu = 0;
    for (i = 0; i <= 100; i++) // 积累100次，求取平均值，获取当前零飘
    {
        imu963ra_get_gyro();
        zero_point_y_accu += imu963ra_gyro_y;
        zero_point_z_accu += imu963ra_gyro_z;
        system_delay_ms(3); // 记得换成本系统的delay
    }
    ZeroDrift_gyro_y = zero_point_y_accu / 100.0;
    ZeroDrift_gyro_z = zero_point_z_accu / 100.0;
}
void gyroscopeGetData()
{
    int16 gyro_z = 0;
    imu963ra_get_gyro();
    gyro_z = imu963ra_gyro_z;
    AngleSpeed = ((gyro_z - ZeroDrift_gyro_z) * GYRO_SENS) * DT;
}
void getGyroscopeAngle()
{
    float K = 0.8;
    FJ_gyro_z = imu963ra_gyro_z;
    FJ_LastAngleSpeed = FJ_AngleSpeed;
    FJ_AngleSpeed += ((FJ_gyro_z - ZeroDrift_gyro_z) * GYRO_SENS) * DT;
    FJ_Angle = FJ_AngleSpeed * K + FJ_LastAngleSpeed * (1 - K); // 向左为正
    FJ_Angle = FJ_Angle > FJ_Angle_Max ? FJ_Angle_Max : FJ_Angle;
    FJ_Angle = FJ_Angle < FJ_Angle_Min ? FJ_Angle_Min : FJ_Angle;
    FJ_Angle = -FJ_Angle;
}
void getGyroscopePitch()
{
    float K = 0.7;
    FJ_LastPitchSpeed = FJ_PitchSpeed;
    FJ_gyro_y = imu963ra_gyro_y;
    FJ_PitchSpeed += ((FJ_gyro_y - ZeroDrift_gyro_y) * GYRO_SENS) * DT;
    FJ_Pitch = FJ_PitchSpeed * K + FJ_LastPitchSpeed * (1 - K);

    FJ_Pitch = FJ_Pitch > 40 ? 40 : FJ_Pitch;
    FJ_Pitch = FJ_Pitch < (-40) ? (-40) : FJ_Pitch;

    if (FJ_Pitch >= 5 && Ramp_Flag == 0)
    { // 俯仰角变大认为是坡道，强行进1状态，但是注意，陀螺仪零飘会导致俯仰角自己在飘，会莫名其妙的进去坡道
        Ramp_Flag = 1;
    }
}
void clearGyroscopePitch()
{
    FJ_Pitch = 0;
    FJ_gyro_y = 0;
    FJ_PitchSpeed = 0;
    FJ_LastPitchSpeed = 0;
}
void clearGyroscopeAngle()
{
    FJ_Angle = 0;
    FJ_gyro_z = 0;
    FJ_AngleSpeed = 0;
    FJ_LastAngleSpeed = 0;
}
