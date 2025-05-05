#include <image.h>
#include <math.h>
// 相关结构体数据存储的变量的定义

uint8_t mt9v03x_image_Binary[MT9V03X_H][MT9V03X_W];
uint8_t mt9v03x_image_HorizonCompress[compress_H][compress_W];

float Error;
int zebraChange = 0;
uint8_t Left_Line[MT9V03X_H]; // eg. Left_Line[5] = 72 第五行左边的边界列数为72
uint8_t Right_Line[MT9V03X_H];
uint8_t White_Column[MT9V03X_W];   // 用于存储每一列的白点数量
uint8_t Left_Line_Lost[MT9V03X_H]; // 左丢线数组，丢线置1，不丢线置0
uint8_t Right_Line_Lost[MT9V03X_H];
uint8_t Left_Line_Lost_Count;
uint8_t Right_Line_Lost_Count;
uint8_t Both_Lost_Times;

uint8_t Left_Boundry_Start;  // 第一个非丢线点,常规边界起始点
uint8_t Right_Boundry_Start; // 第一个非丢线点,常规边界起始点
uint8_t Search_Stop_Line;

uint8_t Straight_Flag;

int Cross_Flag;
int Zebra_Flag;

uint8_t Ramp_Flag;
volatile int Left_Down_Find = 0; // 十字使用，找到被置行数，没找到就是0
volatile int Left_Up_Find = 0;   // 四个拐点标志
volatile int Right_Down_Find = 0;
volatile int Right_Up_Find = 0;

void Image_Pre_Operation_Functions()
{
    /**
      下面的函数都是有关一些图像前置处理比如二值化、压缩啥的
      */
    return;
}

//----------------------------对图像本身的处理(二值化等)------------------------

/*begin  大津法比赛   begin*/
// 快速大津法二值化 pixelSum = width * height/4;
//-------------------------------------------------------------------------------------------------------------------
//   @brief      快速大津
//   @return     uint8
//   @since      v1.1
//   Sample usage:   OTSU_Threshold = otsuThreshold(mt9v03x_image_dvp[0]);//大津法阈值
//-------------------------------------------------------------------------------------------------------------------
uint8_t otsuThreshold_fast(uint8_t *image) // 注意计算阈值的一定要是原图像
{
#define GrayScale 256
    int Pixel_Max = 0;
    int Pixel_Min = 255;
    uint16_t width = MT9V03X_W;              // 宽
    uint16_t height = MT9V03X_H;             // 高
    int pixelCount[GrayScale];               // 各像素GrayScale的个数pixelCount 一维数组
    float pixelPro[GrayScale];               // 各像素GrayScale所占百分比pixelPro 一维数组
    int i, j, pixelSum = width * height / 4; // pixelSum是获取总的图像像素个数的1/4，相应下面轮询时高和宽都是以2为单位自增
    uint8_t threshold = 0;
    //    uint8 last_threshold = 0;
    uint8_t *data = image; // 指向像素数据的指针

    // 清零
    for (i = 0; i < GrayScale; i++)
    {
        pixelCount[i] = 0;
        pixelPro[i] = 0;
    }

    uint32_t gray_sum = 0; // 每次执行到这会将gray_sum清零
    // 统计灰度级中每个像素在整幅图像中的个数
    for (i = 0; i < height; i += 2) // 高
    {
        for (j = 0; j < width; j += 2) // 宽
        {
            pixelCount[(int)data[i * width + j]]++; // 将当前的点的像素值作为计数数组的下标(类似于哈希)
            gray_sum += (int)data[i * width + j];   // 灰度值总和
            if (data[i * width + j] > Pixel_Max)
                Pixel_Max = data[i * width + j];
            if (data[i * width + j] < Pixel_Min)
                Pixel_Min = data[i * width + j];
        }
    }

    // 计算每个像素值的点在整幅图像中的比例
    for (i = Pixel_Min; i < Pixel_Max; i++)
    {
        pixelPro[i] = (float)pixelCount[i] / pixelSum;
    }

    // 遍历灰度级[0,255]
    float w0, w1, u0tmp, u1tmp, u0, u1, u, deltaTmp, deltaMax = 0;

    w0 = w1 = u0tmp = u1tmp = u0 = u1 = u = deltaTmp = 0;
    for (j = Pixel_Min; j < Pixel_Max; j++)
    {

        w0 += pixelPro[j];        // 背景部分每个灰度值的像素点所占比例之和   即背景部分的比例
        u0tmp += j * pixelPro[j]; // 背景部分 每个灰度值的点的比例 *灰度值

        w1 = 1 - w0;
        u1tmp = gray_sum / pixelSum - u0tmp;

        u0 = u0tmp / w0;   // 背景平均灰度
        u1 = u1tmp / w1;   // 前景平均灰度
        u = u0tmp + u1tmp; // 全局平均灰度
        deltaTmp = (float)(w0 * w1 * (u0 - u1) * (u0 - u1));
        if (deltaTmp > deltaMax)
        {
            deltaMax = deltaTmp;
            threshold = (uint8_t)j;
        }
        if (deltaTmp < deltaMax)
        {
            break;
        }
    }

    return threshold;
}
/*end  大津法比赛   end*/

/*begin  大津法学习   begin*/
//------------------摄像头参数--------------//
uint8_t image_threshold = 46; // 图像阈值 0~255
uint8_t dis_image[60][80];

uint8_t otsuThreshold(uint8_t *image, uint16_t width, uint16_t height)
{
#define GrayScale 256
    int pixelCount[GrayScale] = {0}; // 每个灰度值所占像素个数
    float pixelPro[GrayScale] = {0}; // 每个灰度值所占总像素比例
    int i, j;
    int Sumpix = width * height; // 总像素点
    uint8_t threshold = 0;
    uint8_t *data = image; // 指向像素数据的指针

    // 统计灰度级中每个像素在整幅图像中的个数
    for (i = 0; i < height; i++)
    {
        for (j = 0; j < width; j++)
        {
            pixelCount[(int)data[i * width + j]]++; // 将像素值作为计数数组的下标
            //   pixelCount[(int)image[i][j]]++;    若不用指针用这个
        }
    }
    float u = 0;
    for (i = 0; i < GrayScale; i++)
    {
        pixelPro[i] = (float)pixelCount[i] / Sumpix; // 计算每个像素在整幅图像中的比例
        u += i * pixelPro[i];                        // 总平均灰度
    }

    float maxVariance = 0.0;    // 最大类间方差
    float w0 = 0, avgValue = 0; // w0 前景比例 ，avgValue 前景平均灰度
    for (i = 0; i < 256; i++)   // 每一次循环都是一次完整类间方差计算 (两个for叠加为1个)
    {
        w0 += pixelPro[i]; // 假设当前灰度i为阈值, 0~i 灰度像素所占整幅图像的比例即前景比例
        avgValue += i * pixelPro[i];

        float variance = pow((avgValue / w0 - u), 2) * w0 / (1 - w0); // 类间方差
        if (variance > maxVariance)
        {
            maxVariance = variance;
            threshold = (uint8_t)i;
        }
    }
    return threshold;
}

/*end  大津法学习   end*/

/**
 * @brief    将灰度图用固定阈值转化成二值化黑白图
 * @param    value ----对应的二值化阈值
 * @retval   none
 * @note     最后的结果已经保存到Binary_Image里面
 */
void setImageTwoValue(uint8_t value)
{
    uint8_t temp_valude;                    // 暂存灰度值
    for (uint8_t i = 0; i < MT9V03X_H; i++) // 高
    {
        for (uint8_t j = 0; j < MT9V03X_W; j++) // 宽
        {
            temp_valude = mt9v03x_image[i][j];
            if (temp_valude < value)
            {
                mt9v03x_image_Binary[i][j] = PIXEL_BLACK; // 黑
            }
            else
            {
                mt9v03x_image_Binary[i][j] = PIXEL_WHITE; // 白
            }
        }
    }
}

/**
 * 学长版的图像压缩函数
 */
// void horizonCompress(uint8 tempimgbuff[], uint8 img[])
//{
//     int dealMax = CAMERA_H*CAMERA_W/HORIZON_TIMES;
//     for(int i = 0;i < dealMax;i++)
//     {
//         uint8 blackNumCnt=0;
//         for(int index=0;index<HORIZON_TIMES;index++)
//             if( (*(img+i*HORIZON_TIMES+index))<255 )    blackNumCnt++;
//
//         if(blackNumCnt >=2) tempimgbuff[i]=PIXEL_BLACK;         //2个黑点置黑色
//         else tempimgbuff[i]=PIXEL_WHITE;
//     }
// }

/**
 * @brief    将图像压缩成另外一个大小(一般是变小)
 * @param    none
 * @retval   none
 * @note     建议压缩的大小为图像原长宽的倍数大小，浮点不好处理
 * @note     要压缩的话修改宏定义compressH和compressW
 */
void compressImage(void)
{
    int i, j, row, line;
    const float div_h = MT9V03X_H / compress_H, div_w = MT9V03X_W / compress_W;
    for (i = 0; i < compress_H; i++)
    {
        row = i * div_h + 0.5;
        for (j = 0; j < compress_W; j++)
        {
            line = j * div_w + 0.5;
            mt9v03x_image_HorizonCompress[i][j] = mt9v03x_image[row][line]; // 修改这个
        }
    }
}

//-------------------------------图像边界提取的相关代码-----------------------------------

/**
 * @brief    双最长白线列巡线
 * @param    none
 * @retval   none
 * Sample    longestWhiteColumnSearchLines(); 最后得到Line_Info的边界信息
 * @note     startColumn和endColumn是用来约束白线出现的范围的,避免一些极端情况
 **/
void longestWhiteColumnSearchLines()
{
    // Pre:相关变量
    int Start_Column = 20; // 最长白列的搜索区间
    int End_Column = MT9V03X_W - 20;
    // clear之前的数据
    Left_Boundry_Start = 0;
    Right_Boundry_Start = 0;
    Left_Line_Lost_Count = 0;
    Right_Line_Lost_Count = 0;
    Both_Lost_Times = 0;
    memset(Left_Line, 0, MT9V03X_H);
    memset(Right_Line, 0, MT9V03X_H);
    memset(Left_Line_Lost, 0, MT9V03X_H);
    memset(Left_Line_Lost, 0, MT9V03X_H);
    memset(White_Column, 0, MT9V03X_W);

    uint8_t Longest_White_Column_Left[2] = {0};  // [0]表示最长长度,[1]表示在第几列
    uint8_t Longest_White_Column_Right[2] = {0}; // [0]表示最长长度,[1]表示在第几列

    // 一:从左往右，从下往上寻找白点计数，遇到黑点就停止
    for (int j = Start_Column; j <= End_Column; j++)
    {
        for (int i = MT9V03X_H - 1; i >= 0; i--)
        {
            if (mt9v03x_image_Binary[i][j] == PIXEL_BLACK)
                break;
            else
                White_Column[j]++;
        }
    }

    // 环岛需要对最长白列范围进行限定，环岛3状态找不到上角点，可以修改下述参数
    // 环岛3状态需要改变最长白列寻找范围
    if (Right_Island_Flag == 1) // 右环
    {
        if (Island_State == 3)
        {
            Start_Column = 40;
            End_Column = MT9V03X_W - 20;
        }
    }
    else if (Left_Island_Flag == 1) // 左环
    {
        if (Island_State == 3)
        {
            Start_Column = 20;
            End_Column = MT9V03X_W - 40;
        }
    }

    // 二、随后，从左到右寻找白点数量最多的一列；同样的，从右到左也寻找白点数最多的一列
    for (int j = Start_Column; j <= End_Column; j++)
    {
        if (White_Column[j] > Longest_White_Column_Left[0])
        {
            Longest_White_Column_Left[0] = White_Column[j];
            Longest_White_Column_Left[1] = j;
        }
    }
    for (int j = End_Column; j >= Start_Column; j--)
    {
        if (White_Column[j] > Longest_White_Column_Right[0])
        {
            Longest_White_Column_Right[0] = White_Column[j];
            Longest_White_Column_Right[1] = j;
        }
    }
    Search_Stop_Line = Longest_White_Column_Left[0];
    /*三、从左最长白列向左找，找到"黑黑白"，即白点为左边界；
          从右最长白列向右找，找到“白黑黑”，即白点为右边界*/
    for (int i = MT9V03X_H - 1; i > MT9V03X_H - Search_Stop_Line; i--)
    { // 左边找线
        for (int j = Longest_White_Column_Left[1]; j >= 0 + 2; j--)
        {
            if (mt9v03x_image_Binary[i][j] == PIXEL_WHITE &&
                mt9v03x_image_Binary[i][j - 1] == PIXEL_BLACK &&
                mt9v03x_image_Binary[i][j - 2] == PIXEL_BLACK)
            {
                Left_Line[i] = j;
                Left_Line_Lost[i] = 0;
                break;
            }
            else if (j == 2)
            { // 左边丢线
                Left_Line[i] = j;
                Left_Line_Lost[i] = 1;
                Left_Line_Lost_Count++;
                break;
            }
        }
        // 右边找线
        for (int j = Longest_White_Column_Right[1]; j <= MT9V03X_W - 1 - 2; j++)
        {
            if (mt9v03x_image_Binary[i][j] == PIXEL_WHITE &&
                mt9v03x_image_Binary[i][j + 1] == PIXEL_BLACK &&
                mt9v03x_image_Binary[i][j + 2] == PIXEL_BLACK)
            {
                Right_Line[i] = j;
                Right_Line_Lost[i] = 0;
                break;
            }
            else if (j == MT9V03X_W - 1 - 2)
            {
                Right_Line[i] = j;
                Right_Line_Lost[i] = 1;
                Right_Line_Lost_Count++;
                break;
            }
        }
    }
    for (int i = MT9V03X_H - 1; i >= MT9V03X_H - Search_Stop_Line; i--)
    {
        if (Left_Line_Lost[i] != 1 && Left_Boundry_Start == 0)
        {
            Left_Boundry_Start = i;
        }
        if (Right_Line_Lost[i] != 1 && Right_Boundry_Start == 0)
        {
            Right_Boundry_Start = i;
        }
        if (Left_Line_Lost[i] == 1 && Right_Line_Lost[i] == 1)
        {
            Both_Lost_Times++;
        }
    }
    // tft180专用debug语句
    //    for(int i = MT9V03X_H - 1; i >= MT9V03X_H - Search_Stop_Line; i --) {
    //        tft180_draw_point (i, Left_Line[i], RGB565_RED);
    //        tft180_draw_point (i, Right_Line[i], RGB565_BLUE);
    //    }
    // // qt专用debug语句
    // qDebug() << "Left_Boundry_Start:" << Left_Boundry_Start << endl;
    // qDebug() << "Right_Boundry_Start:" << Right_Boundry_Start << endl;
}

void Image_Element_Detecting_Functions()
{
    /**
      智能车赛道上的相关元素有直道、弯道、十字
      第一、直道:
            特征：Search_Stop_Line计数很大(多大自己定)
                 边界起始点很靠下
                 赛道没有丢线
                 todo:赛道中线方差小、视野远近的斜率
      第二、十字：
                 存在双边丢线，且双边丢线基本位于视野中间。
                 存在起码2个角点或者更多。
                 最长白列较长。
     */
    return;
}

// void elementDetecting()
// {
//     Err_Sum();
//     straightDetect();
// }

// void Err_Sum()
// {
//     Error = 0.0;
//     int size = 0;
//     for (int i = MT9V03X_H - 1; i >= Search_Stop_Line; i--)
//     {
//         Error += (MT9V03X_W / 2) - (Left_Line[i] + Right_Line[i]) / 2;
//         size++;
//     }
//     Error /= size;
// }

// void straightDetect()
// {

// #define Search_Line_Standard 40
//     Straight_Flag = 0;
//     if (Search_Stop_Line > Search_Line_Standard) // 截止行很远
//     {
//         if (-10 <= Error && Error <= 10) // 误差很小
//         {
//             Straight_Flag = 1; // 认为是直道
//         }
//     }
// }

/**
 * @brief findLeftDownPoint
 * @note 找四个角点，返回值是角点所在的行数
 * @return
 */
int findLeftDownPoint(int start, int end) // 找四个角点，返回值是角点所在的行数
{
    int i, t;
    int left_down_line = 0;
    // if (Left_Line_Lost_Count >= 0.9 * MT9V03X_H) // 大部分都丢线，没有拐点判断的意义
    //     return left_down_line;
    if (start < end)
    {
        t = start;
        start = end;
        end = t;
    }
    if (start >= MT9V03X_H - 1 - 5) // 下面5行数据不稳定，不能作为边界点来判断，舍弃
        start = MT9V03X_H - 1 - 5;
    if (end <= MT9V03X_H - Search_Stop_Line)
        end = MT9V03X_H - Search_Stop_Line;
    if (end <= 5)
        end = 5;
    for (i = start; i >= end; i--)
    {
        if (left_down_line == 0 &&                       // 只找第一个符合条件的点
            abs(Left_Line[i] - Left_Line[i + 1]) <= 5 && // 角点的阈值可以更改
            abs(Left_Line[i + 1] - Left_Line[i + 2]) <= 5 &&
            abs(Left_Line[i + 2] - Left_Line[i + 3]) <= 5 &&
            (Left_Line[i] - Left_Line[i - 2]) >= 5 &&
            (Left_Line[i] - Left_Line[i - 3]) >= 10 &&
            (Left_Line[i] - Left_Line[i - 4]) >= 10)
        {
            left_down_line = i; // 获取行数即可
            break;
        }
    }
    return left_down_line;
}

/**
 * @brief findLeftUpPoint
 * @note 找四个角点，返回值是角点所在的行数
 * @return
 */
int findLeftUpPoint(int start, int end) // 找四个角点，返回值是角点所在的行数
{
    int i, t;
    int left_up_line = 0;
    //    if (Left_Line_Lost_Count >= 0.9 * MT9V03X_H) // 大部分都丢线，没有拐点判断的意义
    //        return left_up_line;
    if (start < end)
    {
        t = start;
        start = end;
        end = t;
    }
    if (end <= MT9V03X_H - Search_Stop_Line) // 搜索截止行往上的全都不判
        end = MT9V03X_H - Search_Stop_Line;
    if (end <= 5) // 及时最长白列非常长，也要舍弃部分点，防止数组越界
        end = 5;
    if (start >= MT9V03X_H - 1 - 5)
        start = MT9V03X_H - 1 - 5;
    for (i = start; i >= end; i--)
    {
        if (left_up_line == 0 && // 只找第一个符合条件的点
            abs(Left_Line[i] - Left_Line[i - 1]) <= 5 &&
            abs(Left_Line[i - 1] - Left_Line[i - 2]) <= 5 &&
            abs(Left_Line[i - 2] - Left_Line[i - 3]) <= 5 &&
            (Left_Line[i] - Left_Line[i + 2]) >= 8 &&
            (Left_Line[i] - Left_Line[i + 3]) >= 15 &&
            (Left_Line[i] - Left_Line[i + 4]) >= 15)
        {
            left_up_line = i; // 获取行数即可
            break;
        }
    }
    return left_up_line; // 如果是MT9V03X_H-1，说明没有这么个拐点
}
/**
 * @brief findRightDownPoint
 * @note 找四个角点，返回值是角点所在的行数
 * @return
 */
int findRightDownPoint(int start, int end) // 找四个角点，返回值是角点所在的行数
{
    int i, t;
    int right_down_line = 0;
    // if (Right_Line_Lost_Count >= 0.9 * MT9V03X_H) // 大部分都丢线，没有拐点判断的意义
    //     return right_down_line;
    if (start < end)
    {
        t = start;
        start = end;
        end = t;
    }
    if (start >= MT9V03X_H - 1 - 5) // 下面5行数据不稳定，不能作为边界点来判断，舍弃
        start = MT9V03X_H - 1 - 5;
    if (end <= MT9V03X_H - Search_Stop_Line)
        end = MT9V03X_H - Search_Stop_Line;
    if (end <= 5)
        end = 5;
    for (i = start; i >= end; i--)
    {
        if (right_down_line == 0 &&                        // 只找第一个符合条件的点
            abs(Right_Line[i] - Right_Line[i + 1]) <= 5 && // 角点的阈值可以更改
            abs(Right_Line[i + 1] - Right_Line[i + 2]) <= 5 &&
            abs(Right_Line[i + 2] - Right_Line[i + 3]) <= 5 &&
            (Right_Line[i] - Right_Line[i - 2]) <= -5 &&
            (Right_Line[i] - Right_Line[i - 3]) <= -10 &&
            (Right_Line[i] - Right_Line[i - 4]) <= -10)
        {
            right_down_line = i; // 获取行数即可
            break;
        }
    }
    return right_down_line;
}

/**
 * @brief findRightUpPoint
 * @param start
 * @param end
 * @return
 */
int findRightUpPoint(int start, int end) // 找四个角点，返回值是角点所在的行数
{
    int i, t;
    int right_up_line = 0;

    if (start < end)
    {
        t = start;
        start = end;
        end = t;
    }
    if (end <= MT9V03X_H - Search_Stop_Line) // 搜索截止行往上的全都不判
        end = MT9V03X_H - Search_Stop_Line;
    if (end <= 5) // 及时最长白列非常长，也要舍弃部分点，防止数组越界
        end = 5;
    if (start >= MT9V03X_H - 1 - 5)
        start = MT9V03X_H - 1 - 5;
    for (i = start; i >= end; i--)
    {
        if (right_up_line == 0 &&                          // 只找第一个符合条件的点
            abs(Right_Line[i] - Right_Line[i - 1]) <= 5 && // 下面两行位置差不多
            abs(Right_Line[i - 1] - Right_Line[i - 2]) <= 5 &&
            abs(Right_Line[i - 2] - Right_Line[i - 3]) <= 5 &&
            (Right_Line[i] - Right_Line[i + 2]) <= -8 &&
            (Right_Line[i] - Right_Line[i + 3]) <= -15 &&
            (Right_Line[i] - Right_Line[i + 4]) <= -15)
        {
            right_up_line = i; // 获取行数即可
            break;
        }
    }
    return right_up_line;
}
/**
 * @brief findDownPoint
 * @param start
 * @param end
 */
void findDownPoint(int start, int end)
{
    int i, t;
    Right_Down_Find = 0;
    Left_Down_Find = 0;
    if (start < end)
    {
        t = start;
        start = end;
        end = t;
    }
    if (start >= MT9V03X_H - 1 - 5) // 下面5行数据不稳定，不能作为边界点来判断，舍弃
        start = MT9V03X_H - 1 - 5;
    if (end <= MT9V03X_H - Search_Stop_Line)
        end = MT9V03X_H - Search_Stop_Line;
    if (end <= 5)
        end = 5;
    for (i = start; i >= end; i--)
    {
        if (Left_Down_Find == 0 &&                       // 只找第一个符合条件的点
            abs(Left_Line[i] - Left_Line[i + 1]) <= 5 && // 角点的阈值可以更改
            abs(Left_Line[i + 1] - Left_Line[i + 2]) <= 5 &&
            abs(Left_Line[i + 2] - Left_Line[i + 3]) <= 5 &&
            (Left_Line[i] - Left_Line[i - 2]) >= 8 &&
            (Left_Line[i] - Left_Line[i - 3]) >= 10 &&
            (Left_Line[i] - Left_Line[i - 4]) >= 10)
        {
            Left_Down_Find = i; // 获取行数即可
        }
        if (Right_Down_Find == 0 &&                        // 只找第一个符合条件的点
            abs(Right_Line[i] - Right_Line[i + 1]) <= 5 && // 角点的阈值可以更改
            abs(Right_Line[i + 1] - Right_Line[i + 2]) <= 5 &&
            abs(Right_Line[i + 2] - Right_Line[i + 3]) <= 5 &&
            (Right_Line[i] - Right_Line[i - 2]) <= -8 &&
            (Right_Line[i] - Right_Line[i - 3]) <= -15 &&
            (Right_Line[i] - Right_Line[i - 4]) <= -15)
        {
            Right_Down_Find = i;
        }
        if (Left_Down_Find != 0 && Right_Down_Find != 0) // 两个找到就退出
        {
            break;
        }
    }
}
/**
 * @brief Find_Up_Point
 * @param start
 * @param end
 * @return 在函数里面修改RightUpFind和LeftUpFind变量的值
 * @note  start肯定要比end大，注意一下
 */
void findUpPoint(int start, int end)
{
    int i, t;
    Left_Up_Find = 0;
    Right_Up_Find = 0;
    if (start < end)
    {
        t = start;
        start = end;
        end = t;
    }
    if (end <= MT9V03X_H - Search_Stop_Line)
        end = MT9V03X_H - Search_Stop_Line;
    if (end <= 5) // 及时最长白列非常长，也要舍弃部分点，防止数组越界
        end = 5;
    if (start >= MT9V03X_H - 1 - 5) // 下面5行数据不稳定，不能作为边界点来判断，舍弃
        start = MT9V03X_H - 1 - 5;
    for (i = start; i >= end; i--)
    {
        if (Left_Up_Find == 0 && // 只找第一个符合条件的点
            abs(Left_Line[i] - Left_Line[i - 1]) <= 5 &&
            abs(Left_Line[i - 1] - Left_Line[i - 2]) <= 5 &&
            abs(Left_Line[i - 2] - Left_Line[i - 3]) <= 5 &&
            (Left_Line[i] - Left_Line[i + 2]) >= 8 &&
            (Left_Line[i] - Left_Line[i + 3]) >= 15 &&
            (Left_Line[i] - Left_Line[i + 4]) >= 15)
        {
            Left_Up_Find = i; // 获取行数即可
        }
        if (Right_Up_Find == 0 &&                          // 只找第一个符合条件的点
            abs(Right_Line[i] - Right_Line[i - 1]) <= 5 && // 下面两行位置差不多
            abs(Right_Line[i - 1] - Right_Line[i - 2]) <= 5 &&
            abs(Right_Line[i - 2] - Right_Line[i - 3]) <= 5 &&
            (Right_Line[i] - Right_Line[i + 2]) <= -8 &&
            (Right_Line[i] - Right_Line[i + 3]) <= -15 &&
            (Right_Line[i] - Right_Line[i + 4]) <= -15)
        {
            Right_Up_Find = i; // 获取行数即可
        }
        if (Left_Up_Find != 0 && Right_Up_Find != 0) // 下面两个找到就出去
        {
            break;
        }
    }
    if (abs(Right_Up_Find - Left_Up_Find) >= 30) // 纵向撕裂过大，视为误判
    {
        Right_Up_Find = 0;
        Left_Up_Find = 0;
    }
}
/**
 * @brief Cross_Detect
 */
void crossDetect()
{
    int down_search_start = 0; // 下点搜索开始行
    Cross_Flag = 0;
    if (Island_State == 0 && Ramp_Flag == 0) // 与环岛互斥开
    {
        Left_Up_Find = 0;
        Right_Up_Find = 0;
        if (Both_Lost_Times >= 10) // 十字必定有双边丢线，在有双边丢线的情况下再开始找角点
        {
            findUpPoint(MT9V03X_H - 1, 0);
            if (Left_Up_Find == 0 && Right_Up_Find == 0) // 只要没有同时找到两个上点，直接结束
            {
                return;
            }
        }
        if (Left_Up_Find != 0 && Right_Up_Find != 0) // 找到两个上点，就找到十字了
        {
            Cross_Flag = 1;                                                                  // 对应标志位，便于各元素互斥掉
            down_search_start = Left_Up_Find > Right_Up_Find ? Left_Up_Find : Right_Up_Find; // 用两个上拐点坐标靠下者作为下点的搜索上限
            findDownPoint(MT9V03X_H - 5, down_search_start + 2);                             // 在上拐点下2行作为下点的截止行
            if (Left_Down_Find <= Left_Up_Find)
            {
                Left_Down_Find = 0; // 下点不可能比上点还靠上
            }
            if (Right_Down_Find <= Right_Up_Find)
            {
                Right_Down_Find = 0; // 下点不可能比上点还靠上
            }
            if (Left_Down_Find != 0 && Right_Down_Find != 0)
            { // 四个点都在，无脑连线，这种情况显然很少
                // Left_Add_Line(Left_Line[Left_Up_Find], Left_Up_Find, Left_Line[Left_Down_Find], Left_Down_Find);
                addLeftLine(Left_Down_Find, Left_Line[Left_Down_Find], Left_Up_Find, Left_Line[Left_Up_Find]);
                // Right_Add_Line(Right_Line[Right_Up_Find], Right_Up_Find, Right_Line[Right_Down_Find], Right_Down_Find);
                addRightLine(Right_Down_Find, Right_Line[Right_Down_Find], Right_Up_Find, Right_Line[Right_Up_Find]);
            }
            else if (Left_Down_Find == 0 && Right_Down_Find != 0) // 左点没找到，右点找到了
            {                                                     // 三个点
                lengthenLeftBoundry(Left_Up_Find - 1, MT9V03X_H - 1);
                addRightLine(Right_Down_Find, Right_Line[Right_Down_Find], Right_Up_Find, Right_Line[Right_Up_Find]);
            }
            else if (Left_Down_Find != 0 && Right_Down_Find == 0) // 左点找到了，右点没找到
            {                                                     // 三个点
                addLeftLine(Left_Down_Find, Left_Line[Left_Down_Find], Left_Up_Find, Left_Line[Left_Up_Find]);
                lengthenRightBoundry(Right_Up_Find - 1, MT9V03X_H - 1);
            }
            else if (Left_Down_Find == 0 && Right_Down_Find == 0) // 下面两个点都没找到
            {
                lengthenLeftBoundry(Left_Up_Find - 1, MT9V03X_H - 1);
                lengthenRightBoundry(Right_Up_Find - 1, MT9V03X_H - 1);
            }
            else
            {
                Cross_Flag = 0;
            }
        }
    }
}

/**
 * @brief addLeftLine
 * @param    (x1, y1)对应的是down点,(x2, y2)对应的是up点
 */
void addLeftLine(int x1, int y1, int x2, int y2)
{
    /* 正常来说x1 > x2 , y1 < y2 */
    int hx;
    if (x1 >= MT9V03X_H - 1) // 起始点位置校正，排除数组越界的可能
        x1 = MT9V03X_H - 1;
    else if (x1 <= 0)
        x1 = 0;
    if (y1 >= MT9V03X_W - 1)
        y1 = MT9V03X_W - 1;
    else if (y1 <= 0)
        y1 = 0;
    if (x2 >= MT9V03X_H - 1)
        x2 = MT9V03X_H - 1;
    else if (x2 <= 0)
        x2 = 0;
    if (y2 >= MT9V03X_W - 1)
        y2 = MT9V03X_W - 1;
    else if (y2 <= 0)
        y2 = 0;
    float slope = (float)(y2 - y1) / (x2 - x1); // 计算斜率
    for (int x = x1; x >= x2; --x)
    {
        hx = slope * (x - x2) + y2;
        Left_Line[x] = hx; // 将计算出的点存入Left_Line
    }
}
/**
 * @brief addRightLine
 * @param x3
 * @param y3
 * @param x4
 * @param y4
 * @note       (x3, y3)对应down点, (x4, y4)对应up点
 */
void addRightLine(int x3, int y3, int x4, int y4)
{
    /* 正常来说x3 > x4 , y3 > y4 */
    float slope = (float)(y3 - y4) / (x3 - x4);
    for (int x = x3; x >= x4; x--)
    {
        int y = slope * (x - x3) + y3;
        Right_Line[x] = y;
    }
}
/**
 * @brief lengthenLeftBoundry
 * @param start
 * @param end
 * @note    start对应找到的上角点的x坐标，end对应要补到的x坐标，所以理论上end应该比start大
 */
void lengthenLeftBoundry(int start, int end)
{
    if (start >= MT9V03X_H - 1) // 起始点位置校正，排除数组越界的可能
        start = MT9V03X_H - 1;
    else if (start <= 0)
        start = 0;
    if (end >= MT9V03X_H - 1)
        end = MT9V03X_H - 1;
    else if (end <= 0)
        end = 0;
    if (end < start) //++访问，坐标互换
    {
        int t = end;
        end = start;
        start = t;
    }
    if (start <= 5) // 因为需要在开始点向上找3个点，对于起始点过于靠上，不能做延长，只能直接连线
    {
        addLeftLine(start, Left_Line[start], end, Left_Line[end]);
    }

    float k = (float)(Left_Line[start - 4] - Left_Line[start - 8]) / (4); // y1-y2/x1-x2
    for (int x = start; x <= end; x++)
    {
        Left_Line[x] = k * (x - start) + Left_Line[start];
        if (Left_Line[x] >= MT9V03X_W - 1)
            Left_Line[x] = MT9V03X_W - 1;
        else if (Left_Line[x] <= 0)
            Left_Line[x] = 0;
    }
}
/**
 * @brief lengthenRightBoundry
 * @param start
 * @param end
 * @note    start对应找到的上角点的x坐标，end对应要补到的x坐标，所以理论上end应该比start大
 */
void lengthenRightBoundry(int start, int end)
{
    if (start >= MT9V03X_H - 1) // 起始点位置校正，排除数组越界的可能
        start = MT9V03X_H - 1;
    else if (start <= 0)
        start = 0;
    if (end >= MT9V03X_H - 1)
        end = MT9V03X_H - 1;
    else if (end <= 0)
        end = 0;
    if (end < start) //++访问，坐标互换
    {
        int t = end;
        end = start;
        start = t;
    }
    if (start <= 5) // 因为需要在开始点向上找3个点，对于起始点过于靠上，不能做延长，只能直接连线
    {
        addRightLine(start, Right_Line[start], end, Right_Line[end]);
    }

    float k = (float)(Right_Line[start] - Right_Line[start - 4]) / (4); // y1-y2/x1-x2
    for (int x = start; x <= end; x++)
    {
        Right_Line[x] = k * (x - start) + Right_Line[start];
        if (Right_Line[x] >= MT9V03X_W - 1)
            Right_Line[x] = MT9V03X_W - 1;
        else if (Right_Line[x] <= 0)
            Right_Line[x] = 0;
    }
}

/**
 * @brief 在image_Binary上面划线
 * @param startX
 * @param startY
 * @param endX
 * @param endY
 * @note  这次操作的对象不是在LeftLine或RightLine数组上面了，是直接在image_Binary上面标为PIXEL_BLACK
 */
void drawLine(int startX, int startY, int endX, int endY)
{
    int i, x, y;
    int start = 0, end = 0;

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
    if (endY >= MT9V03X_W - 1)
        endY = MT9V03X_W - 1;
    else if (endY <= 0)
        endY = 0;

    if (startX == endX) // 一条横线
    {
        if (startY >= endY)
        {
            start = endY;
            end = startY;
        }
        else
        {
            start = startY;
            end = endY;
        }
        for (i = start; i <= end; i++)
        {
            mt9v03x_image_Binary[startX][i] = PIXEL_BLACK;
            mt9v03x_image_Binary[startX - 1][i] = PIXEL_BLACK;
        }
    }
    else if (startY == endY) // 一条竖线
    {
        if (startX <= endX)
        {
            start = endX;
            end = startX;
        }
        else
        {
            start = startX;
            end = endX;
        }
        for (i = start; i >= end; i--)
        {
            mt9v03x_image_Binary[i][startY] = PIXEL_BLACK;
            mt9v03x_image_Binary[i - 1][startY] = PIXEL_BLACK;
        }
    }
    else // 最基本的情况
    {
        float slope = (float)(endY - startY) / (endX - startX);
        /* y = slope * (x - x1) + y1
         * x = (y - y1) / slope + x1 */
        if (startY > endY)
        {
            start = endY;
            end = startY;
        }
        else
        {
            start = startY;
            end = endY;
        }
        for (i = start; i <= end; i++)
        {
            x = (i - startY) / slope + startX;
            mt9v03x_image_Binary[x][i] = PIXEL_BLACK;
        }

        if (startX > endX)
        {
            start = endX;
            end = startX;
        }
        else
        {
            start = startX;
            end = endX;
        }
        for (i = start; i <= end; i++)
        {
            y = slope * (i - startX) + startY;
            mt9v03x_image_Binary[i][y] = PIXEL_BLACK;
            mt9v03x_image_Binary[i][y - 1] = PIXEL_BLACK;
        }
    }
}
/**
 * @brief 检测斑马线
 * @note  如果是斑马线，设置Zebra_Flag为1
 */
void zebraStripeDetect()
{
    int j = 0;
    int count = 0;
    // 前面的元素互斥补全
    if (Cross_Flag || Island_State)
    {
        return;
    }
    if (Search_Stop_Line >= 55) // 视野很长
    {
        for (j = 10; j <= MT9V03X_W - 10; j++)
        {
            if (abs(White_Column[j] - White_Column[j + 1]) >= 30)
            {
                count++;
            }
        }
    }
    if (count >= 10 && !Zebra_Flag) // 阈值手动调整
    {
        Zebra_Flag = 1;
    }
    if (Zebra_Flag == 1)
    {
        int startCol = 20 + 6;
        int endCol = 20 - 6;
        // int change = 0;
        for (int i = startCol; i >= endCol; i--)
        {
            for (int j = 160; j >= 20; j--)
            {
                if (mt9v03x_image_Binary[i][j] - mt9v03x_image_Binary[i][j - 1])
                    zebraChange++;
            }
        }
        if (zebraChange >= 60)
        {
            Zebra_Flag = 2;
        }
    }
    // 如果找到zebra那么就找左上角角点(对于我们现在来说)
    if (Zebra_Flag == 2)
    {
        //        int garbageSpot = findLeftUpPoint(30, 5);
        //        float k = (float)(Right_Line[garbageSpot] - MT9V03X_W - 20) / (garbageSpot - MT9V03X_H - 1);
        //        drawLineWithSlope(k, MT9V03X_H - 1, MT9V03X_W - 20, garbageSpot);
        //        longestWhiteColumnSearchLines();
    }
}
