#include "push.h"

uint8 NOISE = 11; // 判断状态的噪声范围

volatile od_result_t od_result[10];

/**
 * @brief   检测红色方块
 * @param   none
 * @note    car_state == COLOR_DETECT 代表检测到红色方块
 */
void detectRedBlock()
{
    static uint8 count = 0;
    uint8 pic_weight = od_result[0].res_x2 - od_result[0].res_x1; // 物块宽度
    uint8 pic_height = od_result[0].res_y2 - od_result[0].res_y1; // 物块高度
    // 判定条件1: y1 >= 45 && y2 >= 88 超过十次, 且没有进入push状态(version1)
    // 判定条件1:
    if (car_state == STATE_TRACKING && (od_result[0].res_y1 >= 37) && (od_result[0].res_y2 >= 65))
    {
        count++;
        if (count > 10)
        {
            car_state = STATE_COLOR_DETECT;
            Brake();
            count = 0;
        }
    }
}

/**
 * @brief   获取红色方块在x轴上的中心偏移量
 * @return  物块在x轴上的中心偏移量
 * @note    结果为正值则物块偏右，负值则物块偏左
 */
int8 getCenterOffset_XAxis()
{
    uint8 center_offset = 0;
    uint8 block_weight = od_result[0].res_x2 + od_result[0].res_x1; // 物块宽度
    center_offset = block_weight / 2;                               // 物块中心
    return center_offset - 53;
}

/**
 * @brief   其实上获取的是物块的宽度，我想的是当接近物体时方框的宽度肯定变大
 * @return  车子离物块的距离，大于0则需要继续前进这个数据
 * @note    65是一个经验值，实际使用中可以根据需要进行调整
 */
int8 getCenterOffset_YAxis()
{
    uint8 center_offset = 0;
    uint8 block_weight = od_result[0].res_x2 - od_result[0].res_x1; // 物块宽度
    return 67 - block_weight;
}

/**
 * @brief   检测车子识别到的方块是否稳定
 */
void detectBlockStable()
{
    if (car_state == STATE_COLOR_DETECT &&
        (getCenterOffset_XAxis()) < NOISE &&
        abs(getCenterOffset_YAxis()) < NOISE &&
        od_result[0].res_y2 >= 97)
    {
        car_state = STATE_ROTATE;
    }
}

void rotateCar(Rotate_Direction direction)
{
    if (direction == CLOCKWISE)
    {
        setServoAngle(PID_B(100, encoder_data.encoder_data_B)); // 车子向右转
    }
    else if (direction == COUNTERCLOCKWISE)
    {
        setServoAngle(PID_B(-100, encoder_data.encoder_data_B)); // 车子向左转
    }
    else
    {
        setServoAngle(0); // 车子不转
    }
}
