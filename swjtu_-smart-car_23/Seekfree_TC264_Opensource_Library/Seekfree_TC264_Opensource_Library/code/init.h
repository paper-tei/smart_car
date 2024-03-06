/*
 * init.h
 *
 *  Created on: 2022年1月6日
 *      Author: paper
 */

#ifndef CODE_INIT_H_
#define CODE_INIT_H_
#include "zf_common_headfile.h"

 // 定义蜂鸣器引脚
#define BEEP_PIN P33_4
// 定义电机相关引脚
#define BLDC_F_PWM_PIN ATOM0_CH0_P21_2  // 定义前_无刷电机PWM引脚
#define BLDC_B_PWM_PIN ATOM0_CH1_P21_3  // 定义后_无刷电机PWM引脚
#define BDC_PWM_PIN ATOM1_CH2_P21_4     // 定义有刷电机PWM引脚
#define BDC_DIR_PIN ATOM1_CH6_P23_1     // 定义有刷电机方向引脚
#define BLDC_F_STOP_PIN P22_2           // 定义前_无刷电机刹车引脚
#define BLDC_B_STOP_PIN P22_3           // 定义后_无刷电机刹车引脚
#define BLDC_F_DIR_PIN P22_0            // 定义前_无刷电机方向引脚
#define BLDC_B_DIR_PIN P22_1            // 定义后_无刷电机方向引脚
#define SERVO_A_PWM_PIN ATOM3_CH0_P33_10            // 定义舵机PWM引脚
#define SERVO_B_PWM_PIN ATOM3_CH4_P33_12            // 定义舵机PWM引脚

#define A_F_PIN ADC0_CH0_A0             // 定义前_无刷电机电流引脚
#define A_B_PIN ADC0_CH1_A1             // 定义后_无刷电机电流引脚
// 定义按键引脚
#define KEY1 P33_8
#define KEY2 P33_7
#define KEY3 P33_6
#define KEY4 P33_5
#define KEY5 P20_6
#define KEY6 P20_7
#define KEY7 P20_8
#define KEY8 P20_9
// 定义拨码开关引脚
#define SW1 P33_9
#define SW2 P33_10
// 定义编码器引脚
#define B_PIN_F TIM4_ENCODER_CH1_P02_8
#define A_PIN_F TIM4_ENCODER_CH2_P00_9
#define B_PIN_B TIM6_ENCODER_CH2_P20_0
#define A_PIN_B TIM6_ENCODER_CH1_P20_3
#define DIR_ENCODER TIM5_ENCODER_CH2_P10_1
#define LSB_ENCODER TIM5_ENCODER_CH1_P10_3

//定义串口下位机相关引脚
#define UART_INDEX_PROTOCOL              UART_2                          //
#define UART_BAUDRATE_PROTOCOL           115200                          // 默认 115200
#define UART_TX_PIN_PROTOCOL             UART2_TX_P10_5
#define UART_RX_PIN_PROTOCOL             UART2_RX_P10_6

#define UART_INDEX_PROTOCOL_0              UART_0                          //
#define UART_BAUDRATE_PROTOCOL_0             115200                          // 默认 115200
#define UART_TX_PIN_PROTOCOL_0               UART0_TX_P14_0
#define UART_RX_PIN_PROTOCOL_0               UART0_RX_P14_1

void Init(void);
void Buzzer(uint8 value);

#define way_Crossroad 0//十字算法选择，1为进去十字，0为不进去
#endif /* CODE_INIT_H_ */
