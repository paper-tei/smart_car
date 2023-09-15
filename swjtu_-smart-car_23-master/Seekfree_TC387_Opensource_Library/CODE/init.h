/*
 * init.h
 *
 *  Created on: 2022年1月6日
 *      Author: paper
 */

#ifndef CODE_INIT_H_
#define CODE_INIT_H_
#include "headfile.h"
void Init(void);

#define BEEP_PIN     P33_4      //定义蜂鸣器引脚
#define MOTOR1_PWM   ATOM0_CH0_P21_2    //定义前_无刷电机PWM引脚
#define MOTOR2_PWM   ATOM0_CH2_P21_3    //定义后_无刷电机PWM引脚
#define MOTOR3_PWM   ATOM0_CH3_P21_4    //定义有刷电机PWM引脚
//定义按键引脚
#define KEY1    P33_7
#define KEY2    P33_5
#define KEY3    P33_8
#define KEY4    P33_6
//定义拨码开关引脚
#define SW1     P33_9
#define SW2     P33_10

#endif /* CODE_INIT_H_ */


