#include "timer.h"
#include "capture.h"
#include "motor.h"
#include "protocol.h"
#include "common.h"
#include "bsp_pid.h"

//通用定时器7初始化
//arr：自动重装值。
//psc：时钟预分频数
//定时器溢出时间计算方法:Tout=((arr+1)*(psc+1))/Ft us.
//Ft=定时器工作频率,单位:Mhz
void TIMx_calcPID_init(u16 arr,u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7,ENABLE);  ///使能TIM7时钟
	
    TIM_TimeBaseInitStructure.TIM_Period = arr;   //自动重装载值
	TIM_TimeBaseInitStructure.TIM_Prescaler=psc;  //定时器分频
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //向上计数模式
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	TIM_TimeBaseInit(TIM7,&TIM_TimeBaseInitStructure);//初始化TIM7
	
	TIM_ITConfig(TIM7,TIM_IT_Update,ENABLE); //允许定时器6更新中断
	TIM_Cmd(TIM7,DISABLE); //初始化时先不开启定时器7
	
	NVIC_InitStructure.NVIC_IRQChannel=TIM7_IRQn; //定时器6中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x01; //抢占优先级1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x03; //子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

void TIMx_calcPID_start(void)
{
	TIM_Cmd(TIM7,ENABLE); //使能定时器7
}

void TIMx_calcPID_stop(void)
{
	TIM_Cmd(TIM7,DISABLE); //关闭定时器7
}

//PID周期计算与控制
int pwm_val_protect(int pwm_input)
{
	int pwm_output = 0;
	
	if(pwm_input>999) 
	{
		pwm_output = 999;
	}
	else if(pwm_input<-999) 
	{
		pwm_output = -999;
	}
	else if((pwm_input>-100)&&(pwm_input<100)) 
	{
		pwm_output = 0;
	}
	else
	{
		pwm_output = pwm_input;
	}
	
	return pwm_output;
}




//周期定时器的回调函数
void AutoReloadCallback()
{
	int sum = 0;
	int res_pwm = 0;
	static int i=0;
	
	sum = read_encoder();

    res_pwm = pwm_val_protect((int)PID_realize(sum));
	
	set_motor_rotate(res_pwm);
	
#if (PID_ASSISTANT_EN)
	i++;
	if(i==8)
	{
		i=0;
		set_computer_value(SEND_FACT_CMD, CURVES_CH1, &sum, 1); // 给通道 1 发送实际值
	}
#else
	i++;
	if(i==100)
	{
		i=0;
		printf("sum:%d set_pwm:%d\r\n",sum,res_pwm);
		
	}
#endif
}



//定时器7中断服务函数
void TIM7_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM7,TIM_IT_Update)==SET) //溢出中断
	{
		AutoReloadCallback();
	}
	TIM_ClearITPendingBit(TIM7,TIM_IT_Update);  //清除中断标志位
}
