/**
  ******************************************************************************
  * @file    main.c
  * @version V1.0
  * @date    2021-xx-xx
  * @brief   输入捕获测试
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
#include "usart.h"
#include "led.h"
#include "pwm.h"
#include "capture.h"

extern u8  TIM5CH1_CAPTURE_STA;		//输入捕获状态		    				
extern u32	TIM5CH1_CAPTURE_VAL;	//输入捕获值 


int main(void)
{ 
	u16 led0pwmval=0;    
	u8 dir=1;
	long long temp=0; 
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
	
	delay_init(168);  //初始化延时函数
	uart_init(115200);//初始化串口波特率为115200
	
 	TIM3_PWM_Init(500-1,84-1);	//84M/84=1Mhz的计数频率,重装载值500，所以PWM频率为 1M/500=2Khz.  
	
	TIM5_CH1_Cap_Init(0XFFFFFFFF,84-1); //以1Mhz的频率计数（计1个数耗时1us） 
	
	while(1) //实现比较值从0-300递增，到300后从300-0递减，循环
	{
 		delay_ms(10);
		
		if(dir)
		{
			led0pwmval+=5;//dir==1 led0pwmval递增
		}
		else 
		{
			led0pwmval-=5;	//dir==0 led0pwmval递减 
		}
 		if(led0pwmval>300)
		{
			dir=0;//led0pwmval到达300后，方向为递减
		}
		if(led0pwmval==0)
		{
			dir=1;	//led0pwmval递减到0后，方向改为递增
		}
 
		TIM_SetCompare1(TIM3,led0pwmval);	/*CCR 修改比较值（占空比）*/
		
		
		/* 成功捕获到了一次高电平 (1000 0000) */
		if(TIM5CH1_CAPTURE_STA&0X80)        
		{
			temp=TIM5CH1_CAPTURE_STA&0X3F;  /* 获取溢出的次数N (0011 1111) */
			temp*=0XFFFFFFFF;		 		/* 溢出时间总和 = N*溢出计数值 */
			temp+=TIM5CH1_CAPTURE_VAL;		/* 总的高电平时间 = 溢出时间总和 + 下降沿时的计数值*/
			
			printf("HIGH:%lld us\r\n",temp); //打印总的高点平时间
			TIM5CH1_CAPTURE_STA=0;			 //开启下一次捕获
		}
	}
}
