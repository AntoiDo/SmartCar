#include <menu.h>

#define PAGE_DISP_NUM (7)       // 一页最多显示的操作函数个数
#define FLASH_SECTION_INDEX (0) // 存储数据用的扇区
#define FLASH_PAGE_INDEX (8)    // 存储数据用的页码 倒数第一个页码

// 关于舵机相关的define
#define SERVO_MOTOR_DUTY(x) ((float)PWM_DUTY_MAX / (1000.0 / (float)Servo_Freq) * (0.5 + (float)(x) / 90.0))
#define SERVO_MOTOR_L_MAX (50)  // 定义主板上舵机活动范围 角度
#define SERVO_MOTOR_R_MAX (150) // 定义主板上舵机活动范围 角度

uint32 sectorNum = 0; // 不可修改的数据默认0

volatile uint8 ExitMenu_flag;
extern int16 mt9v03x_set_config_buffer[MT9V03X_CONFIG_FINISH][2];
extern volatile float Servo_P;
int ServoPTemp = 0;
extern volatile float Servo_D;

// 蜂鸣器初始化函数
void BEEP_init(void)
{
    gpio_init(BEEP_ENABLE, GPO, 0, GPO_PUSH_PULL);
}

// 打开蜂鸣器
void BEEP_on(void)
{
    gpio_set_level(BEEP_ENABLE, 1);
}

// 关闭蜂鸣器
void BEEP_off(void)
{
    gpio_set_level(BEEP_ENABLE, 0);
}

void Uart_Test()
{
    uartOpenMVSendString("Hello, MCX!\r\n");
    ips200_show_string(0, 0, "Send OK!");
    system_delay_ms(1000);
    ips200_show_string(0, 0, "Go back to menu!");
    return;
}

void EEPROM_Test()
{
}
//----------------------------------   菜单定义   --------------------------------------

// 定义eeprom中存储的数据
uint32 *EEPROM_DATA[] = {
    (uint32 *)&ServoPTemp,
    (uint32 *)&mt9v03x_set_config_buffer[1][1],
    (uint32 *)&mt9v03x_set_config_buffer[2][1],
    (uint32 *)&mt9v03x_set_config_buffer[3][1],
    (uint32 *)&mt9v03x_set_config_buffer[4][1],
    (uint32 *)&mt9v03x_set_config_buffer[5][1],
    (uint32 *)&mt9v03x_set_config_buffer[6][1],
    (uint32 *)&mt9v03x_set_config_buffer[7][1],
    (uint32 *)&mt9v03x_set_config_buffer[8][1],
};

MENU_PRMT MainMenu_Prmt;       // 定义主菜单结构
MENU_PRMT exp7_Prmt;           // 定义实验七的菜单结构
MENU_PRMT exp8_Prmt;           // 定义实验八的菜单结构
MENU_PRMT Camera_Setting_Prmt; // 定义摄像头参数设置的菜单结构
MENU_PRMT show_Image_Prmt;     // 定义图像显示的菜单结构

MENU_TABLE MainMenu_Table[] = // 定义存储主菜单中的执行函数结构体
    {
        {(uint8 *)"1.NULL          ", Menu_Null, NULL},
        {(uint8 *)"2.Exp7          ", Menu_exp7, NULL},
        {(uint8 *)"3.Servo         ", Menu_exp8, NULL},
        {(uint8 *)"4.Camera Setting", Menu_Camera_Setting, NULL},
        {(uint8 *)"5.Image  Show   ", Menu_show_Image, NULL},
};

MENU_TABLE exp7_MenuTable[] = // 定义存储实验七中的执行函数结构体
    {
        {(uint8 *)"1.BEEP ON    ", BEEP_on, NULL},
        {(uint8 *)"2.BEEP OFF   ", BEEP_off, NULL},
        {(uint8 *)"3.Uart       ", Uart_Test, NULL},
};

MENU_TABLE exp8_MenuTable[] = // 定义存储实验八中的执行函数结构体
    {
        {(uint8 *)"1.Servo_P", Menu_Null, (uint16 *)&ServoPTemp},
};
MENU_TABLE Camera_Setting_MenuTable[] = // 定义存储摄像头调参中的执行函数结构体
    {
        {(uint8 *)"1.AUTO_EXP  ", Menu_Null, (uint16 *)&mt9v03x_set_config_buffer[1][1]},
        {(uint8 *)"2.EXP_TIME  ", Menu_Null, (uint16 *)&mt9v03x_set_config_buffer[2][1]},
        {(uint8 *)"3.SET_COL   ", Menu_Null, (uint16 *)&mt9v03x_set_config_buffer[3][1]},
        {(uint8 *)"4.SET_ROW   ", Menu_Null, (uint16 *)&mt9v03x_set_config_buffer[4][1]},
        {(uint8 *)"5.FPS       ", Menu_Null, (uint16 *)&mt9v03x_set_config_buffer[5][1]},
        {(uint8 *)"6.GAIN      ", Menu_Null, (uint16 *)&mt9v03x_set_config_buffer[6][1]},
        {(uint8 *)"7.LR_OFFSE  ", Menu_Null, (uint16 *)&mt9v03x_set_config_buffer[7][1]},
        {(uint8 *)"8.UD_OFFSET ", Menu_Null, (uint16 *)&mt9v03x_set_config_buffer[8][1]},
};

MENU_TABLE show_Image_MenuTable[] = // 定义存储图像显示中的执行函数结构体
    {
        {(uint8 *)"1.Gray Image", myShowGrayImage, NULL},
        {(uint8 *)"2.Bin Image ", myShowBinaryImage, NULL},
        {(uint8 *)"3.Compress  ", myShowCompressImage, NULL},
};

void Menu_exp7(void) // 定义实验七要运行的菜单函数
{
    ips200_clear();
    uint8 menuNum;
    menuNum = sizeof(exp7_MenuTable) / sizeof(exp7_MenuTable[0]);
    Menu_Process((uint8 *)" -=    exp7    =- ", &exp7_Prmt, exp7_MenuTable, menuNum);
}

void Menu_exp8(void) // 定义实验八要运行的菜单函数
{
    ips200_clear();
    uint8 menuNum;
    menuNum = sizeof(exp8_MenuTable) / sizeof(exp8_MenuTable[0]);
    Menu_Process((uint8 *)"-=      exp8     =-", &exp8_Prmt, exp8_MenuTable, menuNum);
}

void Menu_Camera_Setting(void) // 定义摄像头参数设置要运行的菜单函数
{
    ips200_clear();
    uint8 menuNum;
    menuNum = sizeof(Camera_Setting_MenuTable) / sizeof(Camera_Setting_MenuTable[0]);
    Menu_Process((uint8 *)"-= Camera Setting =-", &Camera_Setting_Prmt, Camera_Setting_MenuTable, menuNum);
}

void Menu_show_Image(void) // 定义图像显示要运行的菜单函数
{
    ips200_clear();
    uint8 menuNum;
    menuNum = sizeof(show_Image_MenuTable) / sizeof(show_Image_MenuTable[0]);
    Menu_Process((uint8 *)" -= Image  Show =- ", &show_Image_Prmt, show_Image_MenuTable, menuNum);
}
//----------------------------------   菜单定义   --------------------------------------

//----------------------------------   操作函数   --------------------------------------
void image_show_rgb(void)
{
    ips200_show_rgb565_image(0, 0, (uint16 *)mt9v03x_image[0], MT9V03X_W, MT9V03X_H, SCC8660_W, SCC8660_H, 1);
}
void image_show_gray(void)
{
    ips200_show_gray_image(0, 0, mt9v03x_image[0], MT9V03X_W, MT9V03X_H, SCC8660_W, SCC8660_H, 0);
    // 最后一个参数可改为二值化阈值
}
/******************************************************************************
 * FunctionName   : KeySan()
 * Description    : 按键获取
 * EntryParameter : None
 * ReturnValue    : 按键值
 *******************************************************************************/
KEY_e KeySan(void)
{
    // keymsg存储的是选择按键的状态
    // 当按键按下 或 退出菜单时进入下一步
    while (keymsg.status == KEY_UP && !ExitMenu_flag)
    {
    }
    // 赋给选择按键状态为：按键松开
    keymsg.status = KEY_UP;
    // 返回触发的按键值
    return keymsg.key;
}

/******************************************************************************
 * FunctionName   : Menu_Move()
 * Description    : 光标移动
 * EntryParameter : prmt - 菜单参数
 * EntryParameter : key - 按键值
 * ReturnValue    : 有确认按键就返回0，否则返回1
 ******************************************************************************/
uint8 Menu_Move(MENU_PRMT *prmt, KEY_e key)
{
    uint8 rValue = 1;

    switch (key)
    {
    case KEY_U: // 向上
    {
        if (prmt->Cursor != 0) // 光标不在顶端
        {
            prmt->Cursor--; // 光标上移
            uint16 x = prmt->Cursor;
            uint16 y = (x + 1) * 16;
            ips200_show_char(0, y, '>');
            system_delay_ms(250);
        }
        else // 光标在顶端
        {
            if (prmt->PageNo != 0) // 页面没有到最小
            {
                prmt->PageNo--; // 向上翻
                uint16 x = prmt->Cursor;
                uint16 y = (x + 1) * 16;
                ips200_show_char(0, y, '>');
                system_delay_ms(250);
            }
            else
            {
                prmt->Cursor = prmt->DispNum - 1; // 光标到底
                prmt->PageNo = prmt->MaxPage - 1; // 最后页
                uint16 x = prmt->Cursor;
                uint16 y = (x + 1) * 16;
                ips200_show_char(0, y, '>');
                system_delay_ms(250);
            }
        }
        keymsg.status = KEY_UP;

        break;
    }

    case KEY_D: // 向下
    {
        if (prmt->Cursor < prmt->DispNum - 1) // 光标没有到底，移动光标
        {
            prmt->Cursor++; // 光标向下移动
            uint16 x = prmt->Cursor;
            uint16 y = (x + 1) * 16;
            ips200_show_char(0, y, '>');
            system_delay_ms(250);
        }
        else // 光标到底
        {
            if (prmt->PageNo < prmt->MaxPage - 1) // 页面没有到底，页面移动
            {
                prmt->PageNo++; // 下翻一页
                uint16 x = prmt->Cursor;
                uint16 y = (x + 1) * 16;
                ips200_show_char(0, y, '>');
                system_delay_ms(250);
            }
            else // 页面和光标都到底，返回开始页
            {
                prmt->Cursor = 0;
                prmt->PageNo = 0;
                uint16 x = prmt->Cursor;
                uint16 y = (x + 1) * 16;
                ips200_show_char(0, y, '>');
                system_delay_ms(250);
            }
        }
        keymsg.status = KEY_UP;

        break;
    }
    case KEY_B: // 确认
    {
        prmt->Index = prmt->Cursor + prmt->PageNo; // 计算执行项的索引
        rValue = 0;
        keymsg.status = KEY_UP;
        uint16 x = prmt->Cursor;
        uint16 y = (x + 1) * 16;
        ips200_show_char(0, y, '*');
        system_delay_ms(250);
        break;
    }
    case KEY_L: // 左/键返回上级菜单
    {

        prmt->Cursor = 0;
        prmt->PageNo = 0;
        prmt->ExitMark = 1;
        uint16 x = prmt->Cursor;
        uint16 y = (x + 1) * 16;
        ips200_show_char(0, y, '>');
        system_delay_ms(250);

        keymsg.status = KEY_UP;
        /*
        if(camera_flag == 1)
        {
            ips200_clear();
            camera_flag = 0;
//                if(mt9v03x_set_config(mt9v03x_set_config_buffer))
//                    ips200_show_string(0,0,"setting got!");
//                else
//                    ips200_show_string(0,0,"setting failed!");
        }
        */
        break;
    }
    case KEY_R: // 右键跳到底部
    {
        prmt->Cursor = prmt->DispNum - 1; // 光标到底
        prmt->PageNo = prmt->MaxPage - 1; // 最后页
        uint16 x = prmt->Cursor;
        uint16 y = (x + 1) * 16;
        ips200_show_char(0, y, '>');
        system_delay_ms(250);
        keymsg.status = KEY_UP;
        break;
    }

    default:
        break;
    }

    return rValue; // 返回执行索引
}

/******************************************************************************
 * FunctionName   : MainMenu_Set()
 * Description    : 常规设置
 * EntryParameter : None
 * ReturnValue    : None
 *******************************************************************************/
void MainMenu_Set(void)
{
    // while (!GET_SWITCH1() && !GET_SWITCH2())
    // {
    //     ips200_show_string(0, 0, "Whether Read DATA?");
    // }
    // if (GET_SWITCH1())
    // {
    //     Read_EEPROM();
    // }
    // else
    // {
    //     ips200_show_string(0, 0, "Skip to Read DATA!");
    //     system_delay_ms(500);
    // }

    ips200_clear();
    ExitMenu_flag = 0;

    // 显示菜单
    uint8 menuNum = sizeof(MainMenu_Table) / sizeof(MainMenu_Table[0]); // 菜单项数
    Menu_Process((uint8 *)" -=   Setting   =- ", &MainMenu_Prmt, MainMenu_Table, menuNum);

    // Write_EEPROM(); // 将数据写入EEPROM保存
    ips200_show_string(0, 0, "Ready to push them out");
    system_delay_ms(1000);
    ips200_clear();
}

/******************************************************************************
 * FunctionName   : Menu_Process()
 * Description    : 处理菜单项
 * EntryParameter : menuName - 菜单名称
 * EntryParameter : prmt - 菜单参数，
 * EntryParameter : table - 菜单表项
 * EntryParameter : num - 菜单项数
 * ReturnValue    : None
 * Example        : Menu_Process((uint8 *)" -=exp7=- ", &exp7_Prmt, exp7_MenuTable, menuNum);
 ******************************************************************************/
void Menu_Process(uint8 *menuName, MENU_PRMT *prmt, MENU_TABLE *table, uint8 num)
{
    KEY_e key;
    Site_t site;
    uint8 page; // 显示菜单需要的页数

    if (num <= PAGE_DISP_NUM) // 当不足一页最多显示的操作函数个数时只创建一页菜单
        page = 1;
    else
    {
        page = num - PAGE_DISP_NUM + 1;
        num = PAGE_DISP_NUM;
    }
    Menu_PrmtInit(prmt, num, page); // 设置显示项数和页数

    do // 当ExitMark保持不退出状态(=0) 且 退出指针为FALSE时一直循环
    {
        ips200_clear();
        ips200_show_string(0, 0, (const char *)menuName); // 显示菜单标题

        Menu_Display(table, prmt->PageNo, prmt->DispNum, prmt->Cursor); // 显示菜单项
        key = KeySan();                                                 // 获取按键
        // function

        if (Menu_Move(prmt, key) == 0) // 按下按键，菜单移动
        {

            // 判断此菜单项有无需要调节的参数,
            if (table[prmt->Index].DebugParam != NULL && table[prmt->Index].ItemHook == Menu_Null) // 有则进入参数调节
            {
                site.x = 120;
                site.y = (1 + prmt->Cursor) * 16;

                ips200_show_uint(site.x, site.y, *(table[prmt->Index].DebugParam), 4);
                ips200_show_char(0, site.y, '*'); // 标志位*时开始调参
                adjustParam(site, table[prmt->Index].DebugParam, 4, RGB565_WHITE, RGB565_RED);
            }
            else // 无就执行菜单函数
            {
                table[prmt->Index].ItemHook();
            }
        }
    } while (prmt->ExitMark == 0 && ExitMenu_flag == 0);

    ips200_clear();
}

/******************************************************************************
 * FunctionName   : Menu_PrmtInit()
 * Description    : 初始化菜单参数
 * EntryParameter : prmt - 菜单参数, num - 每页显示项数, page - 最大显示页数
 * ReturnValue    : None
 *******************************************************************************/
void Menu_PrmtInit(MENU_PRMT *prmt, uint8 num, uint8 page)
{
    prmt->ExitMark = 0; // 清除退出菜单标志

    prmt->Cursor = 0;    // 光标清零
    prmt->PageNo = 0;    // 页清零
    prmt->Index = 0;     // 索引清零
    prmt->DispNum = num; // 页最多显示项目数

    prmt->MaxPage = page; // 最多页数
}

/******************************************************************************
 * FunctionName   : Menu_Display()
 * Description    : *menuTable  - 显示菜单项
 * EntryParameter : pageNo      - 当前页
 * EntryParameter : dispNum     - 每一页的显示项
 * EntryParameter : cursor      - 光标位置
 * ReturnValue    : None
 *******************************************************************************/
void Menu_Display(MENU_TABLE *menuTable, uint8 pageNo, uint8 dispNum, uint8 cursor)
{
    uint8 i;
    Site_t site;

    for (i = 0; i < dispNum; i++)
    {
        if (cursor == i)
        {
            // 显示当前光标选中菜单项
            site.x = 0;
            site.y = (i + 1) * 16;
            ips200_show_string(SHOW_POSITION, (uint16)site.y, (const char *)menuTable[pageNo + i].MenuName);
            // ips200_show_char(0, site.y, '>');//显示光标

            // 若此菜单项有需要调的参数，则显示该参数
            if (menuTable[pageNo + i].DebugParam != NULL)
            {
                site.x = 120;

                uint32 num_t = (*(menuTable[pageNo + i].DebugParam));
                ips200_show_uint(site.x, site.y, num_t, 4);
            }
        }
        else
        {
            /* 正常显示其余菜单项 */
            site.x = 0;
            site.y = (i + 1) * 16;
            ips200_show_string(SHOW_POSITION, (uint16)site.y, (const char *)menuTable[pageNo + i].MenuName);
            // lcd_showstr_setColor((uint16)site.x, (uint16)site.y, (const char *)menuTable[pageNo + i].MenuName, RGB565_BLUE, RGB565_WHITE);
            /* 若此菜单项有需要调的参数，则显示该参数 */
            if (menuTable[pageNo + i].DebugParam != NULL)
            {
                site.x = 120;

                uint16 num_t = (*(menuTable[pageNo + i].DebugParam));
                ips200_show_uint(site.x, site.y, num_t, 4);
            }
        }
    }
}

/******************************************************************************
 * FunctionName   : adjustParam()
 * Description    : 在单片机上快速调参
 * EntryParameter : menuName - 菜单名称
 * EntryParameter : prmt - 菜单参数
 * EntryParameter : table - 菜单表项
 * EntryParameter : num - 菜单项数
 * ReturnValue    : None
 ******************************************************************************/
void adjustParam(Site_t site, uint16 *param, uint8 max_param_bit, uint16 Color, uint16 bkColor)
{
    do
    {
        KeySan();

        switch (keymsg.key)
        {
        case KEY_U:
            if (*param <= 65534)
                (*param)++;
            break;

        case KEY_D:
            if (*param >= 1)
                (*param)--;
            break;

        case KEY_L:
            if (*param >= 10)
                (*param) -= 10;
            break;

        case KEY_R:
            if (*param <= 65525)
                (*param) += 10;
            break;

        case KEY_B:
            system_delay_ms(20);
            if (keymsg.key == KEY_B)
                return;
        }
        ips200_show_uint(site.x, site.y, *param, 4);

    } while (1);
}

void Menu_Null()
{
    // DELAY_MS(100);
}

//-------------------------------------------------------------------------------------------------------------------
//  函数简介      清除EEPROM扇区信息
//  参数说明      sector_num    仅可填写0  此处扇区编号并无实际作用，只是留出接口
//  参数说明      page_num      需要写入的页编号   参数范围0-11
//  备注信息      遍历所有数据页，找到有数据的并且擦除
//-------------------------------------------------------------------------------------------------------------------
/*
void eeprom_erase_sector(void)
{
    uint32 sectorNum = 0;
    for (uint16 pageNum = 0; pageNum < 11; ++pageNum)
    {
        if(flash_check(sectorNum, pageNum))                  // 判断是否有数据
        {
            flash_erase_page(sectorNum, pageNum);            // 擦除这一页
        }
    }
    ips200_show_string(0, 0, "Clear Finish!");
    system_delay_ms(1000);
}
*/
//-------------------------------------------------------------------------------------------------------------------
//  函数简介      向EEPROM写入数据(一页存一个数据)
//  参数说明      sector_num    仅可填写0  此处扇区编号并无实际作用，只是留出接口
//  参数说明      page_num      需要写入的页编号   参数范围0-11
/*  备注信息      一共有12个扇区
                 每个扇区有1024页
                 一共有96KB
                 一页只有8个字节，放两个32位，4个16位*/
//-------------------------------------------------------------------------------------------------------------------
void Write_EEPROM(void) //
{
    uint32 EEPROM_DATA_NUM = sizeof(EEPROM_DATA) / sizeof(EEPROM_DATA[0]); // 需要写入的数据页数
    uint32 pageNum = 0;
    uint32 *data_ptr = NULL; // 创建指针，将eeprom中的数据作为指针传入写入函数

    // 擦除扇区0的第11页，作为数据缓冲页
    flash_erase_page(0, 11);

    ips200_show_string(0, 0, "START WRITE!");
    system_delay_ms(500);

    for (uint16 pageNum = 0; pageNum < EEPROM_DATA_NUM; ++pageNum)
    {
        data_ptr = (uint32 *)EEPROM_DATA[pageNum];
        if (data_ptr != NULL) // 如果待写入数据非空，就写入缓冲页
        {
            flash_union_buffer[pageNum].uint32_type = (uint16)*data_ptr;
        }
    }

    // 再向指定页写入缓冲区中的数据
    flash_write_page_from_buffer(0, 11);
    flash_buffer_clear();
    ips200_show_string(0, 0, "WRITE IS OK!");
    system_delay_ms(1000);
}

//-------------------------------------------------------------------------------------------------------------------
//  函数简介      读入EEPROM数据
//  参数说明      sector_num    仅可填写0  此处扇区编号并无实际作用，只是留出接口
//  参数说明      page_num      需要写入的页编号   参数范围0-11
//  参数说明      data_temp     存储读入的数据
//  备注信息
//-------------------------------------------------------------------------------------------------------------------
void Read_EEPROM(void)
{
    uint32 EEPROM_DATA_NUM = sizeof(EEPROM_DATA) / sizeof(EEPROM_DATA[0]); // 需要写入的数据页数
    uint32 sectorNum = 0;
    uint32 *data_ptr = NULL;

    ips200_clear();                             // 清屏
    ips200_show_string(0, 0, "Start Reading!"); // 显示读取字样
    system_delay_ms(1000);
    ips200_clear();

    flash_buffer_clear();
    flash_read_page_to_buffer(0, 11);

    // 从第一页开始遍历每一页
    for (uint16 pageNum = 0; pageNum < EEPROM_DATA_NUM; pageNum++)
    {
        ips200_show_string(0, 0, "reading...");
        system_delay_ms(200);

        uint32 temp_vaule = flash_union_buffer[pageNum].uint32_type;

        if ((uint16)temp_vaule < 5000)
        {
            data_ptr = (uint32 *)EEPROM_DATA[pageNum];
            if (data_ptr != NULL)
            {
                *data_ptr = (uint16)temp_vaule;
            }
        }
        else
        {
            data_ptr = (uint32 *)EEPROM_DATA[pageNum];
            if (data_ptr != NULL)
            {
                *data_ptr = 0;
            }
        }
    }
}
//----------------------------------   操作函数   --------------------------------------
void drawCursor(MENU_PRMT *prmt, KEY_e key)
{
    uint16 x = prmt->Cursor;
    uint16 y = (x + 1) * 16;
    ips200_show_char(x, y, '*');
}
