#include "motor.h"
#include "pwm.h"

#define RCC_AHB1Periph_MOTOR1 RCC_AHB1Periph_GPIOA
#define GPIO_MOTOR1 GPIOA

#define GPIO_Pin_MOTOR1_IN1 GPIO_Pin_4
#define GPIO_Pin_MOTOR1_IN2 GPIO_Pin_5

 void MOTOT_Init()
{		 					 
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_MOTOR1, ENABLE);//使能GPIOA时钟

	//GPIOA6,A7初始化设置
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_MOTOR1_IN1 | GPIO_Pin_MOTOR1_IN2;//LEDa和LEDb对应IO口
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIO_MOTOR1, &GPIO_InitStructure);//初始化GPIO

	GPIO_ResetBits(GPIO_MOTOR1,GPIO_Pin_MOTOR1_IN1 | GPIO_Pin_MOTOR1_IN2);//设置高，灯灭
}  

static void set_clockwise_rotate(void)
{
	GPIO_SetBits(GPIO_MOTOR1,GPIO_Pin_MOTOR1_IN1);
	GPIO_ResetBits(GPIO_MOTOR1,GPIO_Pin_MOTOR1_IN2);
}

static void set_anticlockwise_rotate(void)
{
	GPIO_ResetBits(GPIO_MOTOR1,GPIO_Pin_MOTOR1_IN1);
	GPIO_SetBits(GPIO_MOTOR1,GPIO_Pin_MOTOR1_IN2);
}

static void set_stop_rotate(void)
{
	GPIO_ResetBits(GPIO_MOTOR1,GPIO_Pin_MOTOR1_IN1);
	GPIO_ResetBits(GPIO_MOTOR1,GPIO_Pin_MOTOR1_IN2);
}


void set_motor_rotate(int pwm)
{
	if(pwm > 0)
	{
		set_pwm(pwm);
		set_clockwise_rotate();
	}
	else if(pwm < 0)
	{
		set_pwm(-pwm);
		set_anticlockwise_rotate();
	}
	else
	{
		set_pwm(0);
		set_stop_rotate();
	}
}
