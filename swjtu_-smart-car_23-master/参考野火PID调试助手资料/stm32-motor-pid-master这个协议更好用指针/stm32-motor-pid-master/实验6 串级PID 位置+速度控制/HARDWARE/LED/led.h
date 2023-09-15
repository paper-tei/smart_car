//*******************************
//STM32F407 板载LED配置
//File: led.h
//Author: xxpcb(wxgzh:码农爱学习)
//Vesion: V1.0
//Date: 2020/05/30
//*******************************

#ifndef __LED_H
#define __LED_H
#include "sys.h"

//=========================
//板载两个LED,低电平点亮
//A6:D2(重命名为LEDa)
//A7:D3(重命名为LEDb)
//=========================

//LED端口定义
#define RCC_AHB1Periph_LED RCC_AHB1Periph_GPIOA
#define GPIO_LED GPIOA

#define GPIO_Pin_LEDa GPIO_Pin_6
#define GPIO_Pin_LEDb GPIO_Pin_7

#define digitalHigh(p,i)     {p->BSRRH=i;}  //引脚输出高电平       
#define digitalLow(p,i)      {p->BSRRL=i;}   //引脚输出低电平
#define digitalToggle(p,i)   {p->ODR ^=i;} //反转

#define LEDa_OFF      digitalLow(GPIO_LED,GPIO_Pin_LEDa)
#define LEDb_OFF      digitalLow(GPIO_LED,GPIO_Pin_LEDb)

#define LEDa_ON     digitalHigh(GPIO_LED,GPIO_Pin_LEDa)
#define LEDb_ON     digitalHigh(GPIO_LED,GPIO_Pin_LEDb)

#define LEDa_Toggle  digitalToggle(GPIO_LED,GPIO_Pin_LEDa)
#define LEDb_Toggle  digitalToggle(GPIO_LED,GPIO_Pin_LEDb)

void LED_Init(void);//初始化		 				    
#endif
