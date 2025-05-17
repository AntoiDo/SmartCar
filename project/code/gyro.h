#ifndef GYRO_H
#define GYRO_H

#include "zf_common_headfile.h"

/* 灵敏度 */
#define GYRO_SENS (1 * 1 / 16.4) // ≈0.3049 °/s/LSB
#define ACCE_SENS (90.0 / 4096)
#define DT (0.01)

extern volatile float FJ_Pitch; // 偏航角
extern volatile float FJ_Angle; // 俯仰角

void zeroPointDetect();
void gyroscopeGetData();
void getGyroscopeAngle();
void getGyroscopePitch();
void clearGyroscopePitch();
void clearGyroscopeAngle();

#endif