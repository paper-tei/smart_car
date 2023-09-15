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
	else if((pwm_input>-50)&&(pwm_input<50)) 
	{
		pwm_output = 0;
	}
	else
	{
		pwm_output = pwm_input;
	}
	
	return pwm_output;
}


extern __IO int16_t EncoderOverflowCnt;

//周期定时器的回调函数
void AutoReloadCallback()
{
	static __IO int encoderNow = 0;    /*当前时刻总计数值*/
    static __IO int encoderLast = 0;   /*上一时刻总计数值*/
	int encoderDelta = 0; /*当前时刻与上一时刻编码器的变化量*/
	
	int res_pwm = 0;/*PID计算得到的PWM值*/
	static int i=0;
	
	/*【1】读取编码器的值*/
	encoderNow = read_encoder() + EncoderOverflowCnt*ENCODER_TIM_PERIOD;/*获取当前的累计值*/
	encoderDelta = encoderNow - encoderLast; /*得到变化值*/
	encoderLast = encoderNow;/*更新上次的累计值*/
 
	/*【2】PID运算，得到PWM控制值*/
    //res_pwm = pwm_val_protect((int)PID_realize(encoderDelta));/*传入编码器的[变化值]，实现电机【速度】控制*/
	res_pwm = pwm_val_protect((int)PID_realize(encoderNow));/*传入编码器的[总计数值]，实现电机【位置】控制*/
	
	/*【3】PWM控制电机*/
	set_motor_rotate(res_pwm);
	
	/*【4】数据上传到上位机显示*/
#if (PID_ASSISTANT_EN)
	i++;
	if(i==12)
	{
		i=0;
		//set_computer_value(SEND_FACT_CMD, CURVES_CH1, &encoderDelta, 1); /*给通道1发送实际的电机【速度】值*/
		set_computer_value(SEND_FACT_CMD, CURVES_CH1, &encoderNow, 1); /*给通道1发送实际的电机【位置】值*/
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
