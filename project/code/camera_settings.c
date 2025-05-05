/*
 * camera_setting.c
 *
 *  Created on: 2022年7月25日
 *      Author: Samurai
 */

#include "camera_settings.h"

// 初始化一个全局的11行2列二维数组，用于在eeprom当中存储和调节摄像头各个参数
extern int16 mt9v03x_set_config_buffer[10][2] =
    {
        {MT9V03X_INIT, 0},       // 摄像头初始化   0
        {MT9V03X_AUTO_EXP, 0},   // 自动曝光设置   默认为0关闭
        {MT9V03X_EXP_TIME, 512}, // 曝光时间      摄像头收到后会自动计算出最大曝光时间，如果设置过大则设置为计算出来的最大曝光值
        {MT9V03X_FPS, 50},       // 图像帧率      摄像头收到后会自动计算出最大FPS，如果过大则设置为计算出来的最大FPS
        {MT9V03X_SET_COL, 188},  // 图像列数量    范围1-752
        {MT9V03X_SET_ROW, 120},  // 图像行数量    范围1-480
        {MT9V03X_LR_OFFSET, 0},  // 图像左右偏移量  正值 右偏移   负值 左偏移  列为188 376 752时无法设置偏移    摄像头收偏移数据后会自动计算最大偏移，如果超出则设置计算出来的最大偏移
        {MT9V03X_UD_OFFSET, 0},  // 图像上下偏移量  正值 上偏移   负值 下偏移  行为120 240 480时无法设置偏移    摄像头收偏移数据后会自动计算最大偏移，如果超出则设置计算出来的最大偏移
        {MT9V03X_GAIN, 32},      // 图像增益      范围16-64     增益可以在曝光时间固定的情况下改变图像亮暗程度
        {MT9V03X_PCLK_MODE, 0},  // 像素时钟模式   仅总钻风MT9V034 V2.0以及以上版本支持该命令
};

CAMERA_DEBUG_VAR cameraDebugVar;

void cameraDebugVarInit()
{
    cameraDebugVar.INIT = 0;
    cameraDebugVar.AUTO_EXP = 0;
    cameraDebugVar.EXP_TIME = 450;
    cameraDebugVar.FPS = 100;
    cameraDebugVar.SET_COL = 188;
    cameraDebugVar.SET_ROW = 120;
    cameraDebugVar.GAIN = 32;
    cameraDebugVar.LR_OFFSET = 0;
    cameraDebugVar.UD_OFFSET = 0;
}

void cameraVarInitFromEeprom()
{
    // 自动曝光设置      范围1-63 0为关闭 如果自动曝光开启  EXP_TIME命令设置的数据将会变为最大曝光时间，也就是自动曝光时间的上限
    // 一般情况是不需要开启这个功能，因为比赛场地光线一般都比较均匀，如果遇到光线非常不均匀的情况可以尝试设置该值，增加图像稳定性
    mt9v03x_set_config_buffer[1][1] = (int16)cameraDebugVar.AUTO_EXP;

    // 曝光时间   摄像头收到后会自动计算出最大曝光时间，如果设置过大则设置为计算出来的最大曝光值
    mt9v03x_set_config_buffer[2][1] = (int16)cameraDebugVar.EXP_TIME;

    // 图像帧率          摄像头收到后会自动计算出最大FPS，如果过大则设置为计算出来的最大FPS
    mt9v03x_set_config_buffer[3][1] = (int16)cameraDebugVar.FPS;

    mt9v03x_set_config_buffer[4][1] = (int16)cameraDebugVar.SET_COL;

    mt9v03x_set_config_buffer[5][1] = (int16)cameraDebugVar.SET_ROW;

    // 图像左右偏移量    正值 右偏移   负值 左偏移  列为188 376 752时无法设置偏移
    mt9v03x_set_config_buffer[6][1] = (int16)cameraDebugVar.LR_OFFSET;

    // 图像上下偏移量    正值 上偏移   负值 下偏移  行为120 240 480时无法设置偏移
    mt9v03x_set_config_buffer[7][1] = (int16)cameraDebugVar.UD_OFFSET;

    // 图像增益          范围16-64     增益可以在曝光时间固定的情况下改变图像亮暗程度
    mt9v03x_set_config_buffer[8][1] = (int16)cameraDebugVar.GAIN;
}
/**
 * @brief    在ips200屏幕上打印总钻风灰度图
 * @param    void
 * @retval   void
 * @tips     调用逐飞官方库就好了
 */
void myShowGrayImage()
{
    system_delay_ms(100);
    do
    {
        if (mt9v03x_finish_flag)
        {
            // ips200_displayimage03x(mt9v03x_image[0],MT9V03X_W,MT9V03X_H);//要调节展示图像的宽和高的话去修改这两个宏定义
            ips200_show_gray_image(0, 0, mt9v03x_image[0], MT9V03X_W, MT9V03X_H, MT9V03X_W, MT9V03X_H, 0);
            mt9v03x_finish_flag = 0; // 图像显示完成后才对标志位清零
        }
        if (keymsg.status == KEY_DOWN)
        {
            system_delay_ms(15);
            if (keymsg.status == KEY_DOWN)
                return;
        }
    } while (1);

    //    if(mt9v03x_finish_flag){
    //       //ips200_displayimage03x(mt9v03x_image[0],MT9V03X_W,MT9V03X_H);//要调节展示图像的宽和高的话去修改这两个宏定义
    //       ips200_show_gray_image(50, 0, mt9v03x_image[0], MT9V03X_W, MT9V03X_H, MT9V03X_W, MT9V03X_H, 0);
    //       mt9v03x_finish_flag = 0;//图像显示完成后才对标志位清零
    //    }
}
/**
 * @brief    在ips200上打印通过大津法获得动态阈值得到的二值化图像
 * @param    void
 * @retval   void
 *
 */
void myShowBinaryImage()
{
    system_delay_ms(100);
    do
    {
        if (mt9v03x_finish_flag)
        {
            uint8 BandW_threshold = otsuThreshold_fast(mt9v03x_image[0]); // 大津法得到动态阈值BandW_threshold
            setImageTwoValue(BandW_threshold);                            // 动态阈值二值化得到二维数组mt9v03x_image_BandW
            ips200_show_gray_image(0, 0, mt9v03x_image_Binary[0], MT9V03X_W, MT9V03X_H, MT9V03X_W, MT9V03X_H, 0);
            mt9v03x_finish_flag = 0; // 图像显示完成后才对标志位清零
        }

        if (keymsg.status == KEY_DOWN)
        {
            system_delay_ms(15);
            if (keymsg.status == KEY_DOWN)
                return;
        }
    } while (1);
}

void myShowCompressImage()
{
    system_delay_ms(100);
    do
    {
        if (mt9v03x_finish_flag)
        {
            compressImage();
            ips200_show_gray_image(0, 0, mt9v03x_image_HorizonCompress[0], compress_W, compress_H, compress_W, compress_H, 0);
            mt9v03x_finish_flag = 0; // 图像显示完成后才对标志位清零
        }
        if (keymsg.status == KEY_DOWN)
        {
            system_delay_ms(15);
            if (keymsg.status == KEY_DOWN)
                return;
        }
    } while (1);
}
