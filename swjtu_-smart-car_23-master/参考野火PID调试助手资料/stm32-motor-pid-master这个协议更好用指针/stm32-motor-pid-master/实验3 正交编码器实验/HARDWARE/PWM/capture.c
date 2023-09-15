#include "capture.h"
#include "sys.h"
#include "usart.h"

#define ENCODER_TIM_PSC  0          /*计数器分频*/
#define ENCODER_TIM_PERIOD  65535   /*计数器最大值*/
#define CNT_INIT 0                  /*计数器初值*/

#define ENCODER_RESOLUTION 11    /*编码器一圈的物理脉冲数*/
#define ENCODER_MULTIPLE 4       /*编码器倍频，通过定时器的编码器模式设置*/
#define MOTOR_REDUCTION_RATIO 34 /*电机的减速比*/
/*电机转一圈总的脉冲数(定时器能读到的脉冲数) = 编码器物理脉冲数*编码器倍频*电机减速比 */
#define TOTAL_RESOLUTION ( ENCODER_RESOLUTION*ENCODER_MULTIPLE*MOTOR_REDUCTION_RATIO ) 

/**
* @brief TIM4 通道1通道2 正交编码器
* @param none
*/
void TIM4_ENCODER_Init(void)                      
{ 
	GPIO_InitTypeDef GPIO_InitStruct;            /*GPIO*/
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStruct; /*时基*/
	TIM_ICInitTypeDef TIM_ICInitStruct;          /*输入通道*/
    
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
} 

// 读取定时器计数值
static int read_encoder(void)
{
	int encoderNum = 0;
	encoderNum = (int)((int16_t)(TIM4->CNT)); /*CNT为uint32, 转为int16*/
	TIM_SetCounter(TIM4, CNT_INIT);/*CNT设初值*/

	return encoderNum;
}

//计算电机转速（被另一个定时器100ms调用1次）
void calc_motor_rotate_speed()
{
	int encoderNum = 0;
	float rotateSpeed = 0;
	
	/*读取编码器的值，正负代表旋转方向*/
	encoderNum = read_encoder();
	/* 转速(1秒钟转多少圈)=单位时间内的计数值/总分辨率*时间系数 */
	rotateSpeed = (float)encoderNum/TOTAL_RESOLUTION*10;
	
	printf("encoder: %d\t speed:%.2f rps\r\n",encoderNum,rotateSpeed);

}

