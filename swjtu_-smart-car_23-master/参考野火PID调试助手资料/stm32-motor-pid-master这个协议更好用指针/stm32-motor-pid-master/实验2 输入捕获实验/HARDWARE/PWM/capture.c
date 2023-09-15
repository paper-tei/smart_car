#include "capture.h"

/**
* @brief TIM5 通道1输入捕获配置 
* @param arr：自动重装值(TIM2,TIM5是32位的!!)
*        psc：时钟预分频数
*/
void TIM5_CH1_Cap_Init(u32 arr,u16 psc)
{
	GPIO_InitTypeDef GPIO_InitStructure;            /*GPIO 结构体*/
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure; /*时基 结构体*/
	TIM_ICInitTypeDef  TIM5_ICInitStructure;        /*输入通道 结构体*/
	
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5,ENABLE);  	//TIM5时钟使能    
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE); 	//使能PORTA时钟	
	
	/*输入信号的GPIO初始化*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;           //GPIOA0
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;        /*复用功能*/
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//速度100MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;      //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;      /*下拉*/
	GPIO_Init(GPIOA,&GPIO_InitStructure);               //初始化PA0

	GPIO_PinAFConfig(GPIOA,GPIO_PinSource0,GPIO_AF_TIM5); //PA0复用位定时器5
  
	/*时基初始化*/
	TIM_TimeBaseStructure.TIM_Period=arr;     /* 自动重装载值 */
	TIM_TimeBaseStructure.TIM_Prescaler=psc;  /* 定时器分频 */
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; //向上计数模式
	TIM_TimeBaseInit(TIM5,&TIM_TimeBaseStructure);
	

	/*输入通道初始化，初始化TIM5输入捕获参数*/
	TIM5_ICInitStructure.TIM_Channel = TIM_Channel_1;                //CC1S=01 选择输入端 IC1映射到TI1上
	TIM5_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;	 /* 上升沿捕获 */
	TIM5_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; //映射到TI1上
	TIM5_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	         //配置输入分频,不分频 
	TIM5_ICInitStructure.TIM_ICFilter = 0x00;                        //IC1F=0000 配置输入滤波器 不滤波
	TIM_ICInit(TIM5, &TIM5_ICInitStructure);

	TIM_ITConfig(TIM5,TIM_IT_Update|TIM_IT_CC1,ENABLE);//允许更新中断 ,允许CC1IE捕获中断	

	TIM_Cmd(TIM5,ENABLE ); 	//使能定时器5

    /*定时器中断配置*/
	NVIC_InitStructure.NVIC_IRQChannel = TIM5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2; //抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =0;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);                     	//根据指定的参数初始化NVIC寄存器
}


/* 捕获状态 TIM5CH1_CAPTURE_STA
*  [7]: 0,没有成功的捕获  
*       1,成功捕获到一次
*  [6]: 0,还没捕获到低电平
*       1,已经捕获到低电平了
*  [5:0]: 00000~11111,捕获低电平后溢出的次数(对于32位定时器来说,1us计数器加1,溢出时间:4294秒)
*/
u8  TIM5CH1_CAPTURE_STA=0;	//输入捕获状态（当中一个自制的寄存器使用，初始为0）	

u32	TIM5CH1_CAPTURE_VAL;	//输入捕获值(TIM2/TIM5是32位)

/**
* @brief 定时器5中断服务程序
*/
void TIM5_IRQHandler(void)
{ 		
 	if((TIM5CH1_CAPTURE_STA&0X80)==0)//还未成功捕获	(1000 0000)
	{
		/*定时器溢出中断*/
		if(TIM_GetITStatus(TIM5, TIM_IT_Update) != RESET)
		{	     
			if(TIM5CH1_CAPTURE_STA&0X40)/* 之前标记了开始信号(0100 0000) */
			{
				if((TIM5CH1_CAPTURE_STA&0X3F)==0X3F) /* 高电平太长了,计数溢出了 (0011 1111) */
				{
					TIM5CH1_CAPTURE_STA|=0X80;		 /* (强制)标记成功捕获了一次 (1000 0000) */
					TIM5CH1_CAPTURE_VAL=0XFFFFFFFF;  /* 因为溢出次数N不能再加了，就将当前的捕获值设置为32位的最大值，等效Nmax+1*/
				}
				else /* 正常情况是不会溢出，最终得出正确的高电平时间 */
				{
					TIM5CH1_CAPTURE_STA++; /* 累计定时器溢出次数N */
				}
			}
			else
			{
				/* 还没有捕获到信号时，定时器溢出后什么也不做，自己清零继续计数即可 */
			}
		}
		
		/*捕获1发生捕获事件*/
		if(TIM_GetITStatus(TIM5, TIM_IT_CC1) != RESET)
		{	
			/*捕获到一个下降沿(结束信号)*/
			if(TIM5CH1_CAPTURE_STA&0X40) /* 之前标记了开始信号(0100 0000) */		 		
			{	  			
				TIM5CH1_CAPTURE_STA|=0X80;		                   /* 标记成功捕获到一次高电平脉宽 (1000 0000) */
				TIM5CH1_CAPTURE_VAL=TIM_GetCapture1(TIM5);         /* 获取当前的捕获值 */
	 			TIM_OC1PolarityConfig(TIM5,TIM_ICPolarity_Rising); /* CC1P=0 重新设置为上升沿捕获，用于下次捕捉信号 */
			}      
			/*还未开始,第一次捕获 上升沿(起始信号) */
			else  								
			{
				TIM5CH1_CAPTURE_STA=0;			/* 清空 捕获状态寄存器 */
				TIM5CH1_CAPTURE_VAL=0;          /* 清空 捕获值 */
				TIM5CH1_CAPTURE_STA|=0X40;		/* 标记捕获到了上升沿 (0100 0000) */
				
				TIM_Cmd(TIM5,DISABLE ); 	    /* 关闭定时器5 */
	 			TIM_SetCounter(TIM5,0);         /* 清空CNT，重新从0开始计数 */
	 			TIM_OC1PolarityConfig(TIM5,TIM_ICPolarity_Falling);	/* CC1P=1 设置为下降沿捕获 */
				TIM_Cmd(TIM5,ENABLE ); 	        /* 使能定时器5 */
			}		    
		}			     	    					   
 	}
	TIM_ClearITPendingBit(TIM5, TIM_IT_CC1|TIM_IT_Update); //清除中断标志位
}
