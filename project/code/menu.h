#ifndef MENU_H
#define MENU_H

#include <zf_common_headfile.h>

#define SHOW_POSITION 10 // 选项初始的y值,前面10个给光标
extern uint32 ledBrightness;
/******************************* 菜单结构体定义 *******************************/
typedef struct
{

    uint8 ExitMark; // 退出菜单(0-不退出，1-退出)

    uint8 Cursor; // 光标值(当前光标位置)

    uint8 PageNo; // 菜单页(显示开始项)

    uint8 Index; // 菜单索引(当前选择的菜单项)

    uint8 DispNum; // 显示项数(每页可以显示在菜单项)

    uint8 MaxPage; // 最大页数(最大有多少种显示页)

} MENU_PRMT; // 菜单参数

typedef struct
{

    uint8 *MenuName; // 菜单项目名称

    void (*ItemHook)(void); // 要运行的菜单函数

    uint16 *DebugParam; // 要调试的参数

} MENU_TABLE; // 菜单执行
/******************************* 菜单结构体定义 *******************************/

/******************************* 按键端口的枚举 *******************************/
typedef struct
{

    uint8 x;
    uint8 y;

} Site_t;
/******************************* 按键端口的枚举 *******************************/

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

// 对函数进行声明
void BEEP_init();
void BEEP_on();
void BEEP_off();

#endif