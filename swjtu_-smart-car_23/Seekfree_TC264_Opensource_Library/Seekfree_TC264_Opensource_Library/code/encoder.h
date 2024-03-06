/*
 * encoder.h
 *
 *  Created on: 2022年12月16日
 *      Author: paper
 */
#ifndef CODE_ENCODER_H_
#define CODE_ENCODER_H_
#include "zf_common_headfile.h"

extern float encoder_bldc_f;//前无刷电机脉冲计数
extern float encoder_bldc_b;//后无刷电机脉冲计数
extern float encoder_bdc;   //有刷电机脉冲计数
extern int16 encoder_bdc_get;
extern float encoder_bldc_f_total;
extern float encoder_bldc_b_total;
extern float encoder_bdc_total;
void encoder_get(void);
#endif 
