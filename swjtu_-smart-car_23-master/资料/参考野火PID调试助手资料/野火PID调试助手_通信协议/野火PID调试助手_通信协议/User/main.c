/**
******************************************************************************
//		野火PID助手通信协议
//具体说明请看  ->  必读说明
//有个很奇怪的地方是 我把周期设置为0，串口助手自己的变成9了，他那个周期老是会在我发送周期的
//基础上+9，不知道为什么。。。有知道的老哥请务必告诉我。写这个写的头大，没再去改进，就这样吧。
******************************************************************************
*/
#include "stm32f4xx.h"
#include "./usart/bsp_debug_usart.h"
#include "timer.h"
#include "bsp_led.h" 
/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
uint32_t time=0;
int now_dat=111;
uint8_t	Target=100;

PIDC Pid_Test=
{
	.KP=10,
	.KI=11,
	.KD=12
};

int main(void)
{		
  /*初始化USART 配置模式为 115200 8-N-1，中断接收*/
  Debug_USART_Config();
	Basic_Time_Init();
	LED_GPIO_Config();
	/* 发送一个字符串 */

	Send_SlaveTOHost(HOST_CHANNAL_1,SET_HOST_CHANNAL_TARGET,1,&Target);//主机目标值
	Send_SlaveTOHost(HOST_CHANNAL_1,SET_HOST_CHANNAL_REALY,1,&now_dat);//主机实际值
	Send_SlaveTOHost(HOST_CHANNAL_1,SET_HOST_PID_PARAMETER,4,&Pid_Test);//主机PID值
	Send_SlaveTOHost(HOST_CHANNAL_1,SET_HOST_PERIOD,1,&time);//主机周期
	Send_SlaveTOHost(HOST_CHANNAL_1,SET_HOST_START,0,0);//主机开始传输
	
  while(1)
	{	
		if(time == 100)
		{
			static u8 flag = 0;
			if(flag == 0)
			{
				if(++now_dat >=130 ) 
				{
					flag = 1;
					LED_YELLOW;
				}					
			}
			else 
			{
				if(--now_dat <=90 ) 
				{
					flag = 0;
					LED_CYAN;
				}
			}			
				
		Send_SlaveTOHost(HOST_CHANNAL_1,SET_HOST_CHANNAL_REALY,1,&now_dat);	
		time = 0;
		}	
	}
	
}



/*********************************************END OF FILE**********************/

