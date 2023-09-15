//*******************************
//STM32F407 板载LED配置
//File: led.c
//Author: xxpcb(wxgzh:码农爱学习)
//Version: V1.0
//Date: 2020/05/30
//*******************************

#include "led.h" 

//===========================================
//LED IO初始化
//初始化PA6和PA7为输出口.并使能这两个口的时钟		    
//===========================================
void LED_Init(void)
{    	 
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_LED, ENABLE);//使能GPIOA时钟

	//GPIOA6,A7初始化设置
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_LEDa | GPIO_Pin_LEDb;//LEDa和LEDb对应IO口
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
	GPIO_Init(GPIO_LED, &GPIO_InitStructure);//初始化GPIO

	GPIO_SetBits(GPIO_LED,GPIO_Pin_LEDa | GPIO_Pin_LEDb);//设置高，灯灭
}
