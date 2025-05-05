
#ifndef PID_H_
#define PID_H_

#include "zf_common_headfile.h"
float cameraErrorSum();
int PD_Camera(float error);

#define SPEED_MAX 4000
#define SPEED_MIN -4000

int PID_L(int setSpeed, int realSpeed);
int PID_R(int setSpeed, int realSpeed);
int PID_B(int setSpeed, int realSpeed);

#endif /* CODE_PID_H_ */
