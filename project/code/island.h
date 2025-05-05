/*
 * island.h
 *
 *  Created on: 2024年10月12日
 *      Author: cjhdd
 */

#ifndef ISLAND_H_
#define ISLAND_H_

#include "zf_common_headfile.h"

extern int Island_State;        // 环岛状态标志
extern int Left_Island_Flag;    // 左右环岛标志
extern int Right_Island_Flag;   // 左右环岛标志
extern int Left_Up_Turning[2];  // 四个拐点的坐标存储，[0]存y，第某行，{1}存x，第某列
extern int Right_Up_Turning[2]; // 四个拐点的坐标存储，[0]存y，第某行，{1}存x，第某列
extern int island_3_cnt;


void islandDetect();
int ContinuityChangeLeft(int start, int end);
int ContinuityChangeRight(int start, int end);
int MonotonicityChangeLeft(int start, int end);
int MonotonicityChangeRight(int start, int end);

void drawLineWithSlope(float slope, int startX, int startY, int endX);
void drawLeftBoundryWithSlope(float slope, int startX, int startY, int endX);
void drawRightBoundryWithSlope(float slope, int startX, int startY, int endX);

#endif /* CODE_ISLAND_H_ */
