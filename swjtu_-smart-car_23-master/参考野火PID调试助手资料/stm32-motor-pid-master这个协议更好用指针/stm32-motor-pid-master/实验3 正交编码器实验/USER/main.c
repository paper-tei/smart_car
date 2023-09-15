/**
  ******************************************************************************
  * @file    main.c
  * @version V1.0
  * @date    2021-xx-xx
  * @brief   正交编码器测试
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
#include "cmd.h"
#include "led.h"
#include "pwm.h"
#include "timer.h"
#include "capture.h"
#include "motor.h"

extern u8  TIM5CH1_CAPTURE_STA;		//输入捕获状态		    				
extern u32	TIM5CH1_CAPTURE_VAL;	//输入捕获值 



extern u8 isReceiveUartCmd;
int main(void)
{ 
	int encoder;
	int t =0;
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
	
	delay_init(168);  //初始化延时函数
	uart_init(115200);//初始化串口波特率为115200
	
	MOTOT_Init();/*电机控制引脚初始化*/
	TIM3_PWM_Init(1000-1,84-1);/*输出PWM使电机转动*/

	TIM4_ENCODER_Init();/*捕获正交编码器的脉冲*/
	TIM7_Int_Init(1000-1,8400-1);/*定时，周期性进行速度计算*/
	/*8400分频到10kHz, 即1ms计数10次，ARR设为1000，即100ms溢出一次，实现每100ms计算一次转速*/

	while(1)
	{
		if(isReceiveUartCmd)/*接收串口的PWM调速指令*/
		{
			isReceiveUartCmd = 0;
			process_uart_data();
		}
	}
}
