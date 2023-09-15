/*
 * error.h
 *
 *  Created on: 20
 *      Author: paper
 */
 //#ifndef CODE_ERROR_H_
 //#define CODE_ERROR_H_


#include "zf_common_headfile.h"
void get_deviation(void);
void Weighted_error(void);
extern float  image_Error;
void BDC_speed_PID_change(float x1, float y1, float x2, float y2);
extern uint8 unlost_R, unlost_L;
extern float straight_flag_error;
extern uint8 Track_Type, Track_Type_last;

#define Straight 0
#define Left     1
#define Right    2
#define Ring_in_out   3
#define Wobble    4
#define Ring_turn 5
//#endif

