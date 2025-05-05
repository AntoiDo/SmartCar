/*
 * island.h
 *
 *  Created on: 2024��10��12��
 *      Author: cjhdd
 */

#ifndef ISLAND_H_
#define ISLAND_H_

#include "zf_common_headfile.h"

extern int Island_State;        // ����״̬��־
extern int Left_Island_Flag;    // ���һ�����־
extern int Right_Island_Flag;   // ���һ�����־
extern int Left_Up_Turning[2];  // �ĸ��յ������洢��[0]��y����ĳ�У�{1}��x����ĳ��
extern int Right_Up_Turning[2]; // �ĸ��յ������洢��[0]��y����ĳ�У�{1}��x����ĳ��
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
