/**
  ******************************************************************************
  * @file    bsp_debug_usart.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   重定向c库printf函数到usart端口，中断接收模式
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火  STM32 F429 开发板  
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 
  
#include "./usart/bsp_debug_usart.h"

/****************************使用的一下变量***********************************/
//串口接收数组
unsigned char UART_RxBuffer[UART_RX_BUFFER_SIZE];
//串口接收数组指针
unsigned char UART_RxPtr;

PIDC Pid_Test_C;
/* 命令接收完成 */

uint8_t receive_cmd = 0,recevie_data_length=11;

/***************************************************************/
 /**
  * @brief  配置嵌套向量中断控制器NVIC
  * @param  无
  * @retval 无
  */
static void NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  
  /* 嵌套向量中断控制器组选择 */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
  
  /* 配置USART为中断源 */
  NVIC_InitStructure.NVIC_IRQChannel = DEBUG_USART_IRQ;
  /* 抢断优先级为1 */
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  /* 子优先级为1 */
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  /* 使能中断 */
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  /* 初始化配置NVIC */
  NVIC_Init(&NVIC_InitStructure);
}


 /**
  * @brief  DEBUG_USART GPIO 配置,工作模式配置。115200 8-N-1 ，中断接收模式
  * @param  无
  * @retval 无
  */
void Debug_USART_Config(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  USART_InitTypeDef USART_InitStructure;
		
  RCC_AHB1PeriphClockCmd(DEBUG_USART_RX_GPIO_CLK|DEBUG_USART_TX_GPIO_CLK,ENABLE);

  /* 使能 USART 时钟 */
  RCC_APB2PeriphClockCmd(DEBUG_USART_CLK, ENABLE);
  
  /* GPIO初始化 */
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;  
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  
  /* 配置Tx引脚为复用功能  */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Pin = DEBUG_USART_TX_PIN  ;  
  GPIO_Init(DEBUG_USART_TX_GPIO_PORT, &GPIO_InitStructure);

  /* 配置Rx引脚为复用功能 */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Pin = DEBUG_USART_RX_PIN;
  GPIO_Init(DEBUG_USART_RX_GPIO_PORT, &GPIO_InitStructure);
  
 /* 连接 PXx 到 USARTx_Tx*/
  GPIO_PinAFConfig(DEBUG_USART_RX_GPIO_PORT,DEBUG_USART_RX_SOURCE,DEBUG_USART_RX_AF);

  /*  连接 PXx 到 USARTx__Rx*/
  GPIO_PinAFConfig(DEBUG_USART_TX_GPIO_PORT,DEBUG_USART_TX_SOURCE,DEBUG_USART_TX_AF);
  
  /* 配置串DEBUG_USART 模式 */
  /* 波特率设置：DEBUG_USART_BAUDRATE */
  USART_InitStructure.USART_BaudRate = DEBUG_USART_BAUDRATE;
  /* 字长(数据位+校验位)：8 */
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  /* 停止位：1个停止位 */
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  /* 校验位选择：不使用校验 */
  USART_InitStructure.USART_Parity = USART_Parity_No;
  /* 硬件流控制：不使用硬件流 */
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  /* USART模式控制：同时使能接收和发送 */
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  /* 完成USART初始化配置 */
  USART_Init(DEBUG_USART, &USART_InitStructure); 
	
  /* 嵌套向量中断控制器NVIC配置 */
	NVIC_Configuration();
  
	/* 使能串口接收中断 */
	USART_ITConfig(DEBUG_USART, USART_IT_RXNE,ENABLE);
	USART_ITConfig(DEBUG_USART,	USART_IT_IDLE,ENABLE);	
	
  /* 使能串口 */
  USART_Cmd(DEBUG_USART, ENABLE);
}

/*****************  发送一个字符 **********************/
void Usart_SendByte( USART_TypeDef * pUSARTx, uint8_t ch)
{
	/* 发送一个字节数据到USART */
	USART_SendData(pUSARTx,ch);
		
	/* 等待发送数据寄存器为空 */
	while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);	
}
/*****************  发送字符串 **********************/
void Usart_SendString( USART_TypeDef * pUSARTx, char *str)
{
	unsigned int k=0;
  do 
  {
      Usart_SendByte( pUSARTx, *(str + k) );
      k++;
  } while(*(str + k)!='\0');
  
  /* 等待发送完成 */
  while(USART_GetFlagStatus(pUSARTx,USART_FLAG_TC)==RESET)
  {}
}

/*****************  发送一个16位数 **********************/
void Usart_SendHalfWord( USART_TypeDef * pUSARTx, uint16_t ch)
{
	uint8_t temp_h, temp_l;
	
	/* 取出高八位 */
	temp_h = (ch&0XFF00)>>8;
	/* 取出低八位 */
	temp_l = ch&0XFF;
	
	/* 发送高八位 */
	USART_SendData(pUSARTx,temp_h);	
	while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);
	
	/* 发送低八位 */
	USART_SendData(pUSARTx,temp_l);	
	while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);	
}

///重定向c库函数printf到串口，重定向后可使用printf函数
int fputc(int ch, FILE *f)
{
		/* 发送一个字节数据到串口 */
		USART_SendData(DEBUG_USART, (uint8_t) ch);
		
		/* 等待发送完毕 */
		while (USART_GetFlagStatus(DEBUG_USART, USART_FLAG_TXE) == RESET);		
	
		return (ch);
}

///重定向c库函数scanf到串口，重写向后可使用scanf、getchar等函数
int fgetc(FILE *f)
{
		/* 等待串口输入数据 */
		while (USART_GetFlagStatus(DEBUG_USART, USART_FLAG_RXNE) == RESET);

		return (int)USART_ReceiveData(DEBUG_USART);
}
/*********************************************************下面是通信协议的函数，要移植的话直接从下面剪切就好**********************************************************************/
///****************************************************************************
/// @data     :         发送结构体函数                                                        
/// @input    :                                                                 
/// @output   :                                                                 
/// @brief    :                                                                 
///****************************************************************************
void Usart_SendStruct(USART_TypeDef * pUSARTx, uint8_t *str,uint8_t length)
{
	static uint8_t i=0;
	USART_ClearFlag(pUSARTx,USART_FLAG_TC); 
	for(i=0;i<length;i++)
	{
		USART_SendData(pUSARTx,str[i]);
		while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!= SET);
	}
}
///****************************************************************************
/// @data     :            校验和检验函数，野火写的，哈哈                                                     
/// @input    :                                                                 
/// @output   :                                                                 
/// @brief    :                                                                 
///****************************************************************************
uint8_t Calculate_CheckSum(uint8_t Sum_init, uint8_t *ptr, uint8_t len)
{
	uint8_t sum=Sum_init;
	
	while(len--)
	{
		sum += *ptr;
		ptr++;
	}
	
	return sum;
}
///****************************************************************************
/// @data     :       从机发送数据给主机使用的                                                          
/// @input    :                                                                 
/// @output   :                                                                 
/// @brief    :       float是四个字节                                                          
///****************************************************************************
void Send_SlaveTOHost(uint8_t channal,uint8_t command,uint8_t parameter_Number,void *data)
{
	static Packed Send_Pack;
	uint8_t pack_sum=0;
	
	parameter_Number *= 4;
	
	Send_Pack.Pack_Channel=channal;
	Send_Pack.Pack_Command=(uint8_t)command;
	Send_Pack.Pack_Length=(0x0B+parameter_Number);//包长度
	Send_Pack.Pack_Start=PACK_SART_VALUE;
//	Send_Pack.Pack_Sum=0;
	
	pack_sum=Calculate_CheckSum(0,(uint8_t *)&Send_Pack,sizeof(Send_Pack));//包头校验和
	//根据结构体计算规则可知，多计算了一个sum的大小
	pack_sum=Calculate_CheckSum(pack_sum,(uint8_t *)&data,parameter_Number);//参数校验和
	
	Usart_SendStruct(USART1,(uint8_t *)&Send_Pack,sizeof(Send_Pack));//发送数据头
	Usart_SendStruct(USART1,(uint8_t *)data,parameter_Number);//发送参数
	Usart_SendStruct(USART1,(uint8_t *)&pack_sum,sizeof(pack_sum));//发送校验和
	
}
///****************************************************************************
/// @data     :       从机解析主机数据使用                                                          
/// @input    :       四个U8数据 合并成一个float数据，主要是学习联合体的使用                                                          
/// @output   :                                                                 
/// @brief    :                                                                 
///****************************************************************************
void u8TOFloat(uint8_t Array_Target,float *PID)
{
	uint8_t j=0,cnt=0;
	Union_Data Receive_St;
	
	cnt=Array_Target;
	for(j=0;j<4;j++)
		Receive_St.Rec_data[j]=UART_RxBuffer[cnt++];
	//接收到的数据，小端在前，
	
		*PID=Receive_St.flo;
}
///****************************************************************************
/// @data     :     主机发送数据给从机，从机解析的时候使用的                                                            
/// @input    :                                                                 
/// @output   :                                                                 
/// @brief    :                                                                 
///****************************************************************************
int8_t Receive_HostTOSlave(uint8_t *counter)
{
	Packed Recevie_Packed;
	
	Recevie_Packed.Pack_Command	=UART_RxBuffer[CMD_INDEX_VAL];
	//接受到的数据，第九位就是命令，这是一种简化的写法
	Recevie_Packed.Pack_Length	=COMPOUND_32BIT(&UART_RxBuffer[LEN_INDEX_VAL]);//合成长度
	Recevie_Packed.Pack_Start		=COMPOUND_32BIT(&UART_RxBuffer[HEAD_INDEX_VAL]);//合成包头
	
//	*counter=Recevie_Packed.Pack_Length;
	recevie_data_length +=(Recevie_Packed.Pack_Length-0x0B);
	//求出数据总共占多少8位数组
				
		*counter=0;
		if(Recevie_Packed.Pack_Start == PACK_SART_VALUE)
		{
			if(Calculate_CheckSum(0,UART_RxBuffer,Recevie_Packed.Pack_Length -1) == UART_RxBuffer[Recevie_Packed.Pack_Length-1])
			{		
				/*解析出的命令在这里，根据不同的命令添加不同的处理函数就好。*/
				switch(Recevie_Packed.Pack_Command)
				{
					case SET_SLAVE_PID:
						LED_YELLOW;		
						u8TOFloat(P_INDEX_VAL,&Pid_Test_C.KP);
						u8TOFloat(I_INDEX_VAL,&Pid_Test_C.KI);
						u8TOFloat(D_INDEX_VAL,&Pid_Test_C.KD);
						Send_SlaveTOHost(HOST_CHANNAL_1,SET_HOST_PID_PARAMETER,4,&Pid_Test_C);
						break;
					
					case SET_SLAVE_TARGET:
						LED_BLUE;
						break;
					
					case SET_SLAVE_START:
						LED_GREEN
						break;
					
					case SET_SLAVE_STOP:
						LED_CYAN;
						break;
					
					case SET_SLAVE_PERIOD:
						LED_WHITE;
						break;
					
					case SET_SLAVE_RESET:
						LED_RED;
						break;				
				}		
			}		
		}
		
	return 1;
}
/*********************************************END OF FILE**********************/
