#include "encoder.h"

void encoderInit()
{
    encoder_quad_init(ENCODER_1, ENCODER_1_A, ENCODER_1_B); // 初始化编码器模块与引脚 正交解码编码器模式
    encoder_quad_init(ENCODER_2, ENCODER_2_A, ENCODER_2_B); // 初始化编码器模块与引脚 正交解码编码器模式
    encoder_quad_init(ENCODER_3, ENCODER_3_A, ENCODER_3_B); // 初始化编码器模块与引脚 正交解码编码器模式
}
