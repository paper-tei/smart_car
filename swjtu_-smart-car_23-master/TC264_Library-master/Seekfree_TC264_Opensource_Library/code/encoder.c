/*
 * encoder.c
 *
 *  Created on: 2022年11月18日
 *      Author: paper
 */

#include "encoder.h"
float low_pass = 0.6;

//面朝车头顺时针转动编码器为正方向
float encoder_bldc_f = 0; // 前无刷电机脉冲计数
float encoder_bldc_b = 0; // 后无刷电机脉冲计数
float encoder_bdc = 0;    // 有刷电机脉冲计数

float encoder_bldc_f_total = 0;
float encoder_bldc_b_total = 0;
float encoder_bdc_total = 0;
int16 encoder_bdc_get = 0;
void encoder_get(void) {
    // 前无刷电机脉冲计数
    int16 encoder_bldc_f_get = encoder_get_count(TIM4_ENCODER);

    // 编码器 低通滤波
    encoder_bldc_f = encoder_bldc_f * low_pass + encoder_bldc_f_get * (1 - low_pass);
    encoder_bldc_f_total += encoder_bldc_f;
    encoder_clear_count(TIM4_ENCODER);

    // 后无刷电机脉冲计数
    int16 encoder_bldc_b_get = encoder_get_count(TIM6_ENCODER);
    // 编码器 低通滤波
    encoder_bldc_b = encoder_bldc_b * low_pass + encoder_bldc_b_get * (1 - low_pass);
    encoder_bldc_b_total += encoder_bldc_b;
    encoder_clear_count(TIM6_ENCODER);

    // 有刷电机脉冲计数
    encoder_bdc_get = encoder_get_count(TIM5_ENCODER);
    // 编码器 低通滤波
    encoder_bdc = encoder_bdc * low_pass + encoder_bdc_get * (1 - low_pass);
    encoder_bdc_total += encoder_bdc;
    encoder_clear_count(TIM5_ENCODER);
}
