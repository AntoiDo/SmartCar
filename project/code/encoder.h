#ifndef ENCODER_H
#define ENCODER_H

#include "zf_common_headfile.h"

#define ENCODER_1 (QTIMER2_ENCODER1)
#define ENCODER_1_A (QTIMER2_ENCODER1_CH1_C3)
#define ENCODER_1_B (QTIMER2_ENCODER1_CH2_C4)

#define ENCODER_2 (QTIMER1_ENCODER1)
#define ENCODER_2_A (QTIMER1_ENCODER1_CH1_C0)
#define ENCODER_2_B (QTIMER1_ENCODER1_CH2_C1)

#define ENCODER_3 (QTIMER2_ENCODER2)
#define ENCODER_3_A (QTIMER2_ENCODER2_CH1_C5)
#define ENCODER_3_B (QTIMER2_ENCODER2_CH2_C25)

void encoderInit();
void clearAllEncoderCount();
void getAllEncoderCount();
#endif