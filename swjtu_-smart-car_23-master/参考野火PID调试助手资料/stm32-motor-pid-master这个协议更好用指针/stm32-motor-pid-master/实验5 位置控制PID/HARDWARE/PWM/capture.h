#ifndef _CAPTURE_H
#define _CAPTURE_H

#include "sys.h"

#define ENCODER_TIM_PSC  0          /*计数器分频*/
#define ENCODER_TIM_PERIOD  65535   /*计数器最大值*/
#define CNT_INIT 0                  /*计数器初值*/

#define ENCODER_RESOLUTION 11    /*编码器一圈的物理脉冲数*/
#define ENCODER_MULTIPLE 4       /*编码器倍频，通过定时器的编码器模式设置*/
#define MOTOR_REDUCTION_RATIO 34 /*电机的减速比*/
/*电机转一圈总的脉冲数(定时器能读到的脉冲数) = 编码器物理脉冲数*编码器倍频*电机减速比 */
/* 11*4*34= 1496*/
#define TOTAL_RESOLUTION ( ENCODER_RESOLUTION*ENCODER_MULTIPLE*MOTOR_REDUCTION_RATIO ) 

void TIMx_encoder_init(void);
void calc_motor_rotate_speed(void);
uint32_t read_encoder(void);
#endif
