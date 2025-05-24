#include "strategy.h"

/**
 * 我们现在需要的状态：
 * Normal_Mode: 正常循迹不需要推箱子之类的
 * Push_Mode: 检测到红色方块，准备推箱子
 * Stable_Mode: 推箱子稳定状态
 * Rotate_Mode: 车子动中间轮子来转向
 * Dash_Mode: 带着箱子向前冲刺
 * Reset_Mode: 推箱子完成，准备复位
 */
// bool Normal_Mode = true; // 正常模式
// bool Push_State = 0;     // 当检测到红色物块相关参数进入阈值范围时设置为1
// bool Stable_State = 0;   // 当相关参数都在可控范围内就认为稳定了
// bool Rotate_State = 0;   // 当车子在转向
// bool Dash_State = 0;     // 当车子在冲刺
// bool Reset_State = 0;    // 当车子完成任务

void judgeCarStrategy()
{
    detectRedBlock();    // 判断Push_State
    detectBlockStable(); // 判断Stable_State
}
