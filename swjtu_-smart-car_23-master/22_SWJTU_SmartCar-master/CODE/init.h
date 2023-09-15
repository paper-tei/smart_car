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
#define BEEP_PIN   P33_10       //定义蜂鸣器引脚
#define MOTOR1_PWM   ATOM0_CH0_P21_2    //定义1电机PWM引脚
#define MOTOR2_PWM   ATOM0_CH2_P21_4    //定义2电机PWM引脚
#define MOTOR3_PWM   ATOM0_CH3_P21_5    //定义3电机PWM引脚
#define MOTOR4_PWM   ATOM0_CH1_P21_3    //定义4电机PWM引脚
#define S_MOTOR_PIN   ATOM1_CH1_P33_9       //定义舵机引脚
#endif /* CODE_INIT_H_ */
#define BEEP_PIN   P33_10       //定义蜂鸣器引脚


// #define MOTOR1_DIR   P21_2              //定义1电机方向控制引脚 左下  in12
// #define MOTOR1_PWM   ATOM0_CH1_P21_3    //定义1电机PWM引脚  右下 in11
 
// #define MOTOR2_DIR   P21_4              //定义2电机方向控制引脚  左上
// #define MOTOR2_PWM   ATOM0_CH3_P21_5    //定义2电机PWM引脚   右上
