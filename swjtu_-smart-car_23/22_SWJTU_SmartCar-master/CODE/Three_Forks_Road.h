/*
 * Three_Forks_Road.h
 *
 *  Created on: 2022Äê1ÔÂ13ÈÕ
 *      Author: paper
 */

#ifndef CODE_THREE_FORKS_ROAD_H_
#define CODE_THREE_FORKS_ROAD_H_
#include "headfile.h"
void Three_Forks_Road_scan(void);
void Three_Forks_Road_ru(void);
void Three_Forks_Road_chu(void);
extern int32 icm_san_cha;
extern uint8 san_cha_flag,left_san_cha_hang,right_san_cha_hang,san_cha_point;
extern  uint8 san_cha_tiaobian,left_san_cha_chu_lost,right_san_cha_chu_lost;
extern uint8 left_san_cha_lose,right_san_cha_lose,left_san_cha_find,right_san_cha_find,in_san_cha;
extern uint8 real_endline;
extern uint8 Number_of_san_cha ,san_cha_ok_flag,icm_san_cha_flag ;
extern uint8 san_cha_xiao_L,san_cha_xiao_R;
extern uint8 encoder_san_cha_flag;
extern int   encoder_san_cha;
#endif /* CODE_THREE_FORKS_ROAD_H_ */
