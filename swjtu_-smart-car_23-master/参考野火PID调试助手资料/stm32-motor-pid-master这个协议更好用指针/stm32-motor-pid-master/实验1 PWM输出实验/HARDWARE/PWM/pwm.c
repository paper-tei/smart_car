#include "pwm.h"
#include "led.h"
#include "usart.h"

/**
* @brief TIM3 PWM部分初始化 
* @param arr：自动重装值
*        psc：时钟预分频数
*/
 void TIM3_PWM_Init(u32 arr,u32 psc)
{		 					 
	GPIO_InitTypeDef GPIO_InitStructure; /*引脚配置 结构体*/
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure; /*时基 结构体*/
	TIM_OCInitTypeDef  TIM_OCInitStructure; /*输出通道 结构体*/
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);  	//TIM3时钟使能    
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE); 	//使能PORTA时钟	
	
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource6,GPIO_AF_TIM3);  /*GPIOA6复用为定时器3*/
	
	/*复用引脚配置*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;           //GPIOA6
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;        /*复用功能*/
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//速度100MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;      //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;        //上拉
	GPIO_Init(GPIOA,&GPIO_InitStructure);               //初始化PA6
	  
	/*时基初始化*/
	TIM_TimeBaseStructure.TIM_Period=arr;                     /*ARR 自动重装载值(周期)，例如500*/
	TIM_TimeBaseStructure.TIM_Prescaler=psc;                  /*PSC 定时器分频，例如84*/
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;     /*时钟分割*/
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; /*向上计数模式*/
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseStructure);            /*初始化定时器3*/
	
	/*输出通道初始化，初始化TIM3 Channel1 PWM模式*/	 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;              /*选择定时器模式:TIM脉冲宽度调制模式1*/
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;  //比较输出使能
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;      /*输出极性:TIM输出比较极性高*/
	TIM_OC1Init(TIM3, &TIM_OCInitStructure);                       //根据指定的参数初始化外设TIM3 OC1

	TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);  /*使能TIM3在CCR1上的预装载寄存器*/
 
	TIM_ARRPreloadConfig(TIM3,ENABLE);/*ARPE使能：使能控制寄存器CR的第8位：ARPR, Auto-reload preload enable*/
	
	TIM_Cmd(TIM3, ENABLE);  /*使能TIM3：使能控制寄存器CR的第0位：CEN, counter enable*/								  
}  


