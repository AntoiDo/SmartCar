#include "island.h"

int Island_State = 0;      // ����״̬��־
int Left_Island_Flag = 0;  // ���һ�����־
int Right_Island_Flag = 0; // ���һ�����־
int island_3_cnt = 0;      // ״̬3���ƻ�������ı�־λ
int Left_Up_Turning[2];    // �ĸ��յ������洢��[0]��y����ĳ�У�{1}��x����ĳ��
int Right_Up_Turning[2];   // �ĸ��յ������洢��[0]��y����ĳ�У�{1}��x����ĳ��

void islandDetect()
{
    static float k = 0;   // 3��5״̬��k
    static float k_1 = 0; // 1״̬��k, ���¹յ�
    static float k_2 = 0; // ����k1��ʼֵ��״̬����
    static int serveFor2_X = 0;
    static int serveFor2_Y = 0;
    static int island_state_5_down[2] = {0}; // ״̬5ʱ�����뿪���������ұ߽����͵㣬[0]��y����ĳ�У�{1}��x����ĳ��
    static int island_state_3_up[2] = {0};   // ״̬3ʱ�������뻷���ã���������ǵ�[0]��y����ĳ�У�{1}��x����ĳ��
    static int left_down_guai[2] = {0};      // �ĸ��յ������洢��[0]��y����ĳ�У�{1}��x����ĳ��
    static int right_down_guai[2] = {0};     // �ĸ��յ������洢��[0]��y����ĳ�У�{1}��x����ĳ��
    int monotonicity_change_line[2];         // �����Ըı�����꣬[0]��ĳ�У�[1]��ĳ��
    int monotonicity_change_left_flag = 0;   // ��ת����0
    int monotonicity_change_right_flag = 0;  // ��ת����0
    int continuity_change_right_flag = 0;    // ������0
    int continuity_change_left_flag = 0;     // ������0
    // �����ǳ����жϷ�
    continuity_change_left_flag = ContinuityChangeLeft(MT9V03X_H - 1 - 5, 10); // �������ж�
    continuity_change_right_flag = ContinuityChangeRight(MT9V03X_H - 1 - 5, 10);
    monotonicity_change_right_flag = MonotonicityChangeRight(MT9V03X_H - 1 - 10, 10);
    monotonicity_change_left_flag = MonotonicityChangeLeft(MT9V03X_H - 1 - 10, 10);
    if (Cross_Flag == 0 && Island_State == 0 && Ramp_Flag == 0) //&&Ramp_Flag==0
    {
        continuity_change_left_flag = ContinuityChangeLeft(MT9V03X_H - 1 - 5, 10); // �������ж�
        continuity_change_right_flag = ContinuityChangeRight(MT9V03X_H - 1 - 5, 10);
        if (Left_Island_Flag == 0) // ��
        {
            if (monotonicity_change_right_flag == 0 &&                                           // �ұ��ǵ�����
                continuity_change_left_flag != 0 &&                                              // ����ǲ�������
                continuity_change_right_flag == 0 &&                                             // �󻷵��ұ���������
                Left_Line_Lost_Count >= 10 &&                                                    // ��߶��ߺܶ�
                Left_Line_Lost_Count <= 50 &&                                                    // Ҳ����ȫ����
                Right_Line_Lost_Count <= 10 &&                                                   // �ұ߶��߽���
                Search_Stop_Line >= MT9V03X_H * 0.95 &&                                          // ������ֹ�п�����Զ
                Left_Boundry_Start >= MT9V03X_H - 20 && Right_Boundry_Start >= MT9V03X_H - 20 && // �߽���ʼ�㿿��
                Both_Lost_Times <= 10)                                                           // ˫�߶�����
            {
                left_down_guai[0] = findLeftDownPoint(MT9V03X_H - 1, 20); // �����½ǵ�
                if (left_down_guai[0] >= 30)                              // ����1���ɣ��������жϹյ㣬λ�ò��ԣ��������У�����
                {
                    Island_State = 1;
                    Left_Island_Flag = 1;
                }
                else // ���У�����
                {
                    Island_State = 0;
                    Left_Island_Flag = 0;
                }
            }
        }
        if (Right_Island_Flag == 0) // �һ�
        {
            if (monotonicity_change_left_flag == 0 &&
                continuity_change_left_flag == 0 &&                                              // �һ��������������
                continuity_change_right_flag != 0 &&                                             // �ұ��ǲ�������
                Right_Line_Lost_Count >= 10 &&                                                   // �Ҷ��߶�
                Right_Line_Lost_Count <= 50 &&                                                   // �Ҷ��߲���̫��
                Left_Line_Lost_Count <= 10 &&                                                    // ������
                Search_Stop_Line >= MT9V03X_H * 0.95 &&                                          // ������ֹ�п�����Զ
                Left_Boundry_Start >= MT9V03X_H - 20 && Right_Boundry_Start >= MT9V03X_H - 20 && // �߽���ʼ�㿿��
                Both_Lost_Times <= 10)
            {
                right_down_guai[0] = findRightDownPoint(MT9V03X_H - 1, 20); // ���µ�
                if (right_down_guai[0] >= 30)                               // ����1���ɣ���������йյ㣬λ�ò��ԣ��������У�����
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
    if (Left_Island_Flag == 1) // 1״̬�¹յ㻹�ڣ�û����
    {
        if (Island_State == 1)
        {
            // ������������½ǵĹյ�
            monotonicity_change_line[0] = MonotonicityChangeLeft(MT9V03X_H - 1, 30); // Ѱ�ҵ����Ըı��
            monotonicity_change_line[1] = Left_Line[monotonicity_change_line[0]];
            if (!serveFor2_X)
                serveFor2_X = MT9V03X_H - 1;
            if (!serveFor2_Y)
                serveFor2_Y = Left_Line[serveFor2_X];
            // serveFor2_X = monotonicity_change_line[0];
            // serveFor2_Y = monotonicity_change_line[1];
            k_1 = (float)(Left_Line[MT9V03X_H - 5] - Left_Line[MT9V03X_H - 1]) / (-4); // Ӧ��Ҫ����
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
            if ((Island_State == 1) && (Left_Boundry_Start <= 30)) // �·�������ʱ���2
            {
                Island_State = 2;
            }
        }

        else if (Island_State == 2) // �·��ǵ���ʧ��2״̬ʱ�·�Ӧ���Ƕ��ߣ������ǻ���
        {
            monotonicity_change_line[0] = MonotonicityChangeLeft(70, 12); // Ѱ�ҵ����Ըı��
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
            if (Island_State == 2 && (Left_Boundry_Start >= MT9V03X_H - 5 && monotonicity_change_line[0] >= 45)) // ��Բ������ʱ�򣬽�3
            {
                Island_State = 3; // �����Ѱ�ҷ�ΧҲҪ�ģ���camera.c
                Left_Island_Flag = 1;
            }
        }
        else if (Island_State == 3) // 3״̬׼��������Ѱ���Ϲյ㣬����
        {
            if (k != 0)
            {
                drawLineWithSlope(k, MT9V03X_H - 1, MT9V03X_W - 30, 0); // k�Ǹո�������ģ���̬��������
                longestWhiteColumnSearchLines();                        // ˢ�±߽�����
            }
            else
            {
                Left_Up_Turning[0] = findLeftUpPoint(40, 5); // �����Ϲյ�
                Left_Up_Turning[1] = Left_Line[Left_Up_Turning[0]];
                // if (Left_Up_Turning[0] < 5) // �˴�Ϊ�˷�ֹ���У����������3״̬���㣬�����޸Ĵ˴��ж�����
                // {
                //     Island_State = 0;
                //     Left_Island_Flag = 0;
                // }

                if (k == 0 && (12 <= Left_Up_Turning[0] && Left_Up_Turning[0] < 50) && (50 < Left_Up_Turning[1] && Left_Up_Turning[1] < 110)) // �յ������һ����Χ�ڣ���Ϊ�ǹյ����
                {
                    island_state_3_up[0] = Left_Up_Turning[0];
                    island_state_3_up[1] = Left_Up_Turning[1];
                    k = (float)(MT9V03X_W - 20 - island_state_3_up[1]) / (float)(MT9V03X_H - 1 - island_state_3_up[0]);
                    // k = (float)((float)(MT9V03X_H - island_state_3_up[0]) / (float)(MT9V03X_W - 20 - island_state_3_up[1]));
                    drawLineWithSlope(k, MT9V03X_H - 1, MT9V03X_W - 20, 0); // ��¼�µ�һ���ϵ����ʱλ�ã�������������һ�����ߣ��뻷
                    longestWhiteColumnSearchLines();                        // ˢ�±߽�����
                }
            }
            // todo:��һ����ʵ�ʹ����л��������һ��
            if ((Island_State == 3) && island_3_cnt++ >= 132) // ���������ǻ����뻷
            {
                k = 0;                           // б������
                Island_State = 4;                // ��һ��ʱ��˳�����������е�������Χ
                longestWhiteColumnSearchLines(); // �����
            }
        }
        else if (Island_State == 4) // ״̬4�Ѿ�������
        {
            monotonicity_change_line[0] = MonotonicityChangeRight(MT9V03X_H - 10, 10); // �����Ըı�
            monotonicity_change_line[1] = Right_Line[monotonicity_change_line[0]];
            // todo:replace
            if (monotonicity_change_line[0] >= 35)
            // if (abs(FJ_Angle) > 180) // ����200���Ժ��ڴ򿪳����ж�
            {
                monotonicity_change_line[0] = MonotonicityChangeRight(MT9V03X_H - 10, 10); // �����Ըı�
                monotonicity_change_line[1] = Right_Line[monotonicity_change_line[0]];
                if ((Island_State == 4) && (35 <= monotonicity_change_line[0] && monotonicity_change_line[0] <= 55 && monotonicity_change_line[1] >= 10)) // �����㿿�£���ȥ5
                {                                                                                                                                         // monotonicity_change_line[1]>=90&&
                    island_state_5_down[0] = MT9V03X_H - 1;
                    island_state_5_down[1] = Right_Line[MT9V03X_H - 1];
                    k = (float)(float)(island_state_5_down[1] - monotonicity_change_line[1]) / (MT9V03X_H - monotonicity_change_line[0]);
                    // k = (float)((float)(MT9V03X_H - monotonicity_change_line[0]) / (float)(island_state_5_down[1] - monotonicity_change_line[1]));
                    drawRightBoundryWithSlope(k, island_state_5_down[0], island_state_5_down[1], 0); // ��״̬3һ������סб��
                    Island_State = 5;
                }
            }
        }
        else if (Island_State == 5) // ����
        {
            drawRightBoundryWithSlope(k, island_state_5_down[0], island_state_5_down[1], 0);
            if ((Island_State == 5) && (Right_Boundry_Start < MT9V03X_H - 20)) // �ұ��ȶ���
            {
                Island_State = 6;
            }
        }
        else if (Island_State == 6) // ���ڳ�
        {
            drawRightBoundryWithSlope(k, island_state_5_down[0], island_state_5_down[1], 0);
            if ((Island_State == 6) && (Right_Boundry_Start > MT9V03X_H - 10))
            // todo:��������һ��
            // if ((Island_State == 6) && ((Right_Boundry_Start > MT9V03X_H - 10) || (abs(FJ_Angle) >= 320))) // �ұ߲�����
            { //
                k = 0;
                Island_State = 7;
            }
        }
        else if (Island_State == 7) // ������ȥ�ˣ���Ѱ�ҹյ㣬׼���뿪����״̬
        {
            Left_Up_Turning[0] = findLeftUpPoint(MT9V03X_H - 10, 0); // ��ȡ���ϵ����꣬��������ȥ8
            Left_Up_Turning[1] = Left_Line[Left_Up_Turning[0]];
            if ((Island_State == 7) && (Left_Up_Turning[1] <= 100) && (5 <= Left_Up_Turning[0] && Left_Up_Turning[0] <= MT9V03X_H - 20)) // ע������Ժ������궼��Ҫ��
            {
                Island_State = 8; // �������ҵ��յ��ȥ8
            }
        }
        else if (Island_State == 8) // ���ߣ��������һ��
        {
            Left_Up_Turning[0] = findLeftUpPoint(MT9V03X_H - 1, 10); // ��ȡ���ϵ�����
            Left_Up_Turning[1] = Left_Line[Left_Up_Turning[0]];
            lengthenLeftBoundry(Left_Up_Turning[0] - 1, MT9V03X_H - 1);
            if ((Island_State == 8) && (Left_Up_Turning[0] >= MT9V03X_H - 20 || (Left_Up_Turning[0] < 10 && Left_Boundry_Start >= MT9V03X_H - 10))) // ���յ㿿��ʱ����Ϊ�����ˣ���������
            {                                                                                                                                       // Ҫô�յ㿿�£�Ҫô�յ㶪�ˣ����·������ߣ���Ϊ����������
                // FJ_Angle = 0;                                                                                                                       // ��������
                Island_State = 0;                                                                                                                   // 8ʱ�򻷵����������ˣ�Ϊ�˷�ֹ�����л���8����9����ż�ʮ�������㣬
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
        if (Island_State == 1) // 1״̬�¹յ㻹�ڣ�û����
        {
            monotonicity_change_line[0] = MonotonicityChangeRight(MT9V03X_H - 1, 30); // �����Ըı�
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
            if (Island_State == 1 && Right_Boundry_Start <= 30) // ���½��ȶ���
            {
                Island_State = 2;
            }
        }
        else if (Island_State == 2) // 2״̬�·����ߣ��Ϸ��������ִ���
        {
            monotonicity_change_line[0] = MonotonicityChangeRight(70, 10); // �����Ըı�
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
            //            if(Island_State==2&&(Boundry_Start_Right>=MT9V03X_H-10))//���½��ٲ����߽�3
            if (Island_State == 2 && (Right_Boundry_Start >= MT9V03X_H - 5 && monotonicity_change_line[0] >= 45)) // ���½��ٲ����߽�3
            {
                Island_State = 3; // �·����ߣ�˵�������Ѿ�������
                Right_Island_Flag = 1;
            }
        }
        else if (Island_State == 3) // �����Ѿ����ִ��ߣ����Ϸ����ֽǵ�
        {
            if (k != 0) // ���ǵ����·����ӣ���һ������
            {
                drawLineWithSlope(k, MT9V03X_H - 1, 30, 0);
                longestWhiteColumnSearchLines(); // ˢ�������
            }
            else
            {
                Right_Up_Turning[0] = findRightUpPoint(40, 5); // �����Ϲյ�
                Right_Up_Turning[1] = Right_Line[Right_Up_Turning[0]];
                // if (Right_Up_Turning[0] < 5) // ����Ĺ����˴�Ϊ�˷�ֹ�������У�����������ֻ���3���㣬���޸Ĵ˴��ж�����
                // {
                //     Island_State = 0;
                //     Right_Island_Flag = 0;
                // }

                if (k == 0 && (12 <= Right_Up_Turning[0] && Right_Up_Turning[0] < 50) && (70 < Right_Up_Turning[1] && Right_Up_Turning[1] < 150)) // �ҵ�һ�����������Ľǵ㣬����
                {
                    island_state_3_up[0] = Right_Up_Turning[0];
                    island_state_3_up[1] = Right_Up_Turning[1];
                    k = (float)(20 - island_state_3_up[1]) / (MT9V03X_H - 1 - island_state_3_up[0]);
                    // k = (float)((float)(MT9V03X_H - island_state_3_up[0]) / (float)(20 - island_state_3_up[1]));
                    drawLineWithSlope(k, MT9V03X_H - 1, 20, 0);
                    longestWhiteColumnSearchLines(); // ˢ����������
                }
            }
            // todo: �����ﻻ������ע�͵�һ��

            if ((Island_State == 3) && island_3_cnt++ >= 132) // ֻ���������ǻ���
            {
                k = 0; // б������
                Island_State = 4;
                longestWhiteColumnSearchLines(); // �����
            } // �ǵ�ȥ������Ǳ߸�һ�£����������һ���
        }
        else if (Island_State == 4) // 4״̬��ȫ��ȥ������
        {
            monotonicity_change_line[0] = MonotonicityChangeLeft(90, 10); // �����Ըı�
            monotonicity_change_line[1] = Left_Line[monotonicity_change_line[0]];
            if (monotonicity_change_line[0] >= 38)
            // todo:ʵ�ʹ���������һ��
            // if (FJ_Angle > 200) // ��������200�Ⱥ��ٴ򿪵���ת���ж�
            {
                monotonicity_change_line[0] = MonotonicityChangeLeft(90, 10); // �����Ըı�
                monotonicity_change_line[1] = Left_Line[monotonicity_change_line[0]];
                if ((Island_State == 4) && (30 <= monotonicity_change_line[0] && monotonicity_change_line[0] <= 48 && monotonicity_change_line[1] < MT9V03X_W - 10)) // �����㿿�£���ȥ5
                {                                                                                                                                                    // monotonicity_change_line[1]<=120&&
                    island_state_5_down[0] = MT9V03X_H - 1;
                    island_state_5_down[1] = Left_Line[MT9V03X_H - 1] - 5; // ץס��һ�γ��ֵ�б�ʣ�����
                    k = (float)(monotonicity_change_line[1] - island_state_5_down[1]) / (float)(monotonicity_change_line[0] - island_state_5_down[0]);

                    drawLeftBoundryWithSlope(k, island_state_5_down[0], island_state_5_down[1], 0);
                    Island_State = 5;
                }
            }
        }
        else if (Island_State == 5) // ׼��������
        {
            drawLeftBoundryWithSlope(k, island_state_5_down[0], island_state_5_down[1], 0);
            if (Island_State == 5 && Left_Boundry_Start < MT9V03X_H - 20) // ����ȶ���
            {
                Island_State = 6;
            }
        }
        else if (Island_State == 6) // ������
        {
            drawLeftBoundryWithSlope(k, island_state_5_down[0], island_state_5_down[1], 0);
            // todo:ʵ�ʹ���������
            if ((Island_State == 6) && (Left_Boundry_Start > MT9V03X_H - 10)) // ����ȶ���
            {                                                                 //
                k = 0;
                Island_State = 7;
            }
        }
        else if (Island_State == 7) // �������������ҽǵ�
        {
            Right_Up_Turning[0] = findRightUpPoint(MT9V03X_H - 10, 0); // ��ȡ���ϵ����꣬�ҵ���ȥ8
            Right_Up_Turning[1] = Right_Line[Right_Up_Turning[0]];
            if ((Island_State == 7) && ((Right_Up_Turning[1] >= MT9V03X_W - 88 && (5 <= Right_Up_Turning[0] && Right_Up_Turning[0] <= MT9V03X_H - 20)))) // ע������Ժ������궼��Ҫ����Ϊ������һ������������ֹյ�
            {                                                                                                                                            // ���ǵ�λ�ú���ʱ����8
                Island_State = 8;
            }
        }
        else if (Island_State == 8) // ����8
        {
            Right_Up_Turning[0] = findRightUpPoint(MT9V03X_H - 1, 10); // ��ȡ���ϵ�����
            Right_Up_Turning[1] = Right_Line[Right_Up_Turning[0]];
            lengthenRightBoundry(Right_Up_Turning[0] - 1, MT9V03X_H - 1);
            if ((Island_State == 8) && (Right_Up_Turning[0] >= MT9V03X_H - 20 || (Right_Up_Turning[0] < 10 && Left_Boundry_Start >= MT9V03X_H - 10))) // ���յ㿿��ʱ����Ϊ�����ˣ���������
            {                                                                                                                                         // �ǵ㿿�£������¶˲����ߣ���Ϊ������
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
 * @brief �����߽���������Ƿ����仯(���緢���ϵ�֮��)
 * @param start
 * @param end
 * @return     ��������0
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
        if (abs(Left_Line[i] - Left_Line[i - 1]) >= 5) // �����ж���ֵ��5,�ɸ���
        {
            continuity_change_flag = i;
            break;
        }
    }
    return continuity_change_flag;
}
/**
 * @brief ����ұ߽���������Ƿ����仯(���緢���ϵ�֮��
 * @param start
 * @param end
 * @return      ��������0
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
        if (abs(Right_Line[i] - Right_Line[i - 1]) >= 5) // �����ж���ֵ��5,�ɸ���
        {
            continuity_change_flag = i;
            break;
        }
    }
    return continuity_change_flag;
}

/**
 * @brief �����߽�ĵ�����
 * @param start
 * @param end
 * @return �����ڵ��������Ҳ�������0
 */
int MonotonicityChangeLeft(int start, int end)
{
    int i;
    int monotonicity_change_line = 0;
    //    if (Left_Line_Lost_Count >= 0.9 * MT9V03X_H) // �󲿷ֶ����ߣ�û�е������жϵ�����
    //        return monotonicity_change_line;
    if (start >= MT9V03X_H - 1 - 5) // ����Խ�籣�������жϵ�i����ʱ
        start = MT9V03X_H - 1 - 5;  // Ҫ������ǰ��5���㣬������ͷ�ĵ�Ҫ������Ϊ����յ�
    if (end <= 5)
        end = 5;
    if (start <= end) // �ݼ����㣬��ڷ��ˣ�ֱ�ӷ���0
        return monotonicity_change_line;
    for (i = start; i >= end; i--) // ���ȡǰ5��5���ݣ�����ǰ������뷶Χ��Ҫ��
    {
        if (Left_Line[i] == Left_Line[i + 5] && Left_Line[i] == Left_Line[i - 5] &&
            Left_Line[i] == Left_Line[i + 4] && Left_Line[i] == Left_Line[i - 4] &&
            Left_Line[i] == Left_Line[i + 3] && Left_Line[i] == Left_Line[i - 3] &&
            Left_Line[i] == Left_Line[i + 2] && Left_Line[i] == Left_Line[i - 2] &&
            Left_Line[i] == Left_Line[i + 1] && Left_Line[i] == Left_Line[i - 1])
        { // һ������һ������Ȼ������Ϊ����ת�۵�
            continue;
        }
        else if (Left_Line[i] >= Left_Line[i + 5] && Left_Line[i] >= Left_Line[i - 5] &&
                 Left_Line[i] >= Left_Line[i + 4] && Left_Line[i] >= Left_Line[i - 4] &&
                 Left_Line[i] >= Left_Line[i + 3] && Left_Line[i] >= Left_Line[i - 3] &&
                 Left_Line[i] >= Left_Line[i + 2] && Left_Line[i] >= Left_Line[i - 2] &&
                 Left_Line[i] >= Left_Line[i + 1] && Left_Line[i] >= Left_Line[i - 1])
        { // �ͺܱ����������������ǰ5����5�����ģ�����ȡ�ȣ����Ǿ��ǵ���ͻ���
            monotonicity_change_line = i;
            break;
        }
    }
    return monotonicity_change_line;
}
/**
 * @brief ����ұ߽�ĵ�����
 * @param start
 * @param end
 * @return �����ڵ��������Ҳ�������0
 */
int MonotonicityChangeRight(int start, int end)
{
    int i;
    int monotonicity_change_line = 0;

    //    if (Right_Line_Lost_Count >= 50) // �󲿷ֶ����ߣ�û�е������жϵ�����
    //        return monotonicity_change_line;
    if (start >= MT9V03X_H - 1 - 5) // ����Խ�籣��
        start = MT9V03X_H - 1 - 5;
    if (end <= 5)
        end = 5;
    if (start <= end)
        return monotonicity_change_line;
    for (i = start; i >= end; i--) // ���ȡǰ5��5���ݣ�����ǰ������뷶Χ��Ҫ��
    {
        if (Right_Line[i] == Right_Line[i + 5] && Right_Line[i] == Right_Line[i - 5] &&
            Right_Line[i] == Right_Line[i + 4] && Right_Line[i] == Right_Line[i - 4] &&
            Right_Line[i] == Right_Line[i + 3] && Right_Line[i] == Right_Line[i - 3] &&
            Right_Line[i] == Right_Line[i + 2] && Right_Line[i] == Right_Line[i - 2] &&
            Right_Line[i] == Right_Line[i + 1] && Right_Line[i] == Right_Line[i - 1])
        { // һ������һ������Ȼ������Ϊ����ת�۵�
            continue;
        }
        else if (Right_Line[i] <= Right_Line[i + 5] && Right_Line[i] <= Right_Line[i - 5] &&
                 Right_Line[i] <= Right_Line[i + 4] && Right_Line[i] <= Right_Line[i - 4] &&
                 Right_Line[i] <= Right_Line[i + 3] && Right_Line[i] <= Right_Line[i - 3] &&
                 Right_Line[i] <= Right_Line[i + 2] && Right_Line[i] <= Right_Line[i - 2] &&
                 Right_Line[i] <= Right_Line[i + 1] && Right_Line[i] <= Right_Line[i - 1])
        { // �ͺܱ����������������ǰ5����5�����ģ��Ǿ��ǵ���ͻ���
            monotonicity_change_line = i;
            break;
        }
    }
    return monotonicity_change_line;
}
/**
 * @brief ��imageBinary�������油��
 * @param slope
 * @param startX
 * @param startY
 * @param endX
 */
void drawLineWithSlope(float slope, int startX, int startY, int endX)
{
    if (startX >= MT9V03X_H - 1) // �޷�����
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
 * @brief ������ֱ�����ڱ�����
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
 * @brief ���ĵ�ǵ��������, endX�������
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
