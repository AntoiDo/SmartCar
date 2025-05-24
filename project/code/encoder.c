#include "encoder.h"

void encoderInit()
{
    encoder_quad_init(ENCODER_1, ENCODER_1_A, ENCODER_1_B); // 初始化编码器模块与引脚 正交解码编码器模式
    encoder_quad_init(ENCODER_2, ENCODER_2_A, ENCODER_2_B); // 初始化编码器模块与引脚 正交解码编码器模式
    encoder_quad_init(ENCODER_3, ENCODER_3_A, ENCODER_3_B); // 初始化编码器模块与引脚 正交解码编码器模式
}

void clearAllEncoderCount()
{
    encoder_clear_count(ENCODER_1); // 清空编码器计数
    encoder_clear_count(ENCODER_2); // 清空编码器计数
    encoder_clear_count(ENCODER_3); // 清空编码器计数
}

void getAllEncoderCount()
{
    encoder_data.encoder_data_R = encoder_get_count(ENCODER_1);  // 获取编码器计数
    encoder_data.encoder_data_B = encoder_get_count(ENCODER_2);  // 获取编码器计数
    encoder_data.encoder_data_L = -encoder_get_count(ENCODER_3); // 获取编码器计数   保证向前走的轮子为正值
}