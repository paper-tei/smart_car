#include "capture.h"
#include "sys.h"


/**
* @brief TIM4 通道1通道2 正交编码器
* @param none
*/
void TIMx_encoder_init(void)                      
{ 
	GPIO_InitTypeDef GPIO_InitStruct;            /*GPIO*/
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStruct; /*时基*/
	TIM_ICInitTypeDef TIM_ICInitStruct;          /*输入通道*/
	NVIC_InitTypeDef NVIC_InitStructure;         /*中断*/
    
    /*GPIO初始化*/    
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE); /*使能GPIO时钟 AHB1*/                    
	GPIO_StructInit(&GPIO_InitStruct);        
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7; 
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;        /*复用功能*/
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;	 /*速度100MHz*/
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;   
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;        
	GPIO_Init(GPIOB, &GPIO_InitStruct); 
	
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource6,GPIO_AF_TIM4); 
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource7,GPIO_AF_TIM4); 

	/*时基初始化*/
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);   /*使能定时器时钟 APB1*/
	TIM_DeInit(TIM4);  
	TIM_TimeBaseStructInit(&TIM_TimeBaseStruct);    
	TIM_TimeBaseStruct.TIM_Prescaler = ENCODER_TIM_PSC;       /*预分频 */        
	TIM_TimeBaseStruct.TIM_Period = ENCODER_TIM_PERIOD;       /*周期(重装载值)*/
	TIM_TimeBaseStruct.TIM_ClockDivision = TIM_CKD_DIV1;      
	TIM_TimeBaseStruct.TIM_CounterMode = TIM_CounterMode_Up;  /*连续向上计数模式*/  
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStruct); 

	/*编码器模式配置：同时捕获通道1与通道2(即4倍频)，极性均为Rising*/
	TIM_EncoderInterfaceConfig(TIM4, TIM_EncoderMode_TI12,TIM_ICPolarity_Rising, TIM_ICPolarity_Rising); 
	TIM_ICStructInit(&TIM_ICInitStruct);        
	TIM_ICInitStruct.TIM_ICFilter = 0;   /*输入通道的滤波参数*/
	TIM_ICInit(TIM4, &TIM_ICInitStruct); /*输入通道初始化*/
	TIM_SetCounter(TIM4, CNT_INIT);      /*CNT设初值*/
	TIM_ClearFlag(TIM4,TIM_IT_Update);   /*中断标志清0*/
	TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE); /*中断使能*/
	TIM_Cmd(TIM4,ENABLE);                /*使能CR寄存器*/
	
	/*中断配置*/
	NVIC_InitStructure.NVIC_IRQChannel=TIM4_IRQn; //定时器4中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x01; //抢占优先级1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x01; //子优先级1
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
} 

// 读取定时器计数值
#if 0
int read_encoder(void)
{
	int encoderNum = 0;
	encoderNum = (int)((int16_t)(TIM4->CNT)); /*CNT为uint32, 转为int16*/
	//TIM_SetCounter(TIM4, CNT_INIT);/*CNT设初值*/

	return encoderNum;
}
#else
uint32_t read_encoder(void)
{
	uint32_t encoderNum = 0;
	encoderNum = (TIM4->CNT); /*CNT为uint32, 转为int16*/
	//TIM_SetCounter(TIM4, CNT_INIT);/*CNT设初值*/

	return encoderNum;
}
#endif

/* 定时器溢出次数 */
__IO int16_t EncoderOverflowCnt = 0;

//定时器4中断服务函数
void TIM4_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM4,TIM_IT_Update)==SET) //溢出中断
	{
		if((TIM4->CR1 & TIM_CounterMode_Down) != TIM_CounterMode_Down)
		{
			EncoderOverflowCnt++;/*编码器计数值[向上]溢出*/
		}
		else
		{
			EncoderOverflowCnt--;/*编码器计数值[向下]溢出*/
		}
	}
	TIM_ClearITPendingBit(TIM4,TIM_IT_Update);  //清除中断标志位
}


