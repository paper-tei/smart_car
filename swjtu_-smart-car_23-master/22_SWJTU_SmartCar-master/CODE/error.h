/*
 * error.h
 *
 *  Created on: 2022Äê1ÔÂ6ÈÕ
 *      Author: paper
 */
#include "headfile.h"
void get_deviation(void);
void Weighted_error(void);
void DM_Control(void);
extern int32 Error;
extern uint8 min_break_L,min_break_R;
extern float Differential_speed;
extern uint8 unlost;
extern int32 LastError;
extern uint8 Track_Type;
extern int16 yawspeed;
