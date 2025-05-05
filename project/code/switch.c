
#include "switch.h"
/* ����SWITCH��Ŷ�Ӧ�Ĺܽ� */
gpio_pin_enum SWITCH_PTxn[SWITCH_MAX] = {C12, C11, C14, C13};

/******************************************************************************
 *  @brief      ��ʼ��switch�˿�
 *****************************************************************************/
void switch_init()
{
    uint8 i = SWITCH_MAX;

    //��ʼ��ȫ�� ����
    while(i--)
    {
        gpio_init(SWITCH_PTxn[i], GPI, 0, GPO_PUSH_PULL);
    }
}



/******************************************************************************
 *  @brief      ��ȡswitch״̬
 *  @param      SWITCH_e         SWITCH���
 *  @return     SWITCH_STATUS_e    SWITCH״̬(SWITCH_ON,SWITCH_OFF)
 ******************************************************************************/
SWITCH_STATUS_e Switch_Get(SWITCH_e i)
{
    if(gpio_get_level(SWITCH_PTxn[i]) == SWITCH_OFF)
    {
        return SWITCH_OFF;
    }
    return SWITCH_ON;
}

