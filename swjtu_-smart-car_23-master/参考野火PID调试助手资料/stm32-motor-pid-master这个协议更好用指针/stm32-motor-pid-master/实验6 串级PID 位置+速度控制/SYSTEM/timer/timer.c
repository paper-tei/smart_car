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

//PWM值限制
static int pwm_val_protect(int pwm_input)
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

//目标速度值限制
//#define TARGET_SPEED_MAX   60 // 目标速度的最大值 r/m
static float TARGET_SPEED_MAX = 60.0;
static void speed_val_protect(float *speed_val)
{
	/*目标速度上限处理*/
	if (*speed_val > TARGET_SPEED_MAX)
	{
		*speed_val = TARGET_SPEED_MAX;
	}
	else if (*speed_val < -TARGET_SPEED_MAX)
	{
		*speed_val = -TARGET_SPEED_MAX;
	}	
}

/*上位机目标值获取与设置*/
void SetTargetMaxSpeed(int speed)
{
	TARGET_SPEED_MAX = (float)speed;
}
int GetTargetMaxSpeed(void)
{
	return (int)TARGET_SPEED_MAX;
}


extern __IO int16_t EncoderOverflowCnt;

int temp=0;
//周期定时器的回调函数
void AutoReloadCallback()
{
	static uint32_t location_timer = 0;    // 位置环周期
	
	static __IO int encoderNow = 0;    /*当前时刻总计数值*/
    static __IO int encoderLast = 0;   /*上一时刻总计数值*/
	int encoderDelta = 0; /*当前时刻与上一时刻编码器的变化量*/
	float actual_speed = 0;  /*实际测得速度*/
	int actual_speed_int = 0;
	
	int res_pwm = 0;/*PID计算得到的PWM值*/
	static int i=0;
	
	/*【1】读取编码器的值*/
	encoderNow = read_encoder() + EncoderOverflowCnt*ENCODER_TIM_PERIOD;/*获取当前的累计值*/
	encoderDelta = encoderNow - encoderLast; /*得到变化值*/
	encoderLast = encoderNow;/*更新上次的累计值*/
	
	/*【2】位置PID运算，得到PWM控制值*/
	if ((location_timer++ % 2) == 0)
	{
		float control_val = 0;   /*当前控制值*/
		
		/*位置PID计算*/
		control_val = location_pid_realize(&pid_location, encoderNow);  
		
        /*目标速度值限制*/
		speed_val_protect(&control_val);

		/*设定速度PID的目标值*/
		set_pid_target(&pid_speed, control_val);    

		#if defined(PID_ASSISTANT_EN)
		if ((location_timer % 16) == 8)
		{
			temp = (int)control_val;
			set_computer_value(SEND_TARGET_CMD, CURVES_CH2, &temp, 1);     // 给通道 2 发送目标值
		}
		#endif
	}
	  
	/* 转速(1秒钟转多少圈)=单位时间内的计数值/总分辨率*时间系数, 再乘60变为1分钟转多少圈 */
    actual_speed = (float)encoderDelta / TOTAL_RESOLUTION * 10 * 60;
    
	/*【3】速度PID运算，得到PWM控制值*/
	actual_speed_int = actual_speed;
	res_pwm = pwm_val_protect((int)speed_pid_realize(&pid_speed, actual_speed));
	
	/*【4】PWM控制电机*/
	set_motor_rotate(res_pwm);
	
	/*【5】数据上传到上位机显示*/
#if (PID_ASSISTANT_EN)
	i++; 
	if(i%12 == 5)
	{
		set_computer_value(SEND_FACT_CMD, CURVES_CH1, &encoderNow, 1);   /*给通道1发送实际的电机【位置】值*/
	}
//	else if(i%12 == 10)
//	{
//		set_computer_value(SEND_FACT_CMD, CURVES_CH2, &actual_speed_int, 1); /*给通道2发送实际的电机【速度】值*/
//	}
	
#else
	i++;
	if(i==100)
	{
		i=0;
		printf("实际值速度：%d. 实际位置值：%d, 目标位置值：%.0f\n", actual_speed, encoderNow, get_pid_target(&pid_location)); 
		
	}
#endif
}



//定时器7中断服务函数
void TIM7_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM7,TIM_IT_Update)==SET) //溢出中断
	{
		AutoReloadCallback();
		TIM_ClearITPendingBit(TIM7,TIM_IT_Update);  //清除中断标志位
	}
}
