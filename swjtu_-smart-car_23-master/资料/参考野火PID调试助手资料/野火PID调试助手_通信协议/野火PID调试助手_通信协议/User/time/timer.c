#include "timer.h"

void Basic_Time_Init(void)
{	
	TIM_TimeBaseInitTypeDef	TIM_TimeBaseInitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitStruct.NVIC_IRQChannel=TIM2_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=1;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority=1;
	NVIC_Init(&NVIC_InitStruct);
	
	TIM_TimeBaseInitStruct.TIM_ClockDivision=0;
	TIM_TimeBaseInitStruct.TIM_CounterMode=TIM_CounterMode_Up;
	TIM_TimeBaseInitStruct.TIM_Period			=1000-1;
	TIM_TimeBaseInitStruct.TIM_Prescaler	=72-1;//Ô¤·ÖÅäÏµÊý
	
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseInitStruct);
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);
	TIM_Cmd(TIM2,ENABLE);
}


