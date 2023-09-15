/*
 * init.c
 *
 *  Created on: 2022年11月18日
 *      Author: paper
 */

#include "init.h"
void Init(void) {
    //初始化蜂鸣器引脚
    gpio_init(BEEP_PIN, GPO, 0, PUSHPULL);
    Buzzer(1);
    //按键初始化
    gpio_init(KEY1, GPI, 0, PULLUP);
    gpio_init(KEY2, GPI, 0, PULLUP);
    gpio_init(KEY3, GPI, 0, PULLUP);
    gpio_init(KEY4, GPI, 0, PULLUP);
    //拨码开关初始化
    gpio_init(SW1, GPI, 0, PULLUP);
    gpio_init(SW2, GPI, 0, PULLUP);
    //初始化PWM引脚
    gtm_pwm_init(MOTOR1_PWM, 12500, 0);//定义前_无刷电机PWM引脚
    gtm_pwm_init(MOTOR2_PWM, 12500, 0);//定义后_无刷电机PWM引脚
    gtm_pwm_init(MOTOR3_PWM, 12500, 0);//定义有刷电机PWM引脚

    //初始化编码器
    gpt12_init(GPT12_T5, GPT12_T5INB_P10_3, GPT12_T5EUDB_P10_1);
    gpt12_init(GPT12_T6, GPT12_T6INA_P20_3, GPT12_T6EUDA_P20_0);
}