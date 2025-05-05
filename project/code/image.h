#ifndef IMAGE_H
#define IMAGE_H

#include "zf_common_headfile.h"

//#define MT9V03X_H 60
//#define MT9V03X_W 188

#define compressRatio 2
#define compress_H (MT9V03X_H / compressRatio) // 压缩图的高
#define compress_W (MT9V03X_W / compressRatio) // 压缩图的宽

#define PIXEL_BLACK 0
#define PIXEL_WHITE 255

#define Original 0
#define Binary 1
#define Compress 2

// typedef struct
//{
//     uint8_t Left_Line[MT9V03X_H];   //eg. Left_Line[5] = 72 第五行左边的边界列数为72
//     uint8_t Right_Line[MT9V03X_H];

//    uint8_t Left_Line_Lost[MT9V03X_H]; //左丢线数组，丢线置1，不丢线置0
//    uint8_t Right_Line_Lost[MT9V03X_H];
//    uint8_t Left_Line_Lost_Count;
//    uint8_t Right_Line_Lost_Count;

//    uint8_t Left_Boundry_Start;//第一个非丢线点,常规边界起始点
//    uint8_t Right_Boundry_Start;//第一个非丢线点,常规边界起始点
//    uint8_t Search_Stop_Line;
//} LINE_INFO; // 赛道边界的信息提取,相关特征等

extern uint8_t mt9v03x_image_Binary[MT9V03X_H][MT9V03X_W];
extern uint8_t mt9v03x_image_HorizonCompress[compress_H][compress_W];
// extern LINE_INFO Line_Info;

extern uint8_t Left_Line[MT9V03X_H]; // eg. Left_Line[5] = 72 第五行左边的边界列数为72
extern uint8_t Right_Line[MT9V03X_H];

extern uint8_t Left_Line_Lost[MT9V03X_H]; // 左丢线数组，丢线置1，不丢线置0
extern uint8_t Right_Line_Lost[MT9V03X_H];
extern uint8_t Left_Line_Lost_Count;
extern uint8_t Right_Line_Lost_Count;
extern uint8_t Both_Lost_Times;

extern uint8_t Left_Boundry_Start;  // 第一个非丢线点,常规边界起始点
extern uint8_t Right_Boundry_Start; // 第一个非丢线点,常规边界起始点
extern uint8_t Search_Stop_Line;

// 元素识别的标志
extern uint8_t Straight_Flag;
extern int Cross_Flag;
extern int Zebra_Flag;
extern uint8_t Ramp_Flag;
extern int zebraChange;
//-----------------------图像处理的相关信息-----------------------//

/* 下面是索引函数，没有实际内容，只是负责ctrl点击直接跳转 */
void Image_Pre_Operation_Functions();
void Image_TrackingBorder_Functions();
void Image_Element_Detecting_Functions();

void setImageTwoValue(uint8_t value);
uint8_t otsuThreshold(uint8_t *image, uint16_t width, uint16_t height);
uint8_t otsuThreshold_fast(uint8_t *image);
void compressImage(void);
void longestWhiteColumnSearchLines();
void drawLine(int startX, int startY, int endX, int endY);

void straightDetect();
void elementDetecting();
void Err_Sum();

/* 检测十字路口的函数 */
int findLeftDownPoint(int start, int end);
int findRightDownPoint(int start, int end);
int findLeftUpPoint(int start, int end);
int findRightUpPoint(int start, int end);
void findDownPoint(int start, int end);
void findUpPoint(int start, int end);

void addLeftLine(int x1, int y1, int x2, int y2);
void addRightLine(int x3, int y3, int x4, int y4);
void lengthenLeftBoundry(int start, int end);
void lengthenRightBoundry(int start, int end);
void crossDetect();

#endif // IMAGE_H
