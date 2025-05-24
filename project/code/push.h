#ifndef PUSH_H
#define PUSH_H

#include "zf_common_headfile.h"
#include "global.h"
#define PIC_WIDTH (160)
#define PIC_HEIGHT (120)

typedef struct
{
    uint8_t res_x1;
    uint8_t res_y1;
    uint8_t res_x2;
    uint8_t res_y2;
} od_result_t;

extern volatile od_result_t od_result[10];

void detectRedBlock();
void detectBlockStable();
int8 getCenterOffset_XAxis();
int8 getCenterOffset_YAxis();

// void rotateCar(Rotate_Direction direction);


#endif