#ifndef MENU_H
#define MENU_H

#include <zf_common_headfile.h>

#define SHOW_POSITION 10 // ѡ���ʼ��yֵ,ǰ��10�������
extern uint32 ledBrightness;
/******************************* �˵��ṹ�嶨�� *******************************/
typedef struct
{

    uint8 ExitMark; // �˳��˵�(0-���˳���1-�˳�)

    uint8 Cursor; // ���ֵ(��ǰ���λ��)

    uint8 PageNo; // �˵�ҳ(��ʾ��ʼ��)

    uint8 Index; // �˵�����(��ǰѡ��Ĳ˵���)

    uint8 DispNum; // ��ʾ����(ÿҳ������ʾ�ڲ˵���)

    uint8 MaxPage; // ���ҳ��(����ж�������ʾҳ)

} MENU_PRMT; // �˵�����

typedef struct
{

    uint8 *MenuName; // �˵���Ŀ����

    void (*ItemHook)(void); // Ҫ���еĲ˵�����

    uint16 *DebugParam; // Ҫ���ԵĲ���

} MENU_TABLE; // �˵�ִ��
/******************************* �˵��ṹ�嶨�� *******************************/

/******************************* �����˿ڵ�ö�� *******************************/
typedef struct
{

    uint8 x;
    uint8 y;

} Site_t;
/******************************* �����˿ڵ�ö�� *******************************/

void Menu_RunMode(void);
void Menu_exp7(void);
void Menu_exp8(void);
void Menu_Camera_Setting(void);
void Menu_show_Image(void);
void image_show_rgb(void);
void image_show_gray(void);

void MainMenu_Set(void);
void Menu_Process(uint8 *menuName, MENU_PRMT *prmt, MENU_TABLE *table, uint8 num);
void Menu_PrmtInit(MENU_PRMT *prmt, uint8 num, uint8 page);
void Menu_Display(MENU_TABLE *menuTable, uint8 pageNo, uint8 dispNum, uint8 cursor);
// uint8 Menu_Move(MENU_PRMT *prmt, KEY_e key);

// KEY_e KeySan(void);
void adjustParam(Site_t site, uint16 *param, uint8 max_param_bit, uint16 Color, uint16 bkColor);

void Menu_Null(void);
void Write_EEPROM(void);
void Read_EEPROM(void);

#define BEEP_ENABLE C8

// �Ժ�����������
void BEEP_init();
void BEEP_on();
void BEEP_off();

#endif