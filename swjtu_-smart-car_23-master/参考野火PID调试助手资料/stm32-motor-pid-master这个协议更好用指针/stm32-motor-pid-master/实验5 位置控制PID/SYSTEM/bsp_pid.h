#ifndef __BSP_PID_H
#define	__BSP_PID_H
#include "stm32f4xx.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

typedef struct
{
	float target_val;   //目标值
	float err;          //偏差值
	float err_last;     //上一个偏差值
	float Kp,Ki,Kd;     //比例、积分、微分系数
	float integral;     //积分值
	float output_val;   //输出值
}PID;

void  PID_param_init(void);
void  set_pid_target(float temp_val);
float get_pid_target(void);
void  set_p_i_d(float p, float i, float d);
float PID_realize(float actual_val);
//void  time_period_fun(void);

#endif
