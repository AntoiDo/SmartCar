#include "island.h"

int Island_State = 0;      // 环岛状态标志
int Left_Island_Flag = 0;  // 左右环岛标志
int Right_Island_Flag = 0; // 左右环岛标志
int island_3_cnt = 0;      // 状态3控制环岛进入的标志位
int Left_Up_Turning[2];    // 四个拐点的坐标存储，[0]存y，第某行，{1}存x，第某列
int Right_Up_Turning[2];   // 四个拐点的坐标存储，[0]存y，第某行，{1}存x，第某列

void islandDetect()
{
    static float k = 0;   // 3和5状态的k
    static float k_1 = 0; // 1状态的k, 找下拐点
    static float k_2 = 0; // 保存k1初始值给状态二用
    static int serveFor2_X = 0;
    static int serveFor2_Y = 0;
    static int island_state_5_down[2] = {0}; // 状态5时即将离开环岛，左右边界边最低点，[0]存y，第某行，{1}存x，第某列
    static int island_state_3_up[2] = {0};   // 状态3时即将进入环岛用，左右上面角点[0]存y，第某行，{1}存x，第某列
    static int left_down_guai[2] = {0};      // 四个拐点的坐标存储，[0]存y，第某行，{1}存x，第某列
    static int right_down_guai[2] = {0};     // 四个拐点的坐标存储，[0]存y，第某行，{1}存x，第某列
    int monotonicity_change_line[2];         // 单调性改变点坐标，[0]寸某行，[1]寸某列
    int monotonicity_change_left_flag = 0;   // 不转折是0
    int monotonicity_change_right_flag = 0;  // 不转折是0
    int continuity_change_right_flag = 0;    // 连续是0
    int continuity_change_left_flag = 0;     // 连续是0
    // 以下是常规判断法
    continuity_change_left_flag = ContinuityChangeLeft(MT9V03X_H - 1 - 5, 10); // 连续性判断
    continuity_change_right_flag = ContinuityChangeRight(MT9V03X_H - 1 - 5, 10);
    monotonicity_change_right_flag = MonotonicityChangeRight(MT9V03X_H - 1 - 10, 10);
    monotonicity_change_left_flag = MonotonicityChangeLeft(MT9V03X_H - 1 - 10, 10);
    if (Cross_Flag == 0 && Island_State == 0 && Ramp_Flag == 0) //&&Ramp_Flag==0
    {
        continuity_change_left_flag = ContinuityChangeLeft(MT9V03X_H - 1 - 5, 10); // 连续性判断
        continuity_change_right_flag = ContinuityChangeRight(MT9V03X_H - 1 - 5, 10);
        if (Left_Island_Flag == 0) // 左环
        {
            if (monotonicity_change_right_flag == 0 &&                                           // 右边是单调的
                continuity_change_left_flag != 0 &&                                              // 左边是不连续的
                continuity_change_right_flag == 0 &&                                             // 左环岛右边是连续的
                Left_Line_Lost_Count >= 10 &&                                                    // 左边丢线很多
                Left_Line_Lost_Count <= 50 &&                                                    // 也不能全丢了
                Right_Line_Lost_Count <= 10 &&                                                   // 右边丢线较少
                Search_Stop_Line >= MT9V03X_H * 0.95 &&                                          // 搜索截止行看到很远
                Left_Boundry_Start >= MT9V03X_H - 20 && Right_Boundry_Start >= MT9V03X_H - 20 && // 边界起始点靠下
                Both_Lost_Times <= 10)                                                           // 双边丢线少
            {
                left_down_guai[0] = findLeftDownPoint(MT9V03X_H - 1, 20); // 找左下角点
                if (left_down_guai[0] >= 30)                              // 条件1很松，在这里判断拐点，位置不对，则是误判，跳出
                {
                    Island_State = 1;
                    Left_Island_Flag = 1;
                }
                else // 误判，归零
                {
                    Island_State = 0;
                    Left_Island_Flag = 0;
                }
            }
        }
        if (Right_Island_Flag == 0) // 右环
        {
            if (monotonicity_change_left_flag == 0 &&
                continuity_change_left_flag == 0 &&                                              // 右环岛左边是连续的
                continuity_change_right_flag != 0 &&                                             // 右边是不连续的
                Right_Line_Lost_Count >= 10 &&                                                   // 右丢线多
                Right_Line_Lost_Count <= 50 &&                                                   // 右丢线不能太多
                Left_Line_Lost_Count <= 10 &&                                                    // 左丢线少
                Search_Stop_Line >= MT9V03X_H * 0.95 &&                                          // 搜索截止行看到很远
                Left_Boundry_Start >= MT9V03X_H - 20 && Right_Boundry_Start >= MT9V03X_H - 20 && // 边界起始点靠下
                Both_Lost_Times <= 10)
            {
                right_down_guai[0] = findRightDownPoint(MT9V03X_H - 1, 20); // 右下点
                if (right_down_guai[0] >= 30)                               // 条件1很松，在这里加判拐点，位置不对，则是误判，跳出
                {
                    Island_State = 1;
                    Right_Island_Flag = 1;
                }
                else
                {
                    Island_State = 0;
                    Right_Island_Flag = 0;
                }
            }
        }
    }
    if (Left_Island_Flag == 1) // 1状态下拐点还在，没丢线
    {
        if (Island_State == 1)
        {
            // 这个是现在左下角的拐点
            monotonicity_change_line[0] = MonotonicityChangeLeft(MT9V03X_H - 1, 30); // 寻找单调性改变点
            monotonicity_change_line[1] = Left_Line[monotonicity_change_line[0]];
            if (!serveFor2_X)
                serveFor2_X = MT9V03X_H - 1;
            if (!serveFor2_Y)
                serveFor2_Y = Left_Line[serveFor2_X];
            // serveFor2_X = monotonicity_change_line[0];
            // serveFor2_Y = monotonicity_change_line[1];
            k_1 = (float)(Left_Line[MT9V03X_H - 5] - Left_Line[MT9V03X_H - 1]) / (-4); // 应该要负的
            if (!k_2)
            {
                k_2 = k_1;
            }
            // k_1 = (float)(monotonicity_change_line[1] - Left_Line[monotonicity_change_line[0] + 4]) / (monotonicity_change_line[0] - (monotonicity_change_line[0] + 4));
            // addLeftLine(MT9V03X_H - 1, (int)(monotonicity_change_line[1] * 0.15), monotonicity_change_line[0], monotonicity_change_line[1]);
            for (int x = monotonicity_change_line[0]; x >= 0; x--)
            {
                // y = k * (x - x1) + y1
                Left_Line[x] = k_2 * (x - monotonicity_change_line[0]) + monotonicity_change_line[1];
                if (Left_Line[x] >= MT9V03X_W - 1)
                    Left_Line[x] = MT9V03X_W - 1;
                else if (Left_Line[x] <= 0)
                    Left_Line[x] = 0;
            }
            if ((Island_State == 1) && (Left_Boundry_Start <= 30)) // 下方当丢线时候进2
            {
                Island_State = 2;
            }
        }

        else if (Island_State == 2) // 下方角点消失，2状态时下方应该是丢线，上面是弧线
        {
            monotonicity_change_line[0] = MonotonicityChangeLeft(70, 12); // 寻找单调性改变点
            monotonicity_change_line[1] = Left_Line[monotonicity_change_line[0]];
            if (monotonicity_change_line[0] == 0)
            {
                for (int x = serveFor2_X; x >= 0; x--)
                {
                    // y = k * (x - x1) + y1
                    Left_Line[x] = k_2 * (x - serveFor2_X) + serveFor2_Y;
                    if (Left_Line[x] >= MT9V03X_W - 1)
                        Left_Line[x] = MT9V03X_W - 1;
                    else if (Left_Line[x] <= 0)
                        Left_Line[x] = 0;
                }
            }
            else
            {
                addLeftLine(MT9V03X_H - 1, (int)(monotonicity_change_line[1] * 0.1), monotonicity_change_line[0], monotonicity_change_line[1]);
            }
            if (Island_State == 2 && (Left_Boundry_Start >= MT9V03X_H - 5 && monotonicity_change_line[0] >= 45)) // 当圆弧靠下时候，进3
            {
                Island_State = 3; // 最长白列寻找范围也要改，见camera.c
                Left_Island_Flag = 1;
            }
        }
        else if (Island_State == 3) // 3状态准备进环，寻找上拐点，连线
        {
            if (k != 0)
            {
                drawLineWithSlope(k, MT9V03X_H - 1, MT9V03X_W - 30, 0); // k是刚刚算出来的，静态变量存着
                longestWhiteColumnSearchLines();                        // 刷新边界数据
            }
            else
            {
                Left_Up_Turning[0] = findLeftUpPoint(40, 5); // 找左上拐点
                Left_Up_Turning[1] = Left_Line[Left_Up_Turning[0]];
                // if (Left_Up_Turning[0] < 5) // 此处为了防止误判，如果经常从3状态归零，建议修改此处判断条件
                // {
                //     Island_State = 0;
                //     Left_Island_Flag = 0;
                // }

                if (k == 0 && (12 <= Left_Up_Turning[0] && Left_Up_Turning[0] < 50) && (50 < Left_Up_Turning[1] && Left_Up_Turning[1] < 110)) // 拐点出现在一定范围内，认为是拐点出现
                {
                    island_state_3_up[0] = Left_Up_Turning[0];
                    island_state_3_up[1] = Left_Up_Turning[1];
                    k = (float)(MT9V03X_W - 20 - island_state_3_up[1]) / (float)(MT9V03X_H - 1 - island_state_3_up[0]);
                    // k = (float)((float)(MT9V03X_H - island_state_3_up[0]) / (float)(MT9V03X_W - 20 - island_state_3_up[1]));
                    drawLineWithSlope(k, MT9V03X_H - 1, MT9V03X_W - 20, 0); // 记录下第一次上点出现时位置，针对这个环岛拉一条死线，入环
                    longestWhiteColumnSearchLines();                        // 刷新边界数据
                }
            }
            // todo:这一步在实际工程中换成下面的一句
            if ((Island_State == 3) && island_3_cnt++ >= 132) // 纯靠陀螺仪积分入环
            {
                k = 0;                           // 斜率清零
                Island_State = 4;                // 这一步时候顺便调整了最长白列的搜索范围
                longestWhiteColumnSearchLines(); // 最长白列
            }
        }
        else if (Island_State == 4) // 状态4已经在里面
        {
            monotonicity_change_line[0] = MonotonicityChangeRight(MT9V03X_H - 10, 10); // 单调性改变
            monotonicity_change_line[1] = Right_Line[monotonicity_change_line[0]];
            // todo:replace
            if (monotonicity_change_line[0] >= 35)
            // if (abs(FJ_Angle) > 180) // 积分200度以后在打开出环判断
            {
                monotonicity_change_line[0] = MonotonicityChangeRight(MT9V03X_H - 10, 10); // 单调性改变
                monotonicity_change_line[1] = Right_Line[monotonicity_change_line[0]];
                if ((Island_State == 4) && (35 <= monotonicity_change_line[0] && monotonicity_change_line[0] <= 55 && monotonicity_change_line[1] >= 10)) // 单调点靠下，进去5
                {                                                                                                                                         // monotonicity_change_line[1]>=90&&
                    island_state_5_down[0] = MT9V03X_H - 1;
                    island_state_5_down[1] = Right_Line[MT9V03X_H - 1];
                    k = (float)(float)(island_state_5_down[1] - monotonicity_change_line[1]) / (MT9V03X_H - monotonicity_change_line[0]);
                    // k = (float)((float)(MT9V03X_H - monotonicity_change_line[0]) / (float)(island_state_5_down[1] - monotonicity_change_line[1]));
                    drawRightBoundryWithSlope(k, island_state_5_down[0], island_state_5_down[1], 0); // 和状态3一样，记住斜率
                    Island_State = 5;
                }
            }
        }
        else if (Island_State == 5) // 出环
        {
            drawRightBoundryWithSlope(k, island_state_5_down[0], island_state_5_down[1], 0);
            if ((Island_State == 5) && (Right_Boundry_Start < MT9V03X_H - 20)) // 右边先丢线
            {
                Island_State = 6;
            }
        }
        else if (Island_State == 6) // 还在出
        {
            drawRightBoundryWithSlope(k, island_state_5_down[0], island_state_5_down[1], 0);
            if ((Island_State == 6) && (Right_Boundry_Start > MT9V03X_H - 10))
            // todo:换成下面一句
            // if ((Island_State == 6) && ((Right_Boundry_Start > MT9V03X_H - 10) || (abs(FJ_Angle) >= 320))) // 右边不丢线
            { //
                k = 0;
                Island_State = 7;
            }
        }
        else if (Island_State == 7) // 基本出去了，在寻找拐点，准备离开环岛状态
        {
            Left_Up_Turning[0] = findLeftUpPoint(MT9V03X_H - 10, 0); // 获取左上点坐标，坐标点合理去8
            Left_Up_Turning[1] = Left_Line[Left_Up_Turning[0]];
            if ((Island_State == 7) && (Left_Up_Turning[1] <= 100) && (5 <= Left_Up_Turning[0] && Left_Up_Turning[0] <= MT9V03X_H - 20)) // 注意这里，对横纵坐标都有要求
            {
                Island_State = 8; // 基本上找到拐点就去8
            }
        }
        else if (Island_State == 8) // 连线，出环最后一步
        {
            Left_Up_Turning[0] = findLeftUpPoint(MT9V03X_H - 1, 10); // 获取左上点坐标
            Left_Up_Turning[1] = Left_Line[Left_Up_Turning[0]];
            lengthenLeftBoundry(Left_Up_Turning[0] - 1, MT9V03X_H - 1);
            if ((Island_State == 8) && (Left_Up_Turning[0] >= MT9V03X_H - 20 || (Left_Up_Turning[0] < 10 && Left_Boundry_Start >= MT9V03X_H - 10))) // 当拐点靠下时候，认为出环了，环岛结束
            {                                                                                                                                       // 要么拐点靠下，要么拐点丢了，切下方不丢线，认为环岛结束了
                // FJ_Angle = 0;                                                                                                                       // 数据清零
                Island_State = 0;                                                                                                                   // 8时候环岛基本结束了，为了防止连续判环，8后会进9，大概几十毫秒后归零，
                Left_Island_Flag = 0;
                // clear data
                k = 0;
                k_1 = 0;
                k_2 = 0;
                serveFor2_X = 0;
                serveFor2_Y = 0;
                island_3_cnt = 0;
            }
        }
    }
    else if (Right_Island_Flag == 1)
    {
        if (Island_State == 1) // 1状态下拐点还在，没丢线
        {
            monotonicity_change_line[0] = MonotonicityChangeRight(MT9V03X_H - 1, 30); // 单调性改变
            monotonicity_change_line[1] = Right_Line[monotonicity_change_line[0]];
            if (!serveFor2_X)
                serveFor2_X = MT9V03X_H - 1;
            if (!serveFor2_Y)
                serveFor2_Y = Right_Line[serveFor2_X];
            // serveFor2_X = monotonicity_change_line[0];
            // serveFor2_Y = monotonicity_change_line[1];
            k_1 = (float)(Right_Line[MT9V03X_H - 1] - Right_Line[MT9V03X_H - 5]) / (4);
            if (!k_2)
            {
                k_2 = k_1;
            }
            // k_1 = (float)(monotonicity_change_line[1] - Right_Line[monotonicity_change_line[0] + 4]) / (monotonicity_change_line[0] - (monotonicity_change_line[0] - 4));
            // addRightLine(MT9V03X_H - 1, (int)(MT9V03X_W - 1 - (monotonicity_change_line[1] * 0.15)), monotonicity_change_line[0], monotonicity_change_line[1]);
            for (int x = monotonicity_change_line[0]; x >= 0; x--)
            {
                // y = k * (x - x1) + y1
                Right_Line[x] = k_2 * (x - monotonicity_change_line[0]) + monotonicity_change_line[1];
                if (Right_Line[x] >= MT9V03X_W - 1)
                    Right_Line[x] = MT9V03X_W - 1;
                else if (Right_Line[x] <= 0)
                    Right_Line[x] = 0;
            }
            if (Island_State == 1 && Right_Boundry_Start <= 30) // 右下角先丢线
            {
                Island_State = 2;
            }
        }
        else if (Island_State == 2) // 2状态下方丢线，上方即将出现大弧线
        {
            monotonicity_change_line[0] = MonotonicityChangeRight(70, 10); // 单调性改变
            monotonicity_change_line[1] = Right_Line[monotonicity_change_line[0]];
            if (monotonicity_change_line[0] == 0)
            {
                for (int x = serveFor2_X; x >= 0; x--)
                {
                    // y = k * (x - x1) + y1
                    Right_Line[x] = k_2 * (x - serveFor2_X) + serveFor2_Y - 10;
                    if (Right_Line[x] >= MT9V03X_W - 1)
                        Right_Line[x] = MT9V03X_W - 1;
                    else if (Right_Line[x] <= 0)
                        Right_Line[x] = 0;
                }
            }
            else
            {
                addRightLine(MT9V03X_H - 1, (int)(MT9V03X_W - 1 - (monotonicity_change_line[1] * 0.15)), monotonicity_change_line[0], monotonicity_change_line[1]);
            }
            //            if(Island_State==2&&(Boundry_Start_Right>=MT9V03X_H-10))//右下角再不丢线进3
            if (Island_State == 2 && (Right_Boundry_Start >= MT9V03X_H - 5 && monotonicity_change_line[0] >= 45)) // 右下角再不丢线进3
            {
                Island_State = 3; // 下方丢线，说明大弧线已经下来了
                Right_Island_Flag = 1;
            }
        }
        else if (Island_State == 3) // 下面已经出现大弧线，且上方出现角点
        {
            if (k != 0) // 将角点与下方连接，画一条死线
            {
                drawLineWithSlope(k, MT9V03X_H - 1, 30, 0);
                longestWhiteColumnSearchLines(); // 刷新最长白列
            }
            else
            {
                Right_Up_Turning[0] = findRightUpPoint(40, 5); // 找右上拐点
                Right_Up_Turning[1] = Right_Line[Right_Up_Turning[0]];
                // if (Right_Up_Turning[0] < 5) // 这里改过，此处为了防止环岛误判，如果经常出现环岛3归零，请修改此处判断条件
                // {
                //     Island_State = 0;
                //     Right_Island_Flag = 0;
                // }

                if (k == 0 && (12 <= Right_Up_Turning[0] && Right_Up_Turning[0] < 50) && (70 < Right_Up_Turning[1] && Right_Up_Turning[1] < 150)) // 找第一个符合条件的角点，连线
                {
                    island_state_3_up[0] = Right_Up_Turning[0];
                    island_state_3_up[1] = Right_Up_Turning[1];
                    k = (float)(20 - island_state_3_up[1]) / (MT9V03X_H - 1 - island_state_3_up[0]);
                    // k = (float)((float)(MT9V03X_H - island_state_3_up[0]) / (float)(20 - island_state_3_up[1]));
                    drawLineWithSlope(k, MT9V03X_H - 1, 20, 0);
                    longestWhiteColumnSearchLines(); // 刷新赛道数据
                }
            }
            // todo: 工程里换成下面注释的一句

            if ((Island_State == 3) && island_3_cnt++ >= 132) // 只依靠陀螺仪积分
            {
                k = 0; // 斜率清零
                Island_State = 4;
                longestWhiteColumnSearchLines(); // 最长白列
            } // 记得去最长白列那边改一下，区分下左右环岛
        }
        else if (Island_State == 4) // 4状态完全进去环岛了
        {
            monotonicity_change_line[0] = MonotonicityChangeLeft(90, 10); // 单调性改变
            monotonicity_change_line[1] = Left_Line[monotonicity_change_line[0]];
            if (monotonicity_change_line[0] >= 38)
            // todo:实际工程用下面一句
            // if (FJ_Angle > 200) // 环岛积分200度后再打开单调转折判断
            {
                monotonicity_change_line[0] = MonotonicityChangeLeft(90, 10); // 单调性改变
                monotonicity_change_line[1] = Left_Line[monotonicity_change_line[0]];
                if ((Island_State == 4) && (30 <= monotonicity_change_line[0] && monotonicity_change_line[0] <= 48 && monotonicity_change_line[1] < MT9V03X_W - 10)) // 单调点靠下，进去5
                {                                                                                                                                                    // monotonicity_change_line[1]<=120&&
                    island_state_5_down[0] = MT9V03X_H - 1;
                    island_state_5_down[1] = Left_Line[MT9V03X_H - 1] - 5; // 抓住第一次出现的斜率，定死
                    k = (float)(monotonicity_change_line[1] - island_state_5_down[1]) / (float)(monotonicity_change_line[0] - island_state_5_down[0]);

                    drawLeftBoundryWithSlope(k, island_state_5_down[0], island_state_5_down[1], 0);
                    Island_State = 5;
                }
            }
        }
        else if (Island_State == 5) // 准备出环岛
        {
            drawLeftBoundryWithSlope(k, island_state_5_down[0], island_state_5_down[1], 0);
            if (Island_State == 5 && Left_Boundry_Start < MT9V03X_H - 20) // 左边先丢线
            {
                Island_State = 6;
            }
        }
        else if (Island_State == 6) // 继续出
        {
            drawLeftBoundryWithSlope(k, island_state_5_down[0], island_state_5_down[1], 0);
            // todo:实际工程用下面
            if ((Island_State == 6) && (Left_Boundry_Start > MT9V03X_H - 10)) // 左边先丢线
            {                                                                 //
                k = 0;
                Island_State = 7;
            }
        }
        else if (Island_State == 7) // 基本出环岛，找角点
        {
            Right_Up_Turning[0] = findRightUpPoint(MT9V03X_H - 10, 0); // 获取左上点坐标，找到了去8
            Right_Up_Turning[1] = Right_Line[Right_Up_Turning[0]];
            if ((Island_State == 7) && ((Right_Up_Turning[1] >= MT9V03X_W - 88 && (5 <= Right_Up_Turning[0] && Right_Up_Turning[0] <= MT9V03X_H - 20)))) // 注意这里，对横纵坐标都有要求，因为赛道不一样，会意外出现拐点
            {                                                                                                                                            // 当角点位置合理时，进8
                Island_State = 8;
            }
        }
        else if (Island_State == 8) // 环岛8
        {
            Right_Up_Turning[0] = findRightUpPoint(MT9V03X_H - 1, 10); // 获取右上点坐标
            Right_Up_Turning[1] = Right_Line[Right_Up_Turning[0]];
            lengthenRightBoundry(Right_Up_Turning[0] - 1, MT9V03X_H - 1);
            if ((Island_State == 8) && (Right_Up_Turning[0] >= MT9V03X_H - 20 || (Right_Up_Turning[0] < 10 && Left_Boundry_Start >= MT9V03X_H - 10))) // 当拐点靠下时候，认为出环了，环岛结束
            {                                                                                                                                         // 角点靠下，或者下端不丢线，认为出环了
                // FJ_Angle = 0;
                Island_State = 0;
                Right_Island_Flag = 0;
                // clear data
                k = 0;
                k_1 = 0;
                k_2 = 0;
                serveFor2_X = 0;
                serveFor2_Y = 0;
                island_3_cnt = 0;
            }
        }
    }
}
/**
 * @brief 检测左边界的连续性是否发生变化(比如发生断点之类)
 * @param start
 * @param end
 * @return     连续返回0
 */
int ContinuityChangeLeft(int start, int end)
{
    int i;
    int continuity_change_flag = 0;
    if (start >= MT9V03X_H - 1 - 5)
        start = MT9V03X_H - 1 - 5;
    if (end <= 5)
        end = 5;

    for (i = start; i >= end; i--)
    {
        if (abs(Left_Line[i] - Left_Line[i - 1]) >= 5) // 连续判断阈值是5,可更改
        {
            continuity_change_flag = i;
            break;
        }
    }
    return continuity_change_flag;
}
/**
 * @brief 检测右边界的连续性是否发生变化(比如发生断点之类
 * @param start
 * @param end
 * @return      连续返回0
 */
int ContinuityChangeRight(int start, int end)
{
    int i;
    int continuity_change_flag = 0;
    if (start >= MT9V03X_H - 1 - 5)
        start = MT9V03X_H - 1 - 5;
    if (end <= 5)
        end = 5;

    for (i = start; i >= end; i--)
    {
        if (abs(Right_Line[i] - Right_Line[i - 1]) >= 5) // 连续判断阈值是5,可更改
        {
            continuity_change_flag = i;
            break;
        }
    }
    return continuity_change_flag;
}

/**
 * @brief 检测左边界的单调性
 * @param start
 * @param end
 * @return 点所在的行数，找不到返回0
 */
int MonotonicityChangeLeft(int start, int end)
{
    int i;
    int monotonicity_change_line = 0;
    //    if (Left_Line_Lost_Count >= 0.9 * MT9V03X_H) // 大部分都丢线，没有单调性判断的意义
    //        return monotonicity_change_line;
    if (start >= MT9V03X_H - 1 - 5) // 数组越界保护，在判断第i个点时
        start = MT9V03X_H - 1 - 5;  // 要访问它前后5个点，数组两头的点要不能作为起点终点
    if (end <= 5)
        end = 5;
    if (start <= end) // 递减计算，入口反了，直接返回0
        return monotonicity_change_line;
    for (i = start; i >= end; i--) // 会读取前5后5数据，所以前面对输入范围有要求
    {
        if (Left_Line[i] == Left_Line[i + 5] && Left_Line[i] == Left_Line[i - 5] &&
            Left_Line[i] == Left_Line[i + 4] && Left_Line[i] == Left_Line[i - 4] &&
            Left_Line[i] == Left_Line[i + 3] && Left_Line[i] == Left_Line[i - 3] &&
            Left_Line[i] == Left_Line[i + 2] && Left_Line[i] == Left_Line[i - 2] &&
            Left_Line[i] == Left_Line[i + 1] && Left_Line[i] == Left_Line[i - 1])
        { // 一堆数据一样，显然不能作为单调转折点
            continue;
        }
        else if (Left_Line[i] >= Left_Line[i + 5] && Left_Line[i] >= Left_Line[i - 5] &&
                 Left_Line[i] >= Left_Line[i + 4] && Left_Line[i] >= Left_Line[i - 4] &&
                 Left_Line[i] >= Left_Line[i + 3] && Left_Line[i] >= Left_Line[i - 3] &&
                 Left_Line[i] >= Left_Line[i + 2] && Left_Line[i] >= Left_Line[i - 2] &&
                 Left_Line[i] >= Left_Line[i + 1] && Left_Line[i] >= Left_Line[i - 1])
        { // 就很暴力，这个数据是在前5，后5中最大的（可以取等），那就是单调突变点
            monotonicity_change_line = i;
            break;
        }
    }
    return monotonicity_change_line;
}
/**
 * @brief 检测右边界的单调性
 * @param start
 * @param end
 * @return 点所在的行数，找不到返回0
 */
int MonotonicityChangeRight(int start, int end)
{
    int i;
    int monotonicity_change_line = 0;

    //    if (Right_Line_Lost_Count >= 50) // 大部分都丢线，没有单调性判断的意义
    //        return monotonicity_change_line;
    if (start >= MT9V03X_H - 1 - 5) // 数组越界保护
        start = MT9V03X_H - 1 - 5;
    if (end <= 5)
        end = 5;
    if (start <= end)
        return monotonicity_change_line;
    for (i = start; i >= end; i--) // 会读取前5后5数据，所以前面对输入范围有要求
    {
        if (Right_Line[i] == Right_Line[i + 5] && Right_Line[i] == Right_Line[i - 5] &&
            Right_Line[i] == Right_Line[i + 4] && Right_Line[i] == Right_Line[i - 4] &&
            Right_Line[i] == Right_Line[i + 3] && Right_Line[i] == Right_Line[i - 3] &&
            Right_Line[i] == Right_Line[i + 2] && Right_Line[i] == Right_Line[i - 2] &&
            Right_Line[i] == Right_Line[i + 1] && Right_Line[i] == Right_Line[i - 1])
        { // 一堆数据一样，显然不能作为单调转折点
            continue;
        }
        else if (Right_Line[i] <= Right_Line[i + 5] && Right_Line[i] <= Right_Line[i - 5] &&
                 Right_Line[i] <= Right_Line[i + 4] && Right_Line[i] <= Right_Line[i - 4] &&
                 Right_Line[i] <= Right_Line[i + 3] && Right_Line[i] <= Right_Line[i - 3] &&
                 Right_Line[i] <= Right_Line[i + 2] && Right_Line[i] <= Right_Line[i - 2] &&
                 Right_Line[i] <= Right_Line[i + 1] && Right_Line[i] <= Right_Line[i - 1])
        { // 就很暴力，这个数据是在前5，后5中最大的，那就是单调突变点
            monotonicity_change_line = i;
            break;
        }
    }
    return monotonicity_change_line;
}
/**
 * @brief 在imageBinary数组里面补线
 * @param slope
 * @param startX
 * @param startY
 * @param endX
 */
void drawLineWithSlope(float slope, int startX, int startY, int endX)
{
    if (startX >= MT9V03X_H - 1) // 限幅处理
        startX = MT9V03X_H - 1;
    else if (startX <= 0)
        startX = 0;
    if (startY >= MT9V03X_W - 1)
        startY = MT9V03X_W - 1;
    else if (startY <= 0)
        startY = 0;
    if (endX >= MT9V03X_H - 1)
        endX = MT9V03X_H - 1;
    else if (endX <= 0)
        endX = 0;

    int endY = startY + slope * (endX - startX);
    drawLine(startX, startY, endX, endY);
}
/**
 * @brief 补得线直接贴在边线上
 * @param slope
 * @param startX
 * @param startY
 * @param endX
 */
void drawLeftBoundryWithSlope(float slope, int startX, int startY, int endX)
{
    if (startY >= MT9V03X_W - 1)
        startY = MT9V03X_W - 1;
    else if (startY <= 0)
        startY = 0;
    if (endX >= MT9V03X_H - 1)
        endX = MT9V03X_H - 1;
    else if (endX <= 0)
        endX = 0;

    int endY = startY + slope * (endX - startX);
    addLeftLine(startX, startY, endX, endY);
}
/**
 * @brief 传的点记得是下面的, endX是上面的
 * @param slope
 * @param startX
 * @param startY
 * @param endX
 */
void drawRightBoundryWithSlope(float slope, int startX, int startY, int endX)
{
    if (startY >= MT9V03X_W - 1)
        startY = MT9V03X_W - 1;
    else if (startY <= 0)
        startY = 0;
    if (endX >= MT9V03X_H - 1)
        endX = MT9V03X_H - 1;
    else if (endX <= 0)
        endX = 0;

    int endY = startY + slope * (endX - startX);
    addRightLine(startX, startY, endX, endY);
}
