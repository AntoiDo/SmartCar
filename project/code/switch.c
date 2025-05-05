
#include "switch.h"
/* 定义SWITCH编号对应的管脚 */
gpio_pin_enum SWITCH_PTxn[SWITCH_MAX] = {C12, C11, C14, C13};

/******************************************************************************
 *  @brief      初始化switch端口
 *****************************************************************************/
void switch_init()
{
    uint8 i = SWITCH_MAX;

    //初始化全部 按键
    while(i--)
    {
        gpio_init(SWITCH_PTxn[i], GPI, 0, GPO_PUSH_PULL);
    }
}



/******************************************************************************
 *  @brief      获取switch状态
 *  @param      SWITCH_e         SWITCH编号
 *  @return     SWITCH_STATUS_e    SWITCH状态(SWITCH_ON,SWITCH_OFF)
 ******************************************************************************/
SWITCH_STATUS_e Switch_Get(SWITCH_e i)
{
    if(gpio_get_level(SWITCH_PTxn[i]) == SWITCH_OFF)
    {
        return SWITCH_OFF;
    }
    return SWITCH_ON;
}

