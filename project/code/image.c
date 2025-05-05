#include <image.h>
#include <math.h>
// ��ؽṹ�����ݴ洢�ı����Ķ���

uint8_t mt9v03x_image_Binary[MT9V03X_H][MT9V03X_W];
uint8_t mt9v03x_image_HorizonCompress[compress_H][compress_W];

float Error;
int zebraChange = 0;
uint8_t Left_Line[MT9V03X_H]; // eg. Left_Line[5] = 72 ��������ߵı߽�����Ϊ72
uint8_t Right_Line[MT9V03X_H];
uint8_t White_Column[MT9V03X_W];   // ���ڴ洢ÿһ�еİ׵�����
uint8_t Left_Line_Lost[MT9V03X_H]; // �������飬������1����������0
uint8_t Right_Line_Lost[MT9V03X_H];
uint8_t Left_Line_Lost_Count;
uint8_t Right_Line_Lost_Count;
uint8_t Both_Lost_Times;

uint8_t Left_Boundry_Start;  // ��һ���Ƕ��ߵ�,����߽���ʼ��
uint8_t Right_Boundry_Start; // ��һ���Ƕ��ߵ�,����߽���ʼ��
uint8_t Search_Stop_Line;

uint8_t Straight_Flag;

int Cross_Flag;
int Zebra_Flag;

uint8_t Ramp_Flag;
volatile int Left_Down_Find = 0; // ʮ��ʹ�ã��ҵ�����������û�ҵ�����0
volatile int Left_Up_Find = 0;   // �ĸ��յ��־
volatile int Right_Down_Find = 0;
volatile int Right_Up_Find = 0;

void Image_Pre_Operation_Functions()
{
    /**
      ����ĺ��������й�һЩͼ��ǰ�ô�������ֵ����ѹ��ɶ��
      */
    return;
}

//----------------------------��ͼ����Ĵ���(��ֵ����)------------------------

/*begin  ��򷨱���   begin*/
// ���ٴ�򷨶�ֵ�� pixelSum = width * height/4;
//-------------------------------------------------------------------------------------------------------------------
//   @brief      ���ٴ��
//   @return     uint8
//   @since      v1.1
//   Sample usage:   OTSU_Threshold = otsuThreshold(mt9v03x_image_dvp[0]);//�����ֵ
//-------------------------------------------------------------------------------------------------------------------
uint8_t otsuThreshold_fast(uint8_t *image) // ע�������ֵ��һ��Ҫ��ԭͼ��
{
#define GrayScale 256
    int Pixel_Max = 0;
    int Pixel_Min = 255;
    uint16_t width = MT9V03X_W;              // ��
    uint16_t height = MT9V03X_H;             // ��
    int pixelCount[GrayScale];               // ������GrayScale�ĸ���pixelCount һά����
    float pixelPro[GrayScale];               // ������GrayScale��ռ�ٷֱ�pixelPro һά����
    int i, j, pixelSum = width * height / 4; // pixelSum�ǻ�ȡ�ܵ�ͼ�����ظ�����1/4����Ӧ������ѯʱ�ߺͿ�����2Ϊ��λ����
    uint8_t threshold = 0;
    //    uint8 last_threshold = 0;
    uint8_t *data = image; // ָ���������ݵ�ָ��

    // ����
    for (i = 0; i < GrayScale; i++)
    {
        pixelCount[i] = 0;
        pixelPro[i] = 0;
    }

    uint32_t gray_sum = 0; // ÿ��ִ�е���Ὣgray_sum����
    // ͳ�ƻҶȼ���ÿ������������ͼ���еĸ���
    for (i = 0; i < height; i += 2) // ��
    {
        for (j = 0; j < width; j += 2) // ��
        {
            pixelCount[(int)data[i * width + j]]++; // ����ǰ�ĵ������ֵ��Ϊ����������±�(�����ڹ�ϣ)
            gray_sum += (int)data[i * width + j];   // �Ҷ�ֵ�ܺ�
            if (data[i * width + j] > Pixel_Max)
                Pixel_Max = data[i * width + j];
            if (data[i * width + j] < Pixel_Min)
                Pixel_Min = data[i * width + j];
        }
    }

    // ����ÿ������ֵ�ĵ�������ͼ���еı���
    for (i = Pixel_Min; i < Pixel_Max; i++)
    {
        pixelPro[i] = (float)pixelCount[i] / pixelSum;
    }

    // �����Ҷȼ�[0,255]
    float w0, w1, u0tmp, u1tmp, u0, u1, u, deltaTmp, deltaMax = 0;

    w0 = w1 = u0tmp = u1tmp = u0 = u1 = u = deltaTmp = 0;
    for (j = Pixel_Min; j < Pixel_Max; j++)
    {

        w0 += pixelPro[j];        // ��������ÿ���Ҷ�ֵ�����ص���ռ����֮��   ���������ֵı���
        u0tmp += j * pixelPro[j]; // �������� ÿ���Ҷ�ֵ�ĵ�ı��� *�Ҷ�ֵ

        w1 = 1 - w0;
        u1tmp = gray_sum / pixelSum - u0tmp;

        u0 = u0tmp / w0;   // ����ƽ���Ҷ�
        u1 = u1tmp / w1;   // ǰ��ƽ���Ҷ�
        u = u0tmp + u1tmp; // ȫ��ƽ���Ҷ�
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
/*end  ��򷨱���   end*/

/*begin  ���ѧϰ   begin*/
//------------------����ͷ����--------------//
uint8_t image_threshold = 46; // ͼ����ֵ 0~255
uint8_t dis_image[60][80];

uint8_t otsuThreshold(uint8_t *image, uint16_t width, uint16_t height)
{
#define GrayScale 256
    int pixelCount[GrayScale] = {0}; // ÿ���Ҷ�ֵ��ռ���ظ���
    float pixelPro[GrayScale] = {0}; // ÿ���Ҷ�ֵ��ռ�����ر���
    int i, j;
    int Sumpix = width * height; // �����ص�
    uint8_t threshold = 0;
    uint8_t *data = image; // ָ���������ݵ�ָ��

    // ͳ�ƻҶȼ���ÿ������������ͼ���еĸ���
    for (i = 0; i < height; i++)
    {
        for (j = 0; j < width; j++)
        {
            pixelCount[(int)data[i * width + j]]++; // ������ֵ��Ϊ����������±�
            //   pixelCount[(int)image[i][j]]++;    ������ָ�������
        }
    }
    float u = 0;
    for (i = 0; i < GrayScale; i++)
    {
        pixelPro[i] = (float)pixelCount[i] / Sumpix; // ����ÿ������������ͼ���еı���
        u += i * pixelPro[i];                        // ��ƽ���Ҷ�
    }

    float maxVariance = 0.0;    // �����䷽��
    float w0 = 0, avgValue = 0; // w0 ǰ������ ��avgValue ǰ��ƽ���Ҷ�
    for (i = 0; i < 256; i++)   // ÿһ��ѭ������һ��������䷽����� (����for����Ϊ1��)
    {
        w0 += pixelPro[i]; // ���赱ǰ�Ҷ�iΪ��ֵ, 0~i �Ҷ�������ռ����ͼ��ı�����ǰ������
        avgValue += i * pixelPro[i];

        float variance = pow((avgValue / w0 - u), 2) * w0 / (1 - w0); // ��䷽��
        if (variance > maxVariance)
        {
            maxVariance = variance;
            threshold = (uint8_t)i;
        }
    }
    return threshold;
}

/*end  ���ѧϰ   end*/

/**
 * @brief    ���Ҷ�ͼ�ù̶���ֵת���ɶ�ֵ���ڰ�ͼ
 * @param    value ----��Ӧ�Ķ�ֵ����ֵ
 * @retval   none
 * @note     ���Ľ���Ѿ����浽Binary_Image����
 */
void setImageTwoValue(uint8_t value)
{
    uint8_t temp_valude;                    // �ݴ�Ҷ�ֵ
    for (uint8_t i = 0; i < MT9V03X_H; i++) // ��
    {
        for (uint8_t j = 0; j < MT9V03X_W; j++) // ��
        {
            temp_valude = mt9v03x_image[i][j];
            if (temp_valude < value)
            {
                mt9v03x_image_Binary[i][j] = PIXEL_BLACK; // ��
            }
            else
            {
                mt9v03x_image_Binary[i][j] = PIXEL_WHITE; // ��
            }
        }
    }
}

/**
 * ѧ�����ͼ��ѹ������
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
//         if(blackNumCnt >=2) tempimgbuff[i]=PIXEL_BLACK;         //2���ڵ��ú�ɫ
//         else tempimgbuff[i]=PIXEL_WHITE;
//     }
// }

/**
 * @brief    ��ͼ��ѹ��������һ����С(һ���Ǳ�С)
 * @param    none
 * @retval   none
 * @note     ����ѹ���Ĵ�СΪͼ��ԭ����ı�����С�����㲻�ô���
 * @note     Ҫѹ���Ļ��޸ĺ궨��compressH��compressW
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
            mt9v03x_image_HorizonCompress[i][j] = mt9v03x_image[row][line]; // �޸����
        }
    }
}

//-------------------------------ͼ��߽���ȡ����ش���-----------------------------------

/**
 * @brief    ˫�������Ѳ��
 * @param    none
 * @retval   none
 * Sample    longestWhiteColumnSearchLines(); ���õ�Line_Info�ı߽���Ϣ
 * @note     startColumn��endColumn������Լ�����߳��ֵķ�Χ��,����һЩ�������
 **/
void longestWhiteColumnSearchLines()
{
    // Pre:��ر���
    int Start_Column = 20; // ����е���������
    int End_Column = MT9V03X_W - 20;
    // clear֮ǰ������
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

    uint8_t Longest_White_Column_Left[2] = {0};  // [0]��ʾ�����,[1]��ʾ�ڵڼ���
    uint8_t Longest_White_Column_Right[2] = {0}; // [0]��ʾ�����,[1]��ʾ�ڵڼ���

    // һ:�������ң���������Ѱ�Ұ׵�����������ڵ��ֹͣ
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

    // ������Ҫ������з�Χ�����޶�������3״̬�Ҳ����Ͻǵ㣬�����޸���������
    // ����3״̬��Ҫ�ı������Ѱ�ҷ�Χ
    if (Right_Island_Flag == 1) // �һ�
    {
        if (Island_State == 3)
        {
            Start_Column = 40;
            End_Column = MT9V03X_W - 20;
        }
    }
    else if (Left_Island_Flag == 1) // ��
    {
        if (Island_State == 3)
        {
            Start_Column = 20;
            End_Column = MT9V03X_W - 40;
        }
    }

    // ������󣬴�����Ѱ�Ұ׵���������һ�У�ͬ���ģ����ҵ���ҲѰ�Ұ׵�������һ��
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
    /*������������������ң��ҵ�"�ںڰ�"�����׵�Ϊ��߽磻
          ��������������ң��ҵ����׺ںڡ������׵�Ϊ�ұ߽�*/
    for (int i = MT9V03X_H - 1; i > MT9V03X_H - Search_Stop_Line; i--)
    { // �������
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
            { // ��߶���
                Left_Line[i] = j;
                Left_Line_Lost[i] = 1;
                Left_Line_Lost_Count++;
                break;
            }
        }
        // �ұ�����
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
    // tft180ר��debug���
    //    for(int i = MT9V03X_H - 1; i >= MT9V03X_H - Search_Stop_Line; i --) {
    //        tft180_draw_point (i, Left_Line[i], RGB565_RED);
    //        tft180_draw_point (i, Right_Line[i], RGB565_BLUE);
    //    }
    // // qtר��debug���
    // qDebug() << "Left_Boundry_Start:" << Left_Boundry_Start << endl;
    // qDebug() << "Right_Boundry_Start:" << Right_Boundry_Start << endl;
}

void Image_Element_Detecting_Functions()
{
    /**
      ���ܳ������ϵ����Ԫ����ֱ���������ʮ��
      ��һ��ֱ��:
            ������Search_Stop_Line�����ܴ�(����Լ���)
                 �߽���ʼ��ܿ���
                 ����û�ж���
                 todo:�������߷���С����ҰԶ����б��
      �ڶ���ʮ�֣�
                 ����˫�߶��ߣ���˫�߶��߻���λ����Ұ�м䡣
                 ��������2���ǵ���߸��ࡣ
                 ����нϳ���
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
//     if (Search_Stop_Line > Search_Line_Standard) // ��ֹ�к�Զ
//     {
//         if (-10 <= Error && Error <= 10) // ����С
//         {
//             Straight_Flag = 1; // ��Ϊ��ֱ��
//         }
//     }
// }

/**
 * @brief findLeftDownPoint
 * @note ���ĸ��ǵ㣬����ֵ�ǽǵ����ڵ�����
 * @return
 */
int findLeftDownPoint(int start, int end) // ���ĸ��ǵ㣬����ֵ�ǽǵ����ڵ�����
{
    int i, t;
    int left_down_line = 0;
    // if (Left_Line_Lost_Count >= 0.9 * MT9V03X_H) // �󲿷ֶ����ߣ�û�йյ��жϵ�����
    //     return left_down_line;
    if (start < end)
    {
        t = start;
        start = end;
        end = t;
    }
    if (start >= MT9V03X_H - 1 - 5) // ����5�����ݲ��ȶ���������Ϊ�߽�����жϣ�����
        start = MT9V03X_H - 1 - 5;
    if (end <= MT9V03X_H - Search_Stop_Line)
        end = MT9V03X_H - Search_Stop_Line;
    if (end <= 5)
        end = 5;
    for (i = start; i >= end; i--)
    {
        if (left_down_line == 0 &&                       // ֻ�ҵ�һ�����������ĵ�
            abs(Left_Line[i] - Left_Line[i + 1]) <= 5 && // �ǵ����ֵ���Ը���
            abs(Left_Line[i + 1] - Left_Line[i + 2]) <= 5 &&
            abs(Left_Line[i + 2] - Left_Line[i + 3]) <= 5 &&
            (Left_Line[i] - Left_Line[i - 2]) >= 5 &&
            (Left_Line[i] - Left_Line[i - 3]) >= 10 &&
            (Left_Line[i] - Left_Line[i - 4]) >= 10)
        {
            left_down_line = i; // ��ȡ��������
            break;
        }
    }
    return left_down_line;
}

/**
 * @brief findLeftUpPoint
 * @note ���ĸ��ǵ㣬����ֵ�ǽǵ����ڵ�����
 * @return
 */
int findLeftUpPoint(int start, int end) // ���ĸ��ǵ㣬����ֵ�ǽǵ����ڵ�����
{
    int i, t;
    int left_up_line = 0;
    //    if (Left_Line_Lost_Count >= 0.9 * MT9V03X_H) // �󲿷ֶ����ߣ�û�йյ��жϵ�����
    //        return left_up_line;
    if (start < end)
    {
        t = start;
        start = end;
        end = t;
    }
    if (end <= MT9V03X_H - Search_Stop_Line) // ������ֹ�����ϵ�ȫ������
        end = MT9V03X_H - Search_Stop_Line;
    if (end <= 5) // ��ʱ����зǳ�����ҲҪ�������ֵ㣬��ֹ����Խ��
        end = 5;
    if (start >= MT9V03X_H - 1 - 5)
        start = MT9V03X_H - 1 - 5;
    for (i = start; i >= end; i--)
    {
        if (left_up_line == 0 && // ֻ�ҵ�һ�����������ĵ�
            abs(Left_Line[i] - Left_Line[i - 1]) <= 5 &&
            abs(Left_Line[i - 1] - Left_Line[i - 2]) <= 5 &&
            abs(Left_Line[i - 2] - Left_Line[i - 3]) <= 5 &&
            (Left_Line[i] - Left_Line[i + 2]) >= 8 &&
            (Left_Line[i] - Left_Line[i + 3]) >= 15 &&
            (Left_Line[i] - Left_Line[i + 4]) >= 15)
        {
            left_up_line = i; // ��ȡ��������
            break;
        }
    }
    return left_up_line; // �����MT9V03X_H-1��˵��û����ô���յ�
}
/**
 * @brief findRightDownPoint
 * @note ���ĸ��ǵ㣬����ֵ�ǽǵ����ڵ�����
 * @return
 */
int findRightDownPoint(int start, int end) // ���ĸ��ǵ㣬����ֵ�ǽǵ����ڵ�����
{
    int i, t;
    int right_down_line = 0;
    // if (Right_Line_Lost_Count >= 0.9 * MT9V03X_H) // �󲿷ֶ����ߣ�û�йյ��жϵ�����
    //     return right_down_line;
    if (start < end)
    {
        t = start;
        start = end;
        end = t;
    }
    if (start >= MT9V03X_H - 1 - 5) // ����5�����ݲ��ȶ���������Ϊ�߽�����жϣ�����
        start = MT9V03X_H - 1 - 5;
    if (end <= MT9V03X_H - Search_Stop_Line)
        end = MT9V03X_H - Search_Stop_Line;
    if (end <= 5)
        end = 5;
    for (i = start; i >= end; i--)
    {
        if (right_down_line == 0 &&                        // ֻ�ҵ�һ�����������ĵ�
            abs(Right_Line[i] - Right_Line[i + 1]) <= 5 && // �ǵ����ֵ���Ը���
            abs(Right_Line[i + 1] - Right_Line[i + 2]) <= 5 &&
            abs(Right_Line[i + 2] - Right_Line[i + 3]) <= 5 &&
            (Right_Line[i] - Right_Line[i - 2]) <= -5 &&
            (Right_Line[i] - Right_Line[i - 3]) <= -10 &&
            (Right_Line[i] - Right_Line[i - 4]) <= -10)
        {
            right_down_line = i; // ��ȡ��������
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
int findRightUpPoint(int start, int end) // ���ĸ��ǵ㣬����ֵ�ǽǵ����ڵ�����
{
    int i, t;
    int right_up_line = 0;

    if (start < end)
    {
        t = start;
        start = end;
        end = t;
    }
    if (end <= MT9V03X_H - Search_Stop_Line) // ������ֹ�����ϵ�ȫ������
        end = MT9V03X_H - Search_Stop_Line;
    if (end <= 5) // ��ʱ����зǳ�����ҲҪ�������ֵ㣬��ֹ����Խ��
        end = 5;
    if (start >= MT9V03X_H - 1 - 5)
        start = MT9V03X_H - 1 - 5;
    for (i = start; i >= end; i--)
    {
        if (right_up_line == 0 &&                          // ֻ�ҵ�һ�����������ĵ�
            abs(Right_Line[i] - Right_Line[i - 1]) <= 5 && // ��������λ�ò��
            abs(Right_Line[i - 1] - Right_Line[i - 2]) <= 5 &&
            abs(Right_Line[i - 2] - Right_Line[i - 3]) <= 5 &&
            (Right_Line[i] - Right_Line[i + 2]) <= -8 &&
            (Right_Line[i] - Right_Line[i + 3]) <= -15 &&
            (Right_Line[i] - Right_Line[i + 4]) <= -15)
        {
            right_up_line = i; // ��ȡ��������
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
    if (start >= MT9V03X_H - 1 - 5) // ����5�����ݲ��ȶ���������Ϊ�߽�����жϣ�����
        start = MT9V03X_H - 1 - 5;
    if (end <= MT9V03X_H - Search_Stop_Line)
        end = MT9V03X_H - Search_Stop_Line;
    if (end <= 5)
        end = 5;
    for (i = start; i >= end; i--)
    {
        if (Left_Down_Find == 0 &&                       // ֻ�ҵ�һ�����������ĵ�
            abs(Left_Line[i] - Left_Line[i + 1]) <= 5 && // �ǵ����ֵ���Ը���
            abs(Left_Line[i + 1] - Left_Line[i + 2]) <= 5 &&
            abs(Left_Line[i + 2] - Left_Line[i + 3]) <= 5 &&
            (Left_Line[i] - Left_Line[i - 2]) >= 8 &&
            (Left_Line[i] - Left_Line[i - 3]) >= 10 &&
            (Left_Line[i] - Left_Line[i - 4]) >= 10)
        {
            Left_Down_Find = i; // ��ȡ��������
        }
        if (Right_Down_Find == 0 &&                        // ֻ�ҵ�һ�����������ĵ�
            abs(Right_Line[i] - Right_Line[i + 1]) <= 5 && // �ǵ����ֵ���Ը���
            abs(Right_Line[i + 1] - Right_Line[i + 2]) <= 5 &&
            abs(Right_Line[i + 2] - Right_Line[i + 3]) <= 5 &&
            (Right_Line[i] - Right_Line[i - 2]) <= -8 &&
            (Right_Line[i] - Right_Line[i - 3]) <= -15 &&
            (Right_Line[i] - Right_Line[i - 4]) <= -15)
        {
            Right_Down_Find = i;
        }
        if (Left_Down_Find != 0 && Right_Down_Find != 0) // �����ҵ����˳�
        {
            break;
        }
    }
}
/**
 * @brief Find_Up_Point
 * @param start
 * @param end
 * @return �ں��������޸�RightUpFind��LeftUpFind������ֵ
 * @note  start�϶�Ҫ��end��ע��һ��
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
    if (end <= 5) // ��ʱ����зǳ�����ҲҪ�������ֵ㣬��ֹ����Խ��
        end = 5;
    if (start >= MT9V03X_H - 1 - 5) // ����5�����ݲ��ȶ���������Ϊ�߽�����жϣ�����
        start = MT9V03X_H - 1 - 5;
    for (i = start; i >= end; i--)
    {
        if (Left_Up_Find == 0 && // ֻ�ҵ�һ�����������ĵ�
            abs(Left_Line[i] - Left_Line[i - 1]) <= 5 &&
            abs(Left_Line[i - 1] - Left_Line[i - 2]) <= 5 &&
            abs(Left_Line[i - 2] - Left_Line[i - 3]) <= 5 &&
            (Left_Line[i] - Left_Line[i + 2]) >= 8 &&
            (Left_Line[i] - Left_Line[i + 3]) >= 15 &&
            (Left_Line[i] - Left_Line[i + 4]) >= 15)
        {
            Left_Up_Find = i; // ��ȡ��������
        }
        if (Right_Up_Find == 0 &&                          // ֻ�ҵ�һ�����������ĵ�
            abs(Right_Line[i] - Right_Line[i - 1]) <= 5 && // ��������λ�ò��
            abs(Right_Line[i - 1] - Right_Line[i - 2]) <= 5 &&
            abs(Right_Line[i - 2] - Right_Line[i - 3]) <= 5 &&
            (Right_Line[i] - Right_Line[i + 2]) <= -8 &&
            (Right_Line[i] - Right_Line[i + 3]) <= -15 &&
            (Right_Line[i] - Right_Line[i + 4]) <= -15)
        {
            Right_Up_Find = i; // ��ȡ��������
        }
        if (Left_Up_Find != 0 && Right_Up_Find != 0) // ���������ҵ��ͳ�ȥ
        {
            break;
        }
    }
    if (abs(Right_Up_Find - Left_Up_Find) >= 30) // ����˺�ѹ�����Ϊ����
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
    int down_search_start = 0; // �µ�������ʼ��
    Cross_Flag = 0;
    if (Island_State == 0 && Ramp_Flag == 0) // �뻷�����⿪
    {
        Left_Up_Find = 0;
        Right_Up_Find = 0;
        if (Both_Lost_Times >= 10) // ʮ�ֱض���˫�߶��ߣ�����˫�߶��ߵ�������ٿ�ʼ�ҽǵ�
        {
            findUpPoint(MT9V03X_H - 1, 0);
            if (Left_Up_Find == 0 && Right_Up_Find == 0) // ֻҪû��ͬʱ�ҵ������ϵ㣬ֱ�ӽ���
            {
                return;
            }
        }
        if (Left_Up_Find != 0 && Right_Up_Find != 0) // �ҵ������ϵ㣬���ҵ�ʮ����
        {
            Cross_Flag = 1;                                                                  // ��Ӧ��־λ�����ڸ�Ԫ�ػ����
            down_search_start = Left_Up_Find > Right_Up_Find ? Left_Up_Find : Right_Up_Find; // �������Ϲյ����꿿������Ϊ�µ����������
            findDownPoint(MT9V03X_H - 5, down_search_start + 2);                             // ���Ϲյ���2����Ϊ�µ�Ľ�ֹ��
            if (Left_Down_Find <= Left_Up_Find)
            {
                Left_Down_Find = 0; // �µ㲻���ܱ��ϵ㻹����
            }
            if (Right_Down_Find <= Right_Up_Find)
            {
                Right_Down_Find = 0; // �µ㲻���ܱ��ϵ㻹����
            }
            if (Left_Down_Find != 0 && Right_Down_Find != 0)
            { // �ĸ��㶼�ڣ��������ߣ����������Ȼ����
                // Left_Add_Line(Left_Line[Left_Up_Find], Left_Up_Find, Left_Line[Left_Down_Find], Left_Down_Find);
                addLeftLine(Left_Down_Find, Left_Line[Left_Down_Find], Left_Up_Find, Left_Line[Left_Up_Find]);
                // Right_Add_Line(Right_Line[Right_Up_Find], Right_Up_Find, Right_Line[Right_Down_Find], Right_Down_Find);
                addRightLine(Right_Down_Find, Right_Line[Right_Down_Find], Right_Up_Find, Right_Line[Right_Up_Find]);
            }
            else if (Left_Down_Find == 0 && Right_Down_Find != 0) // ���û�ҵ����ҵ��ҵ���
            {                                                     // ������
                lengthenLeftBoundry(Left_Up_Find - 1, MT9V03X_H - 1);
                addRightLine(Right_Down_Find, Right_Line[Right_Down_Find], Right_Up_Find, Right_Line[Right_Up_Find]);
            }
            else if (Left_Down_Find != 0 && Right_Down_Find == 0) // ����ҵ��ˣ��ҵ�û�ҵ�
            {                                                     // ������
                addLeftLine(Left_Down_Find, Left_Line[Left_Down_Find], Left_Up_Find, Left_Line[Left_Up_Find]);
                lengthenRightBoundry(Right_Up_Find - 1, MT9V03X_H - 1);
            }
            else if (Left_Down_Find == 0 && Right_Down_Find == 0) // ���������㶼û�ҵ�
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
 * @param    (x1, y1)��Ӧ����down��,(x2, y2)��Ӧ����up��
 */
void addLeftLine(int x1, int y1, int x2, int y2)
{
    /* ������˵x1 > x2 , y1 < y2 */
    int hx;
    if (x1 >= MT9V03X_H - 1) // ��ʼ��λ��У�����ų�����Խ��Ŀ���
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
    float slope = (float)(y2 - y1) / (x2 - x1); // ����б��
    for (int x = x1; x >= x2; --x)
    {
        hx = slope * (x - x2) + y2;
        Left_Line[x] = hx; // ��������ĵ����Left_Line
    }
}
/**
 * @brief addRightLine
 * @param x3
 * @param y3
 * @param x4
 * @param y4
 * @note       (x3, y3)��Ӧdown��, (x4, y4)��Ӧup��
 */
void addRightLine(int x3, int y3, int x4, int y4)
{
    /* ������˵x3 > x4 , y3 > y4 */
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
 * @note    start��Ӧ�ҵ����Ͻǵ��x���꣬end��ӦҪ������x���꣬����������endӦ�ñ�start��
 */
void lengthenLeftBoundry(int start, int end)
{
    if (start >= MT9V03X_H - 1) // ��ʼ��λ��У�����ų�����Խ��Ŀ���
        start = MT9V03X_H - 1;
    else if (start <= 0)
        start = 0;
    if (end >= MT9V03X_H - 1)
        end = MT9V03X_H - 1;
    else if (end <= 0)
        end = 0;
    if (end < start) //++���ʣ����껥��
    {
        int t = end;
        end = start;
        start = t;
    }
    if (start <= 5) // ��Ϊ��Ҫ�ڿ�ʼ��������3���㣬������ʼ����ڿ��ϣ��������ӳ���ֻ��ֱ������
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
 * @note    start��Ӧ�ҵ����Ͻǵ��x���꣬end��ӦҪ������x���꣬����������endӦ�ñ�start��
 */
void lengthenRightBoundry(int start, int end)
{
    if (start >= MT9V03X_H - 1) // ��ʼ��λ��У�����ų�����Խ��Ŀ���
        start = MT9V03X_H - 1;
    else if (start <= 0)
        start = 0;
    if (end >= MT9V03X_H - 1)
        end = MT9V03X_H - 1;
    else if (end <= 0)
        end = 0;
    if (end < start) //++���ʣ����껥��
    {
        int t = end;
        end = start;
        start = t;
    }
    if (start <= 5) // ��Ϊ��Ҫ�ڿ�ʼ��������3���㣬������ʼ����ڿ��ϣ��������ӳ���ֻ��ֱ������
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
 * @brief ��image_Binary���滮��
 * @param startX
 * @param startY
 * @param endX
 * @param endY
 * @note  ��β����Ķ�������LeftLine��RightLine���������ˣ���ֱ����image_Binary�����ΪPIXEL_BLACK
 */
void drawLine(int startX, int startY, int endX, int endY)
{
    int i, x, y;
    int start = 0, end = 0;

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
    if (endY >= MT9V03X_W - 1)
        endY = MT9V03X_W - 1;
    else if (endY <= 0)
        endY = 0;

    if (startX == endX) // һ������
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
    else if (startY == endY) // һ������
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
    else // ����������
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
 * @brief ��������
 * @note  ����ǰ����ߣ�����Zebra_FlagΪ1
 */
void zebraStripeDetect()
{
    int j = 0;
    int count = 0;
    // ǰ���Ԫ�ػ��ⲹȫ
    if (Cross_Flag || Island_State)
    {
        return;
    }
    if (Search_Stop_Line >= 55) // ��Ұ�ܳ�
    {
        for (j = 10; j <= MT9V03X_W - 10; j++)
        {
            if (abs(White_Column[j] - White_Column[j + 1]) >= 30)
            {
                count++;
            }
        }
    }
    if (count >= 10 && !Zebra_Flag) // ��ֵ�ֶ�����
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
    // ����ҵ�zebra��ô�������Ͻǽǵ�(��������������˵)
    if (Zebra_Flag == 2)
    {
        //        int garbageSpot = findLeftUpPoint(30, 5);
        //        float k = (float)(Right_Line[garbageSpot] - MT9V03X_W - 20) / (garbageSpot - MT9V03X_H - 1);
        //        drawLineWithSlope(k, MT9V03X_H - 1, MT9V03X_W - 20, garbageSpot);
        //        longestWhiteColumnSearchLines();
    }
}
