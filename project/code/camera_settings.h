
#ifndef CAMERA_SETTINGS_H_
#define CAMERA_SETTINGS_H_

#include "zf_common_headfile.h"
typedef struct
{
    uint32 INIT;
    uint32 AUTO_EXP;
    uint32 EXP_TIME;
    uint32 FPS;
    uint32 GAIN;
    uint32 SET_COL;
    uint32 SET_ROW;
    uint32 LR_OFFSET; // 左右偏移
    uint32 UD_OFFSET; // 向下偏移
} CAMERA_DEBUG_VAR;

void cameraDebugVarInit();
void cameraVarInitFromEeprom();

// 灰度图展示
void myShowGrayImage();

// 二值化图展示
void myShowBinaryImage();

// 压缩图展示
void myShowCompressImage();

extern CAMERA_DEBUG_VAR cameraDebugVar;
#endif /* CODE_CAMERA_SETTING_H_ */
