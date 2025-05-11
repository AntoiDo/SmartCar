#include <menu.h>

#define PAGE_DISP_NUM (7)       // һҳ�����ʾ�Ĳ�����������
#define FLASH_SECTION_INDEX (0) // �洢�����õ�����
#define FLASH_PAGE_INDEX (8)    // �洢�����õ�ҳ�� ������һ��ҳ��

// ���ڶ����ص�define
#define SERVO_MOTOR_DUTY(x) ((float)PWM_DUTY_MAX / (1000.0 / (float)Servo_Freq) * (0.5 + (float)(x) / 90.0))
#define SERVO_MOTOR_L_MAX (50)  // ���������϶�����Χ �Ƕ�
#define SERVO_MOTOR_R_MAX (150) // ���������϶�����Χ �Ƕ�

uint32 sectorNum = 0; // �����޸ĵ�����Ĭ��0

volatile uint8 ExitMenu_flag;
extern int16 mt9v03x_set_config_buffer[MT9V03X_CONFIG_FINISH][2];
extern volatile float Servo_P;
int ServoPTemp = 0;
extern volatile float Servo_D;

// ��������ʼ������
void BEEP_init(void)
{
    gpio_init(BEEP_ENABLE, GPO, 0, GPO_PUSH_PULL);
}

// �򿪷�����
void BEEP_on(void)
{
    gpio_set_level(BEEP_ENABLE, 1);
}

// �رշ�����
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
//----------------------------------   �˵�����   --------------------------------------

// ����eeprom�д洢������
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

MENU_PRMT MainMenu_Prmt;       // �������˵��ṹ
MENU_PRMT exp7_Prmt;           // ����ʵ���ߵĲ˵��ṹ
MENU_PRMT exp8_Prmt;           // ����ʵ��˵Ĳ˵��ṹ
MENU_PRMT Camera_Setting_Prmt; // ��������ͷ�������õĲ˵��ṹ
MENU_PRMT show_Image_Prmt;     // ����ͼ����ʾ�Ĳ˵��ṹ

MENU_TABLE MainMenu_Table[] = // ����洢���˵��е�ִ�к����ṹ��
    {
        {(uint8 *)"1.NULL          ", Menu_Null, NULL},
        {(uint8 *)"2.Exp7          ", Menu_exp7, NULL},
        {(uint8 *)"3.Servo         ", Menu_exp8, NULL},
        {(uint8 *)"4.Camera Setting", Menu_Camera_Setting, NULL},
        {(uint8 *)"5.Image  Show   ", Menu_show_Image, NULL},
};

MENU_TABLE exp7_MenuTable[] = // ����洢ʵ�����е�ִ�к����ṹ��
    {
        {(uint8 *)"1.BEEP ON    ", BEEP_on, NULL},
        {(uint8 *)"2.BEEP OFF   ", BEEP_off, NULL},
        {(uint8 *)"3.Uart       ", Uart_Test, NULL},
};

MENU_TABLE exp8_MenuTable[] = // ����洢ʵ����е�ִ�к����ṹ��
    {
        {(uint8 *)"1.Servo_P", Menu_Null, (uint16 *)&ServoPTemp},
};
MENU_TABLE Camera_Setting_MenuTable[] = // ����洢����ͷ�����е�ִ�к����ṹ��
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

MENU_TABLE show_Image_MenuTable[] = // ����洢ͼ����ʾ�е�ִ�к����ṹ��
    {
        {(uint8 *)"1.Gray Image", myShowGrayImage, NULL},
        {(uint8 *)"2.Bin Image ", myShowBinaryImage, NULL},
        {(uint8 *)"3.Compress  ", myShowCompressImage, NULL},
};

void Menu_exp7(void) // ����ʵ����Ҫ���еĲ˵�����
{
    ips200_clear();
    uint8 menuNum;
    menuNum = sizeof(exp7_MenuTable) / sizeof(exp7_MenuTable[0]);
    Menu_Process((uint8 *)" -=    exp7    =- ", &exp7_Prmt, exp7_MenuTable, menuNum);
}

void Menu_exp8(void) // ����ʵ���Ҫ���еĲ˵�����
{
    ips200_clear();
    uint8 menuNum;
    menuNum = sizeof(exp8_MenuTable) / sizeof(exp8_MenuTable[0]);
    Menu_Process((uint8 *)"-=      exp8     =-", &exp8_Prmt, exp8_MenuTable, menuNum);
}

void Menu_Camera_Setting(void) // ��������ͷ��������Ҫ���еĲ˵�����
{
    ips200_clear();
    uint8 menuNum;
    menuNum = sizeof(Camera_Setting_MenuTable) / sizeof(Camera_Setting_MenuTable[0]);
    Menu_Process((uint8 *)"-= Camera Setting =-", &Camera_Setting_Prmt, Camera_Setting_MenuTable, menuNum);
}

void Menu_show_Image(void) // ����ͼ����ʾҪ���еĲ˵�����
{
    ips200_clear();
    uint8 menuNum;
    menuNum = sizeof(show_Image_MenuTable) / sizeof(show_Image_MenuTable[0]);
    Menu_Process((uint8 *)" -= Image  Show =- ", &show_Image_Prmt, show_Image_MenuTable, menuNum);
}
//----------------------------------   �˵�����   --------------------------------------

//----------------------------------   ��������   --------------------------------------
void image_show_rgb(void)
{
    ips200_show_rgb565_image(0, 0, (uint16 *)mt9v03x_image[0], MT9V03X_W, MT9V03X_H, SCC8660_W, SCC8660_H, 1);
}
void image_show_gray(void)
{
    ips200_show_gray_image(0, 0, mt9v03x_image[0], MT9V03X_W, MT9V03X_H, SCC8660_W, SCC8660_H, 0);
    // ���һ�������ɸ�Ϊ��ֵ����ֵ
}
/******************************************************************************
 * FunctionName   : KeySan()
 * Description    : ������ȡ
 * EntryParameter : None
 * ReturnValue    : ����ֵ
 *******************************************************************************/
KEY_e KeySan(void)
{
    // keymsg�洢����ѡ�񰴼���״̬
    // ���������� �� �˳��˵�ʱ������һ��
    while (keymsg.status == KEY_UP && !ExitMenu_flag)
    {
    }
    // ����ѡ�񰴼�״̬Ϊ�������ɿ�
    keymsg.status = KEY_UP;
    // ���ش����İ���ֵ
    return keymsg.key;
}

/******************************************************************************
 * FunctionName   : Menu_Move()
 * Description    : ����ƶ�
 * EntryParameter : prmt - �˵�����
 * EntryParameter : key - ����ֵ
 * ReturnValue    : ��ȷ�ϰ����ͷ���0�����򷵻�1
 ******************************************************************************/
uint8 Menu_Move(MENU_PRMT *prmt, KEY_e key)
{
    uint8 rValue = 1;

    switch (key)
    {
    case KEY_U: // ����
    {
        if (prmt->Cursor != 0) // ��겻�ڶ���
        {
            prmt->Cursor--; // �������
            uint16 x = prmt->Cursor;
            uint16 y = (x + 1) * 16;
            ips200_show_char(0, y, '>');
            system_delay_ms(250);
        }
        else // ����ڶ���
        {
            if (prmt->PageNo != 0) // ҳ��û�е���С
            {
                prmt->PageNo--; // ���Ϸ�
                uint16 x = prmt->Cursor;
                uint16 y = (x + 1) * 16;
                ips200_show_char(0, y, '>');
                system_delay_ms(250);
            }
            else
            {
                prmt->Cursor = prmt->DispNum - 1; // ��굽��
                prmt->PageNo = prmt->MaxPage - 1; // ���ҳ
                uint16 x = prmt->Cursor;
                uint16 y = (x + 1) * 16;
                ips200_show_char(0, y, '>');
                system_delay_ms(250);
            }
        }
        keymsg.status = KEY_UP;

        break;
    }

    case KEY_D: // ����
    {
        if (prmt->Cursor < prmt->DispNum - 1) // ���û�е��ף��ƶ����
        {
            prmt->Cursor++; // ��������ƶ�
            uint16 x = prmt->Cursor;
            uint16 y = (x + 1) * 16;
            ips200_show_char(0, y, '>');
            system_delay_ms(250);
        }
        else // ��굽��
        {
            if (prmt->PageNo < prmt->MaxPage - 1) // ҳ��û�е��ף�ҳ���ƶ�
            {
                prmt->PageNo++; // �·�һҳ
                uint16 x = prmt->Cursor;
                uint16 y = (x + 1) * 16;
                ips200_show_char(0, y, '>');
                system_delay_ms(250);
            }
            else // ҳ��͹�궼���ף����ؿ�ʼҳ
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
    case KEY_B: // ȷ��
    {
        prmt->Index = prmt->Cursor + prmt->PageNo; // ����ִ���������
        rValue = 0;
        keymsg.status = KEY_UP;
        uint16 x = prmt->Cursor;
        uint16 y = (x + 1) * 16;
        ips200_show_char(0, y, '*');
        system_delay_ms(250);
        break;
    }
    case KEY_L: // ��/�������ϼ��˵�
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
    case KEY_R: // �Ҽ������ײ�
    {
        prmt->Cursor = prmt->DispNum - 1; // ��굽��
        prmt->PageNo = prmt->MaxPage - 1; // ���ҳ
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

    return rValue; // ����ִ������
}

/******************************************************************************
 * FunctionName   : MainMenu_Set()
 * Description    : ��������
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

    // ��ʾ�˵�
    uint8 menuNum = sizeof(MainMenu_Table) / sizeof(MainMenu_Table[0]); // �˵�����
    Menu_Process((uint8 *)" -=   Setting   =- ", &MainMenu_Prmt, MainMenu_Table, menuNum);

    // Write_EEPROM(); // ������д��EEPROM����
    ips200_show_string(0, 0, "Ready to push them out");
    system_delay_ms(1000);
    ips200_clear();
}

/******************************************************************************
 * FunctionName   : Menu_Process()
 * Description    : ����˵���
 * EntryParameter : menuName - �˵�����
 * EntryParameter : prmt - �˵�������
 * EntryParameter : table - �˵�����
 * EntryParameter : num - �˵�����
 * ReturnValue    : None
 * Example        : Menu_Process((uint8 *)" -=exp7=- ", &exp7_Prmt, exp7_MenuTable, menuNum);
 ******************************************************************************/
void Menu_Process(uint8 *menuName, MENU_PRMT *prmt, MENU_TABLE *table, uint8 num)
{
    KEY_e key;
    Site_t site;
    uint8 page; // ��ʾ�˵���Ҫ��ҳ��

    if (num <= PAGE_DISP_NUM) // ������һҳ�����ʾ�Ĳ�����������ʱֻ����һҳ�˵�
        page = 1;
    else
    {
        page = num - PAGE_DISP_NUM + 1;
        num = PAGE_DISP_NUM;
    }
    Menu_PrmtInit(prmt, num, page); // ������ʾ������ҳ��

    do // ��ExitMark���ֲ��˳�״̬(=0) �� �˳�ָ��ΪFALSEʱһֱѭ��
    {
        ips200_clear();
        ips200_show_string(0, 0, (const char *)menuName); // ��ʾ�˵�����

        Menu_Display(table, prmt->PageNo, prmt->DispNum, prmt->Cursor); // ��ʾ�˵���
        key = KeySan();                                                 // ��ȡ����
        // function

        if (Menu_Move(prmt, key) == 0) // ���°������˵��ƶ�
        {

            // �жϴ˲˵���������Ҫ���ڵĲ���,
            if (table[prmt->Index].DebugParam != NULL && table[prmt->Index].ItemHook == Menu_Null) // ��������������
            {
                site.x = 120;
                site.y = (1 + prmt->Cursor) * 16;

                ips200_show_uint(site.x, site.y, *(table[prmt->Index].DebugParam), 4);
                ips200_show_char(0, site.y, '*'); // ��־λ*ʱ��ʼ����
                adjustParam(site, table[prmt->Index].DebugParam, 4, RGB565_WHITE, RGB565_RED);
            }
            else // �޾�ִ�в˵�����
            {
                table[prmt->Index].ItemHook();
            }
        }
    } while (prmt->ExitMark == 0 && ExitMenu_flag == 0);

    ips200_clear();
}

/******************************************************************************
 * FunctionName   : Menu_PrmtInit()
 * Description    : ��ʼ���˵�����
 * EntryParameter : prmt - �˵�����, num - ÿҳ��ʾ����, page - �����ʾҳ��
 * ReturnValue    : None
 *******************************************************************************/
void Menu_PrmtInit(MENU_PRMT *prmt, uint8 num, uint8 page)
{
    prmt->ExitMark = 0; // ����˳��˵���־

    prmt->Cursor = 0;    // �������
    prmt->PageNo = 0;    // ҳ����
    prmt->Index = 0;     // ��������
    prmt->DispNum = num; // ҳ�����ʾ��Ŀ��

    prmt->MaxPage = page; // ���ҳ��
}

/******************************************************************************
 * FunctionName   : Menu_Display()
 * Description    : *menuTable  - ��ʾ�˵���
 * EntryParameter : pageNo      - ��ǰҳ
 * EntryParameter : dispNum     - ÿһҳ����ʾ��
 * EntryParameter : cursor      - ���λ��
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
            // ��ʾ��ǰ���ѡ�в˵���
            site.x = 0;
            site.y = (i + 1) * 16;
            ips200_show_string(SHOW_POSITION, (uint16)site.y, (const char *)menuTable[pageNo + i].MenuName);
            // ips200_show_char(0, site.y, '>');//��ʾ���

            // ���˲˵�������Ҫ���Ĳ���������ʾ�ò���
            if (menuTable[pageNo + i].DebugParam != NULL)
            {
                site.x = 120;

                uint32 num_t = (*(menuTable[pageNo + i].DebugParam));
                ips200_show_uint(site.x, site.y, num_t, 4);
            }
        }
        else
        {
            /* ������ʾ����˵��� */
            site.x = 0;
            site.y = (i + 1) * 16;
            ips200_show_string(SHOW_POSITION, (uint16)site.y, (const char *)menuTable[pageNo + i].MenuName);
            // lcd_showstr_setColor((uint16)site.x, (uint16)site.y, (const char *)menuTable[pageNo + i].MenuName, RGB565_BLUE, RGB565_WHITE);
            /* ���˲˵�������Ҫ���Ĳ���������ʾ�ò��� */
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
 * Description    : �ڵ�Ƭ���Ͽ��ٵ���
 * EntryParameter : menuName - �˵�����
 * EntryParameter : prmt - �˵�����
 * EntryParameter : table - �˵�����
 * EntryParameter : num - �˵�����
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
//  �������      ���EEPROM������Ϣ
//  ����˵��      sector_num    ������д0  �˴�������Ų���ʵ�����ã�ֻ�������ӿ�
//  ����˵��      page_num      ��Ҫд���ҳ���   ������Χ0-11
//  ��ע��Ϣ      ������������ҳ���ҵ������ݵĲ��Ҳ���
//-------------------------------------------------------------------------------------------------------------------
/*
void eeprom_erase_sector(void)
{
    uint32 sectorNum = 0;
    for (uint16 pageNum = 0; pageNum < 11; ++pageNum)
    {
        if(flash_check(sectorNum, pageNum))                  // �ж��Ƿ�������
        {
            flash_erase_page(sectorNum, pageNum);            // ������һҳ
        }
    }
    ips200_show_string(0, 0, "Clear Finish!");
    system_delay_ms(1000);
}
*/
//-------------------------------------------------------------------------------------------------------------------
//  �������      ��EEPROMд������(һҳ��һ������)
//  ����˵��      sector_num    ������д0  �˴�������Ų���ʵ�����ã�ֻ�������ӿ�
//  ����˵��      page_num      ��Ҫд���ҳ���   ������Χ0-11
/*  ��ע��Ϣ      һ����12������
                 ÿ��������1024ҳ
                 һ����96KB
                 һҳֻ��8���ֽڣ�������32λ��4��16λ*/
//-------------------------------------------------------------------------------------------------------------------
void Write_EEPROM(void) //
{
    uint32 EEPROM_DATA_NUM = sizeof(EEPROM_DATA) / sizeof(EEPROM_DATA[0]); // ��Ҫд�������ҳ��
    uint32 pageNum = 0;
    uint32 *data_ptr = NULL; // ����ָ�룬��eeprom�е�������Ϊָ�봫��д�뺯��

    // ��������0�ĵ�11ҳ����Ϊ���ݻ���ҳ
    flash_erase_page(0, 11);

    ips200_show_string(0, 0, "START WRITE!");
    system_delay_ms(500);

    for (uint16 pageNum = 0; pageNum < EEPROM_DATA_NUM; ++pageNum)
    {
        data_ptr = (uint32 *)EEPROM_DATA[pageNum];
        if (data_ptr != NULL) // �����д�����ݷǿգ���д�뻺��ҳ
        {
            flash_union_buffer[pageNum].uint32_type = (uint16)*data_ptr;
        }
    }

    // ����ָ��ҳд�뻺�����е�����
    flash_write_page_from_buffer(0, 11);
    flash_buffer_clear();
    ips200_show_string(0, 0, "WRITE IS OK!");
    system_delay_ms(1000);
}

//-------------------------------------------------------------------------------------------------------------------
//  �������      ����EEPROM����
//  ����˵��      sector_num    ������д0  �˴�������Ų���ʵ�����ã�ֻ�������ӿ�
//  ����˵��      page_num      ��Ҫд���ҳ���   ������Χ0-11
//  ����˵��      data_temp     �洢���������
//  ��ע��Ϣ
//-------------------------------------------------------------------------------------------------------------------
void Read_EEPROM(void)
{
    uint32 EEPROM_DATA_NUM = sizeof(EEPROM_DATA) / sizeof(EEPROM_DATA[0]); // ��Ҫд�������ҳ��
    uint32 sectorNum = 0;
    uint32 *data_ptr = NULL;

    ips200_clear();                             // ����
    ips200_show_string(0, 0, "Start Reading!"); // ��ʾ��ȡ����
    system_delay_ms(1000);
    ips200_clear();

    flash_buffer_clear();
    flash_read_page_to_buffer(0, 11);

    // �ӵ�һҳ��ʼ����ÿһҳ
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
//----------------------------------   ��������   --------------------------------------
void drawCursor(MENU_PRMT *prmt, KEY_e key)
{
    uint16 x = prmt->Cursor;
    uint16 y = (x + 1) * 16;
    ips200_show_char(x, y, '*');
}
