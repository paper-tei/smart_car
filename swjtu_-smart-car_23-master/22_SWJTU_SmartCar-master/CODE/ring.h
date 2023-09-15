/*
 * ring.h
 *
 *  Created on: 2022Äê1ÔÂ15ÈÕ
 *      Author: paper
 */

#ifndef _IMAGE_RING_H_
#define _IMAGE_RING_H_
#include "headfile.h"
void ring_find(void);
void ring_ru(void);
void ring_chu(void);
void ring_previous_find(void);
extern uint16 ringflag,ring_tiaobian,R_lost_cnt,ring_white,ring_white_di,ring_ok_flag,ring_end_R,ring_end_L;
extern uint16 right_B,ring_end_L;
extern uint16 ring_previous;
extern uint16 ring_cnt;
extern int ring_B_L,ring_B_R;
extern uint16 ring_start_L,ALL_find_cnt;
extern int xie,ring_chu_num;
extern uint8 icm_ring_flag, number_of_ring ;
extern int32 icm_ring ;
void ring_start(void);
extern int32 Error_ring_min ;
int regression(int startline,int endline,int p[]);
#endif
 /* CODE_RING_H_ */
