#include <MotorDriver.h>
#include <zf_driver_pwm.h>
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

void setServoAngle(int angle)
{
    if (angle < 0)
    {
        gpio_set_level(MotorServo_DIR, 0);
        pwm_set_duty(MotorServo_PWM, -angle);
    }
    else
    {
        gpio_set_level(MotorServo_DIR, 1);
        pwm_set_duty(MotorServo_PWM, angle);
    }
}
/**
 * @brief   设置电机速度
 * @note 对应的是左轮,3号轮子,顺时针为正方向,此时编码器是正值
 */
void setLeftMotorSpeed(int givenSpeed)
{
    if (givenSpeed < 0)
    {
        gpio_set_level(MotorL_DIR, 0);
        pwm_set_duty(MotorL_PWM, -givenSpeed);
    }
    else
    {
        gpio_set_level(MotorL_DIR, 1);
        pwm_set_duty(MotorL_PWM, givenSpeed);
    }
}
/**
 * @brief   设置电机速度
 * @note 对应的是右轮,1号轮子,顺时针为正方向,此时编码器是正值
 */
void setRightMotorSpeed(int givenSpeed)
{
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