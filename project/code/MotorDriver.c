#include <MotorDriver.h>
#include <zf_driver_pwm.h>

/* 这三个速度是通过输入想要的xy速度和角速度后计算得出的理论结果 */
int8_t Calculate_Speed_Left = 0;
int8_t Calculate_Speed_Right = 0;
int8_t Calculate_Speed_Buttom = 0;

void motorInit()
{
    gpio_init(MotorL_DIR, GPO, GPIO_HIGH, GPO_PUSH_PULL); // GPIO 初始化为输出 默认上拉输出高
    pwm_init(MotorL_PWM, 17000, 0);                       // PWM 通道初始化频率 17KHz 占空比初始为 0

    gpio_init(MotorR_DIR, GPO, GPIO_HIGH, GPO_PUSH_PULL); // GPIO 初始化为输出 默认上拉输出高
    pwm_init(MotorR_PWM, 17000, 0);                       // PWM 通道初始化频率 17KHz 占空比初始为 0
}

void servoInit()
{
    gpio_init(MotorServo_DIR, GPO, GPIO_HIGH, GPO_PUSH_PULL); // GPIO 初始化为输出 默认上拉输出高
    pwm_init(MotorServo_PWM, 17000, 0);                       // PWM 通道初始化频率 50Hz 占空比初始为 0
}

/**
 * @brief  设置中间轮子的速度
 * @param  给的是正值的话那么就是顺时针旋转，负值为逆时针
 */
void setServoAngle(int angle)
{
    if (angle > SPEED_MAX)
        angle = SPEED_MAX;
    else if (angle < -SPEED_MAX)
        angle = -SPEED_MAX;
    if (angle < 0)
    {
        gpio_set_level(MotorServo_DIR, 1);
        pwm_set_duty(MotorServo_PWM, -angle);
    }
    else
    {
        gpio_set_level(MotorServo_DIR, 0);
        pwm_set_duty(MotorServo_PWM, angle);
    }
}
/**
 * @brief  设置电机速度
 * @note   对应的是左轮,3号轮子
 * @param  givenSpeed 电机速度,如果正值逆时针转动,负值顺时针转动
 */
void setLeftMotorSpeed(int givenSpeed)
{
    if (givenSpeed > SPEED_MAX)
        givenSpeed = SPEED_MAX;
    else if (givenSpeed < -SPEED_MAX)
        givenSpeed = -SPEED_MAX;
    if (givenSpeed < 0)
    {
        gpio_set_level(MotorL_DIR, 1);
        pwm_set_duty(MotorL_PWM, -givenSpeed);
    }
    else
    {
        gpio_set_level(MotorL_DIR, 0);
        pwm_set_duty(MotorL_PWM, givenSpeed);
    }
}
/**
 * @brief   设置电机速度
 * @note 对应的是右轮,1号轮子,顺时针为正方向,此时编码器是正值
 */
void setRightMotorSpeed(int givenSpeed)
{
    if (givenSpeed > SPEED_MAX)
        givenSpeed = SPEED_MAX;
    else if (givenSpeed < -SPEED_MAX)
        givenSpeed = -SPEED_MAX;
    if (givenSpeed < 0)
    {
        gpio_set_level(MotorR_DIR, 0);
        pwm_set_duty(MotorR_PWM, -givenSpeed);
    }
    else
    {
        gpio_set_level(MotorR_DIR, 1);
        pwm_set_duty(MotorR_PWM, givenSpeed);
    }
}

#define X_PARAMETER (0.5f)
#define Y_PARAMETER (sqrt(3) / 2.f)
#define L_PARAMETER (0.117f)
/**
 * @brief  运动学分析
 * @param  Vx  期望的x轴速度
 * @param  Vy  期望的y轴速度
 * @param  V_angle  期望的角速度
 * @note   结果为正则顺时针转动，负值为逆时针转动，注意和设置电机转速函数里面的方向进行比较
 * @note   再思考一下好像只要对Left计算结果取反就可以兼容了
 */
void Kinematic_Analysis(float Vx, float Vy, float V_angle)
{
    Calculate_Speed_Buttom = Vx + L_PARAMETER * V_angle / (2 * PI) * 60;
    Calculate_Speed_Right = -X_PARAMETER * Vx + Y_PARAMETER * Vy + L_PARAMETER * V_angle / (2 * PI) * 60;
    Calculate_Speed_Left = -X_PARAMETER * Vx - Y_PARAMETER * Vy + L_PARAMETER * V_angle / (2 * PI) * 60;
    Calculate_Speed_Left = -Calculate_Speed_Left;
}

/**
 * @brief  刹车
 */
void Brake()
{
    pwm_set_duty(MotorL_PWM, 0);
    pwm_set_duty(MotorR_PWM, 0);
    pwm_set_duty(MotorServo_PWM, 0);
    gpio_set_level(MotorL_DIR, 0);
    gpio_set_level(MotorR_DIR, 0);
    gpio_set_level(MotorServo_DIR, 0);
}

/**
 * @brief  设置所有电机的速度
 * @param  left 左轮速度
 * @param  right 右轮速度
 * @param  servo 中间轮速度
 */
void setAllMotorSpeed(int left, int right, int servo)
{
    setLeftMotorSpeed(left);
    setRightMotorSpeed(right);
    setServoAngle(servo);
}

/**
 * @deprecated 没用的版本
 */
void setMotorSpeed(int stdSpeed, int speedBias)
{
    if (speedBias < 0)
    {
        gpio_set_level(MotorL_DIR, 0);
        gpio_set_level(MotorR_DIR, 0);
        pwm_set_duty(MotorL_PWM, stdSpeed - speedBias);
        pwm_set_duty(MotorR_PWM, stdSpeed);
    }
    else
    {
        gpio_set_level(MotorL_DIR, 1);
        gpio_set_level(MotorR_DIR, 1);
        pwm_set_duty(MotorL_PWM, stdSpeed);
        pwm_set_duty(MotorR_PWM, stdSpeed + speedBias);
    }
}