/*
* Adc_Em.h
*
*  Created on: 2023��3��16��
*      Author: Cappi
*/
#ifndef CODE_ADC_EM_H_
#define CODE_ADC_EM_H_

#include "zf_common_headfile.h"

//ADC通道定义
#define ADC_CHANNEL1            (ADC0_CH2_A2)
#define ADC_CHANNEL2            (ADC0_CH3_A3)
#define ADC_CHANNEL3            (ADC0_CH4_A4)
#define ADC_CHANNEL4            (ADC0_CH5_A5)

#define ADC_CHANNEL5            (ADC0_CH6_A6)
#define ADC_CHANNEL6            (ADC0_CH7_A7)
#define ADC_CHANNEL7            (ADC0_CH8_A8)
extern float adc_angle_1;
extern float adc_angle_2;
extern uint16   AD_val_1;
extern uint16   AD_val_2;
extern uint16   AD_val_3;
extern uint16   AD_val_4;
extern uint16  AD_val_1_max;
extern uint16  AD_val_2_max;
extern uint16  AD_val_3_max;
extern uint16  AD_val_4_max;

extern uint16  AD_val_1_min;
extern uint16  AD_val_2_min;
extern uint16  AD_val_3_min;
extern uint16  AD_val_4_min;


typedef struct {
    float direct;//直电感误差系数
    float incline;//斜
}COefficient;
extern COefficient coefficient_L;

typedef struct {
    float adc_incline;//斜电感差比和error//右拐为负数，左拐为正
    float adc_dierct;//横电感//左拐为负数
    float adc_sum;//加权error电感
}Error;
extern Error Error_Em;


extern int   disgy_AD_val_1, disgy_AD_val_2, disgy_AD_val_3, disgy_AD_val_4;
extern uint16   AD_val_1, AD_val_2, AD_val_3, AD_val_4;
///////电磁组这就是归一化，做电磁的这个必须加，很有用，让你使用不同的赛道，可以直接复制，
////原理很简单车子扫一下就是采集一个电感的最大值，和最小值，然后跑的时候检测到的值除以（最大值减最小值）乘以100,
void Em_black_Init(void);
//读取电感ADC
void getadval(void);
//前瞻error运算
void em_calculate(void);
void Em_black_Scan(void);
#endif 
