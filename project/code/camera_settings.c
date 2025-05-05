/*
 * camera_setting.c
 *
 *  Created on: 2022��7��25��
 *      Author: Samurai
 */

#include "camera_settings.h"

// ��ʼ��һ��ȫ�ֵ�11��2�ж�ά���飬������eeprom���д洢�͵�������ͷ��������
extern int16 mt9v03x_set_config_buffer[10][2] =
    {
        {MT9V03X_INIT, 0},       // ����ͷ��ʼ��   0
        {MT9V03X_AUTO_EXP, 0},   // �Զ��ع�����   Ĭ��Ϊ0�ر�
        {MT9V03X_EXP_TIME, 512}, // �ع�ʱ��      ����ͷ�յ�����Զ����������ع�ʱ�䣬������ù���������Ϊ�������������ع�ֵ
        {MT9V03X_FPS, 50},       // ͼ��֡��      ����ͷ�յ�����Զ���������FPS���������������Ϊ������������FPS
        {MT9V03X_SET_COL, 188},  // ͼ��������    ��Χ1-752
        {MT9V03X_SET_ROW, 120},  // ͼ��������    ��Χ1-480
        {MT9V03X_LR_OFFSET, 0},  // ͼ������ƫ����  ��ֵ ��ƫ��   ��ֵ ��ƫ��  ��Ϊ188 376 752ʱ�޷�����ƫ��    ����ͷ��ƫ�����ݺ���Զ��������ƫ�ƣ�������������ü�����������ƫ��
        {MT9V03X_UD_OFFSET, 0},  // ͼ������ƫ����  ��ֵ ��ƫ��   ��ֵ ��ƫ��  ��Ϊ120 240 480ʱ�޷�����ƫ��    ����ͷ��ƫ�����ݺ���Զ��������ƫ�ƣ�������������ü�����������ƫ��
        {MT9V03X_GAIN, 32},      // ͼ������      ��Χ16-64     ����������ع�ʱ��̶�������¸ı�ͼ�������̶�
        {MT9V03X_PCLK_MODE, 0},  // ����ʱ��ģʽ   �������MT9V034 V2.0�Լ����ϰ汾֧�ָ�����
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
    // �Զ��ع�����      ��Χ1-63 0Ϊ�ر� ����Զ��ع⿪��  EXP_TIME�������õ����ݽ����Ϊ����ع�ʱ�䣬Ҳ�����Զ��ع�ʱ�������
    // һ������ǲ���Ҫ����������ܣ���Ϊ�������ع���һ�㶼�ȽϾ��ȣ�����������߷ǳ������ȵ�������Գ������ø�ֵ������ͼ���ȶ���
    mt9v03x_set_config_buffer[1][1] = (int16)cameraDebugVar.AUTO_EXP;

    // �ع�ʱ��   ����ͷ�յ�����Զ����������ع�ʱ�䣬������ù���������Ϊ�������������ع�ֵ
    mt9v03x_set_config_buffer[2][1] = (int16)cameraDebugVar.EXP_TIME;

    // ͼ��֡��          ����ͷ�յ�����Զ���������FPS���������������Ϊ������������FPS
    mt9v03x_set_config_buffer[3][1] = (int16)cameraDebugVar.FPS;

    mt9v03x_set_config_buffer[4][1] = (int16)cameraDebugVar.SET_COL;

    mt9v03x_set_config_buffer[5][1] = (int16)cameraDebugVar.SET_ROW;

    // ͼ������ƫ����    ��ֵ ��ƫ��   ��ֵ ��ƫ��  ��Ϊ188 376 752ʱ�޷�����ƫ��
    mt9v03x_set_config_buffer[6][1] = (int16)cameraDebugVar.LR_OFFSET;

    // ͼ������ƫ����    ��ֵ ��ƫ��   ��ֵ ��ƫ��  ��Ϊ120 240 480ʱ�޷�����ƫ��
    mt9v03x_set_config_buffer[7][1] = (int16)cameraDebugVar.UD_OFFSET;

    // ͼ������          ��Χ16-64     ����������ع�ʱ��̶�������¸ı�ͼ�������̶�
    mt9v03x_set_config_buffer[8][1] = (int16)cameraDebugVar.GAIN;
}
/**
 * @brief    ��ips200��Ļ�ϴ�ӡ�����Ҷ�ͼ
 * @param    void
 * @retval   void
 * @tips     ������ɹٷ���ͺ���
 */
void myShowGrayImage()
{
    system_delay_ms(100);
    do
    {
        if (mt9v03x_finish_flag)
        {
            // ips200_displayimage03x(mt9v03x_image[0],MT9V03X_W,MT9V03X_H);//Ҫ����չʾͼ��Ŀ�͸ߵĻ�ȥ�޸��������궨��
            ips200_show_gray_image(0, 0, mt9v03x_image[0], MT9V03X_W, MT9V03X_H, MT9V03X_W, MT9V03X_H, 0);
            mt9v03x_finish_flag = 0; // ͼ����ʾ��ɺ�ŶԱ�־λ����
        }
        if (keymsg.status == KEY_DOWN)
        {
            system_delay_ms(15);
            if (keymsg.status == KEY_DOWN)
                return;
        }
    } while (1);

    //    if(mt9v03x_finish_flag){
    //       //ips200_displayimage03x(mt9v03x_image[0],MT9V03X_W,MT9V03X_H);//Ҫ����չʾͼ��Ŀ�͸ߵĻ�ȥ�޸��������궨��
    //       ips200_show_gray_image(50, 0, mt9v03x_image[0], MT9V03X_W, MT9V03X_H, MT9V03X_W, MT9V03X_H, 0);
    //       mt9v03x_finish_flag = 0;//ͼ����ʾ��ɺ�ŶԱ�־λ����
    //    }
}
/**
 * @brief    ��ips200�ϴ�ӡͨ����򷨻�ö�̬��ֵ�õ��Ķ�ֵ��ͼ��
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
            uint8 BandW_threshold = otsuThreshold_fast(mt9v03x_image[0]); // ��򷨵õ���̬��ֵBandW_threshold
            setImageTwoValue(BandW_threshold);                            // ��̬��ֵ��ֵ���õ���ά����mt9v03x_image_BandW
            ips200_show_gray_image(0, 0, mt9v03x_image_Binary[0], MT9V03X_W, MT9V03X_H, MT9V03X_W, MT9V03X_H, 0);
            mt9v03x_finish_flag = 0; // ͼ����ʾ��ɺ�ŶԱ�־λ����
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
            mt9v03x_finish_flag = 0; // ͼ����ʾ��ɺ�ŶԱ�־λ����
        }
        if (keymsg.status == KEY_DOWN)
        {
            system_delay_ms(15);
            if (keymsg.status == KEY_DOWN)
                return;
        }
    } while (1);
}
