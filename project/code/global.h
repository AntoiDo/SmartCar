#ifndef GLOBAL_H
#define GLOBAL_H

#include "zf_common_headfile.h"

typedef struct
{
    int16 encoder_data_R;
    int16 encoder_data_B;
    int16 encoder_data_L;
} Encoder_Data;

typedef struct
{
    int Left_Speed;
    int Right_Speed;
    int Buttom_Speed;
} Motor_Speed;

typedef enum
{
    CLOCKWISE,        // 顺时针
    COUNTERCLOCKWISE, // 逆时针
    DEFAULT           // 没检测到色块就是默认
} Rotate_Direction;

typedef enum
{
    STATE_TRACKING,     // 正常循迹模式
    STATE_COLOR_DETECT, // 识别色块模式
    STATE_SIDEWAY,      // 平移靠近色块
    STATE_ROTATE,       // 绕色块旋转90度
    STATE_PUSH,         // 推色块出赛道
    STATE_RECOVER       // 返回循迹的过渡状态
} Car_State;

typedef struct
{
    int last_count;    // 上一次的编码器计数
    int current_count; // 当前的编码器计数
} Encoder_Integration;

extern Encoder_Data encoder_data;
extern Motor_Speed motor_speed;
extern Car_State car_state;
extern Rotate_Direction rotate_direction;
extern Encoder_Integration encoder_integration;

#endif