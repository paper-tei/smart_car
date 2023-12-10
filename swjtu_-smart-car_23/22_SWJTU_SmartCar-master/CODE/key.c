/*
 * key.c
 *
 *  Created on: 2022年1月6日
 *      Author: paper
 */
//按键调参用
#include "key.h"
//拨码开关状态变量
uint8 sw1_status;
uint8 sw2_status;
//开关状态变量
uint8 key1_status = 1;
uint8 key2_status = 1;
uint8 key3_status = 1;
uint8 key4_status = 1;
//上一次开关状态变量
uint8 key1_last_status;
uint8 key2_last_status;
uint8 key3_last_status;
uint8 key4_last_status;
//开关标志位
uint8 key1_flag;
uint8 key2_flag;
uint8 key3_flag;
uint8 key4_flag;
//编码器变量
float encoder_left=0;
float encoder_right=0;
float encoder_left_speed = 0,encoder_right_speed = 0;
double encoder_all = 0,encoder_all_home = 0,encoder_all_left = 0,encoder_all_right = 0;
void Key_scan(void)
{
   //清零标志位
   key1_flag=0;
   key2_flag=0;
   key3_flag=0;
   key4_flag=0;
   sw1_status=0;
   sw2_status=0;
   //读取历史按键状态
   key1_last_status = key1_status;
   key2_last_status = key2_status;
   key3_last_status = key3_status;
   key4_last_status = key4_status;
   //读取当前按键状态
   key1_status = gpio_get(KEY1);
   key2_status = gpio_get(KEY2);
   key3_status = gpio_get(KEY3);
   key4_status = gpio_get(KEY4);
   //获取按键开关状态
   if(key1_status && !key1_last_status)    {key1_flag = 1;Speed_Left = Speed_Left + 100;duty++;}
   if(key2_status && !key2_last_status)    {duty--;Speed_Left = Speed_Left - 100; key2_flag = 1;}
   if(key3_status && !key3_last_status)    {key3_flag = 1;}
   if(key4_status && !key4_last_status)    key4_flag = 1;
   //获取拨码开关状态
   sw1_status = gpio_get(SW1);
   sw2_status = gpio_get(SW2);
   if(key4_flag){go=1; Out_Home_flag = 1;icm_Home_flag = 1;Buzzer(0);gpt12_clear(GPT12_T5); gpt12_clear(GPT12_T6);}
    if(key3_flag){go=1;Buzzer(0);icm_Home_flag = 0;gpt12_clear(GPT12_T5); gpt12_clear(GPT12_T6);}
}
void Buzzer(uint8 value)
{
    gpio_set(P33_10,value);
}
//编码器
void Encoder_scan(void)
{
    encoder_left = -gpt12_get(GPT12_T5);
    encoder_right = gpt12_get(GPT12_T6);
    encoder_left_speed -= encoder_left;
    encoder_right_speed += encoder_right;
    gpt12_clear(GPT12_T5);
    gpt12_clear(GPT12_T6);
}
