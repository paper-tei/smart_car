/*
 * time.h
 *
 *  Created on: 2022年11月18日
 *      Author: paper
 */
#ifndef CODE_TIME_H_
#define CODE_TIME_H_
#include "zf_common_headfile.h"
typedef struct {
    unsigned char T_2ms;
    unsigned char T_6ms;
    unsigned char T_10ms;
    unsigned char T_20ms;
    unsigned char T_100ms;
    unsigned char T_1s;


}Timer_0;
extern uint8 time_stop;
extern Timer_0 Flag;
void Fuse_result(void);
extern float Mechanical_zero, servo_pwm;
extern uint8 flag_pose_1s, rst_flag;
extern uint32 time_1s;

#endif 
