/**
  ******************************************************************************
  * @file    main.c
  * @version V1.0
  * @date    2021-xx-xx
  * @brief   PWM输出测试
  ******************************************************************************
  * @attention
  *
  * 实验平台:STM32F407 
  * 公众号  :码农爱学习(Coder-love-study)
  * 个人博客:https://xxpcb.gitee.io
  *
  ******************************************************************************
  */ 
#include "sys.h"
#include "delay.h"
#include "led.h"
#include "pwm.h"

int tmp1,tmp2;
int main(void)
{ 
	u16 led0pwmval=0;    
	u8 dir=1;
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
	
	delay_init(168);  //初始化延时函数
	
 	TIM3_PWM_Init(500-1,84-1);	//84M/84=1Mhz的计数频率,重装载值500，所以PWM频率为 1M/500=2Khz.  
	
	while(1) //实现比较值从0-300递增，到300后从300-0递减，循环
	{
		//tmp1 = GPIO_ReadOutputDataBit(GPIOA,GPIO_Pin_6);
		//tmp2 = GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_6);
 		delay_ms(10);
		
		if(dir)
		{
			led0pwmval+=5;//dir==1 led0pwmval递增
		}
		else 
		{
			led0pwmval-=5;	//dir==0 led0pwmval递减 
		}
 		if(led0pwmval>500)
		{
			dir=0;//led0pwmval到达500后，方向为递减
		}
		if(led0pwmval==0)
		{
			dir=1;	//led0pwmval递减到0后，方向改为递增
		}
 
		TIM_SetCompare1(TIM3,led0pwmval);	/*CCR 修改比较值（占空比）*/
	}
}
