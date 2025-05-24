#include "global.h"

Encoder_Data encoder_data = {
    .encoder_data_R = 0,
    .encoder_data_B = 0,
    .encoder_data_L = 0,
};

Motor_Speed motor_speed = {
    .Left_Speed = 0,
    .Right_Speed = 0,
    .Buttom_Speed = 0,
};

Car_State car_state = STATE_TRACKING;          // 车子状态
Rotate_Direction rotate_direction = CLOCKWISE; // 车子转向