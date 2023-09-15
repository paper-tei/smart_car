#ifndef __DEBUG_USART_H
#define	__DEBUG_USART_H

#include "stm32f4xx.h"
#include <stdio.h>
#include "bsp_led.h"


//引脚定义
/*******************************************************/
#define DEBUG_USART                             USART1
#define DEBUG_USART_CLK                         RCC_APB2Periph_USART1
#define DEBUG_USART_BAUDRATE                    115200  //串口波特率

#define DEBUG_USART_RX_GPIO_PORT                GPIOA
#define DEBUG_USART_RX_GPIO_CLK                 RCC_AHB1Periph_GPIOA
#define DEBUG_USART_RX_PIN                      GPIO_Pin_10
#define DEBUG_USART_RX_AF                       GPIO_AF_USART1
#define DEBUG_USART_RX_SOURCE                   GPIO_PinSource10

#define DEBUG_USART_TX_GPIO_PORT                GPIOA
#define DEBUG_USART_TX_GPIO_CLK                 RCC_AHB1Periph_GPIOA
#define DEBUG_USART_TX_PIN                      GPIO_Pin_9
#define DEBUG_USART_TX_AF                       GPIO_AF_USART1
#define DEBUG_USART_TX_SOURCE                   GPIO_PinSource9

#define DEBUG_USART_IRQHandler                  USART1_IRQHandler
#define DEBUG_USART_IRQ                 				USART1_IRQn

//串口接收缓冲数组大小
#define UART_RX_BUFFER_SIZE 256 
/************************************************************/
//数据包结构体
/************************************************************/
#define PACK_SART_VALUE	0x59485A53
//length=0x0b+4*参数

typedef struct 
{
	float 	 KP;
	float		KI;
	float 	KD;
}PIDC;

typedef __packed struct 
{
	uint32_t 	Pack_Start;//4
	uint8_t	 	Pack_Channel;//1
	uint32_t	Pack_Length;//8+4
	uint8_t		Pack_Command;//8+4+1
//	uint8_t		Pack_Sum;//8+4+1+1
}Packed;//大小是14？

typedef union 
{
	float flo;
	unsigned char Rec_data[4];
}Union_Data;

/************************************************************/

/************************************************************/
//命令名称->上位机	HOST
/************************************************************/
#define			SET_HOST_CHANNAL_TARGET					0x01
#define 		SET_HOST_CHANNAL_REALY					0x02
#define 		SET_HOST_PID_PARAMETER					0x03
#define 		SET_HOST_START									0x04
#define 		SET_HOST_STOP										0x05
#define			SET_HOST_PERIOD									0x06
/************************************************************/
//通道变量
/************************************************************/
#define	 HOST_CHANNAL_1  0x01
#define	 HOST_CHANNAL_2  0x02
#define	 HOST_CHANNAL_3  0x03
#define	 HOST_CHANNAL_4  0x04
#define	 HOST_CHANNAL_5  0x05	 
/************************************************************/
//命令名称->下位机	SLAVE  
//这里可以改成enum类型，之前调试的时候改掉了，有兴趣的小伙伴可以该来试试
//不知道怎么写的百度一下~~(我还是给留了个模板，hh)
/************************************************************/
//typedef enum{
#define			SET_SLAVE_PID			  0x10
#define 		SET_SLAVE_TARGET	  0x11
#define 		SET_SLAVE_START		  0x12
#define 		SET_SLAVE_STOP		  0x13
#define 		SET_SLAVE_RESET		  0x14
#define 		SET_SLAVE_PERIOD	  0x15
//}SET_SLAVE_COMMAND;
/************************************************************/
/* 索引值宏定义 */
#define HEAD_INDEX_VAL       0x3u     // 包头索引值（4字节）
#define CHX_INDEX_VAL        0x4u     // 通道索引值（1字节）
#define LEN_INDEX_VAL        0x8u     // 包长索引值（4字节）
#define CMD_INDEX_VAL        0x9u     // 命令索引值（1字节）
#define P_INDEX_VAL				 	 0xAu			// 	P参数索引	(其实就是自己按着野火通信协议算出来的，咳咳)
#define I_INDEX_VAL					 0xEu			// 	I参数索引
#define D_INDEX_VAL					 0x12u		//	D参数索引


#define COMPOUND_32BIT(data)        (((*(data-0) << 24) & 0xFF000000) |\
                                     ((*(data-1) << 16) & 0x00FF0000) |\
                                     ((*(data-2) <<  8) & 0x0000FF00) |\
                                     ((*(data-3) <<  0) & 0x000000FF))      // 合成为一个字

#define EXCHANGE_H_L_BIT(data)      ((((data) << 24) & 0xFF000000) |\
                                     (((data) <<  8) & 0x00FF0000) |\
                                     (((data) >>  8) & 0x0000FF00) |\
                                     (((data) >> 24) & 0x000000FF))     // 交换高低字节

/************************************************************/
void Debug_USART_Config(void);
void Usart_SendByte( USART_TypeDef * pUSARTx, uint8_t ch);
void Usart_SendString( USART_TypeDef * pUSARTx, char *str);

void Usart_SendHalfWord( USART_TypeDef * pUSARTx, uint16_t ch);

void Send_SlaveTOHost(uint8_t channal,uint8_t command,uint8_t parameter_Number,void *data);
int8_t Receive_HostTOSlave(uint8_t *counter);

uint8_t Calculate_CheckSum(uint8_t Sum_init, uint8_t *ptr, uint8_t len);


#endif /* __USART1_H */
