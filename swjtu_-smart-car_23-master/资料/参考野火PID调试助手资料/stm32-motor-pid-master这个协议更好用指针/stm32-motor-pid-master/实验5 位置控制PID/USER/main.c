/**
  ******************************************************************************
  * @file    main.c
  * @version V1.0
  * @date    2021-xx-xx
  * @brief   PID位置控制测试
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
#include "timer.h"
#include "capture.h"
#include "motor.h"
#include "protocol.h"
#include "bsp_pid.h"
#include "common.h"



int main(void)
{ 
	int32_t target_speed = 0;
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
	
	delay_init(168);  //初始化延时函数
	
	/*电机控制引脚初始化*/
	MOTOT_Init();
	
	/*输出PWM初始化*/
	TIMx_pwm_init(1000-1,84-1);

	/*正交编码器获取初始化*/
	TIMx_encoder_init();
	
	/*8400分频到10kHz, 即1ms计数10次，ARR设为100，即10ms溢出一次，实现每10ms计算一次pid*/
	TIMx_calcPID_init(100-1,8400-1);/*定时10ms*/
	
	/*协议数据解析初始化*/
	protocol_init();

	/*初始化串口波特率为115200*/
	uart_init(115200);

    /* PID 参数初始化 */
	PID_param_init();
	
#if defined(PID_ASSISTANT_EN)
	/*初始化时，上发stop，同步上位机的启动按钮状态*/
	set_computer_value(SEND_STOP_CMD, CURVES_CH1, NULL, 0);  

	/*获取默认的目标值*/
	target_speed = (int32_t)get_pid_target();
	/*给通道1发送目标值*/
	set_computer_value(SEND_TARGET_CMD, CURVES_CH1, &target_speed, 1);     
#endif
	
	while(1)
	{
		/* 接收数据处理 */
		receiving_process();
	}
}
