#include <key.h>

volatile KEY_MSG_t keymsg = {KEY_L, KEY_R, KEY_U, KEY_D, KEY_B}; // 按键消息结构体

// 根据自己的GPIO口来设定
// 学习板五向开关引脚     P10_3 LEFT    P10_1 RIGHT    P20_3 UP    P20_0 DOWN   P02_5 MID
// 逐飞母板四个开关       P11_2 LEFT    P11_3 RIGHT    P20_6 UP    P20_7 DOWN
// 我的五向开关定义       P20_3 LEFT    P20_0 RIGHT    P10_3 UP    P10_1 DOWN   P20_10 MID
gpio_pin_enum KEY_PTxn[KEY_MAX] = {C31, B10, B9, B11, C30};

/*
 *  @brief      初始化key端口(key 小于 KEY_MAX 时初始化 对应端口，否则初始化全部端口)
 *  @param      KEY_e    KEY编号
 *  @since      v5.0
 *  Sample usage:       key_init (KEY_U);    //初始化 KEY_U
 */
void Key_init(KEY_e key)
{
    if (key < KEY_MAX)
    {
        gpio_init(KEY_PTxn[key], GPI, 1, GPI_PULL_UP);
    }
    else
    {
        key = KEY_MAX;

        // 初始化全部 按键
        while (key--)
        {
            gpio_init(KEY_PTxn[key], GPI, 1, GPI_PULL_UP);
        }
    }
}

/*
 *  @brief      获取key状态（不带延时消抖）
 *  @param      KEY_e           KEY编号
 *  @return     KEY_STATUS_e    KEY状态（KEY_DOWN、KEY_DOWN）
 *  @since      v5.0
 *  Sample usage:
                    if(key_get(KEY_U) ==  KEY_DOWN)
                    {
                        printf("\n按键按下")
                    }
 */
KEY_STATUS_e key_get(KEY_e key)
{
    if (gpio_get_level(KEY_PTxn[key]) == KEY_DOWN)
    {
        return KEY_DOWN;
    }
    return KEY_UP;
}

/*!
 *  @brief      检测key状态（带延时消抖）
 *  @param      KEY_e           KEY编号
 *  @return     KEY_STATUS_e    KEY状态（KEY_DOWN、KEY_DOWN）
 *  @since      v5.0
 *  Sample usage:
                    if(key_check(KEY_U) ==  KEY_DOWN)
                    {
                        printf("\n按键按下")
                    }
 */
KEY_STATUS_e key_check(KEY_e key)
{
    if (key_get(key) == KEY_DOWN)
    {
        system_delay_ms(10);
        if (key_get(key) == KEY_DOWN)
        {
            return KEY_DOWN;
        }
    }
    return KEY_UP;
}
/*********************  如下代码是实现按键定时扫描，发送消息到FIFO  ********************/
/*
 * 定义按键消息FIFO状态
 */
typedef enum
{
    KEY_MSG_EMPTY,  // 没有按键消息
    KEY_MSG_NORMAL, // 正常，有按键消息，但不满
    KEY_MSG_FULL,   // 按键消息满
} key_msg_e;

/*
 * 定义按键消息FIFO相关的变量
 */
KEY_MSG_t key_msg[KEY_MSG_FIFO_SIZE];               // 按键消息FIFO
volatile uint8 key_msg_front = 0, key_msg_rear = 0; // 接收FIFO的指针
volatile uint8 key_msg_flag = KEY_MSG_EMPTY;        // 按键消息FIFO状态

/*!
 *  @brief      发送按键消息到FIFO
 *  @param      KEY_MSG_t       按键消息
 *  @since      v5.0
 *  Sample usage:
                    KEY_MSG_t *keymsg;
                    keymsg.key      = KEY_U;
                    keymsg.status   = KEY_HOLD;
                    send_key_msg(keymsg);                   //发送
 */
void send_key_msg(KEY_MSG_t keymsg)
{
    uint8 tmp;
    // 保存在FIFO里
    if (key_msg_flag == KEY_MSG_FULL)
    {
        // 满了直接不处理
        return;
    }
    key_msg[key_msg_rear].key = keymsg.key;
    key_msg[key_msg_rear].status = keymsg.status;

    key_msg_rear++;

    if (key_msg_rear >= KEY_MSG_FIFO_SIZE)
    {
        key_msg_rear = 0; // 重头开始
    }

    tmp = key_msg_rear;
    if (tmp == key_msg_front) // 追到屁股了，满了
    {
        key_msg_flag = KEY_MSG_FULL;
    }
    else
    {
        key_msg_flag = KEY_MSG_NORMAL;
    }
}

/*!
 *  @brief      从FIFO里获取按键消息
 *  @param      KEY_MSG_t       按键消息
 *  @return     是否获取按键消息（1为获取成功，0为没获取到按键消息）
 *  @since      v5.0
 *  Sample usage:
                    KEY_MSG_t keymsg;
                    if(get_key_msg(&keymsg) == 1)
                    {
                        printf("\n按下按键KEY%d,类型为%d（0为按下，1为弹起，2为长按）",keymsg.key,keymsg.status);
                    }
 */
uint8 get_key_msg(KEY_MSG_t *keymsg)
{
    uint8 tmp;

    if (key_msg_flag == KEY_MSG_EMPTY) // 按键消息FIFO为空，直接返回0
    {
        return 0;
    }
    /**********************出队**********************/
    keymsg->key = key_msg[key_msg_front].key;       // 从FIFO队首中获取按键值
    keymsg->status = key_msg[key_msg_front].status; // 从FIFO队首中获取按键类型

    key_msg_front++; // FIFO队首指针加1，指向下一个消息
    /************************************************/

    if (key_msg_front >= KEY_MSG_FIFO_SIZE) // FIFO指针队首溢出则从0开始计数
    {
        key_msg_front = 0; // 重头开始计数（循环利用数组）
    }

    tmp = key_msg_rear;
    if (key_msg_front == tmp) // 比较队首和队尾是否一样，一样则表示FIFO已空了
    {
        key_msg_flag = KEY_MSG_EMPTY;
    }
    else
    {
        key_msg_flag = KEY_MSG_NORMAL;
    }

    return 1;
}

/*!
 *  @brief      定时检测key状态(带hold判断)
 *  @since      v5.0
 *  @note       此函数需要放入 定时中断服务函数里，定时10ms执行一次
 */
void key_IRQHandler(void)
{
    KEY_e keynum;
    static uint8 keytime[KEY_MAX]; // 静态数组，保存各按键的按下时间

    KEY_MSG_t keymsg; // 按键消息

    for (keynum = (KEY_e)0; keynum < KEY_MAX; keynum++) // 每个按键轮询
    {
        // 判断按键是否被按下
        if (key_get(keynum) == KEY_DOWN) // 被按下
        {
            keytime[keynum]++; // 按下状态下，按下时间累加
            // 判断时间是否没超过消抖确认按下时间
            if (keytime[keynum] <= KEY_DOWN_TIME) // 1.未超过
            {
                continue; // 没达到，则继续等待
            }
            else if (KEY_DOWN_TIME < keytime[keynum] && keytime[keynum] <= KEY_HOLD_TIME) // 2.超过消抖时间，但是没有超过HOLD时间
            {
                keymsg.key = keynum;
                keymsg.status = KEY_DOWN;
                send_key_msg(keymsg); // 把按键值和DOWN消息发送到FIFO
            }
            else if (keytime[keynum] == KEY_DOWN_TIME + 1) // 3.已经超过HOLD时间，处于HOLD状态
            {
                uint8 tme = 0;                                  // 初始化计时器时间为0
                uint8 interval = KEY_HOLD_TIME - KEY_DOWN_TIME; // 设定消息发送间隔时间interval
                keymsg.key = keynum;
                keymsg.status = KEY_HOLD; // 更新按键状态
                send_key_msg(keymsg);
                while (true) // 一旦按键松开就不会再进入该循环所在if大分支
                {
                    if (tme == interval)
                    {
                        send_key_msg(keymsg); // 每隔interval把HOLD消息发送到FIFO
                        tme = 0;              // 计时器清零
                    }
                    else
                    {
                        tme++;
                    }
                }
            }
        }
        else // 没按下
        {
            if (keytime[keynum] > KEY_DOWN_TIME) // 如果超过消抖时间，确认过按下按键
            {
                keymsg.key = keynum;
                keymsg.status = KEY_UP;
                send_key_msg(keymsg); // 发送按键弹起消息
            }
            keytime[keynum] = 0; // 按下时间累计清0
        }
    }
}