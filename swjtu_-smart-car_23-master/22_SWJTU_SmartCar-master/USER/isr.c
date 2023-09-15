
 
/*********************************************************************************************************************
 * COPYRIGHT NOTICE
 * Copyright (c) 2020,逐飞科技
 * All rights reserved.
 * 技术讨论QQ群：三群：824575535
 *
 * 以下所有内容版权均属逐飞科技所有，未经允许不得用于商业用途，
 * 欢迎各位使用并传播本程序，修改内容时必须保留逐飞科技的版权声明。
 *
 * @file       		isr
 * @company	   		成都逐飞科技有限公司
 * @author     		逐飞科技(QQ3184284598)
 * @version    		查看doc内version文件 版本说明
 * @Software 		ADS v1.2.2
 * @Target core		TC364DP
 * @Taobao   		https://seekfree.taobao.com/
 * @date       		2020-11-23
 ********************************************************************************************************************/


#include "isr_config.h"
#include "isr.h"


//PIT中断函数  示例
uint16 time;
IFX_INTERRUPT(cc60_pit_ch0_isr, CCU6_0_CH0_INT_VECTAB_NUM, CCU6_0_CH0_ISR_PRIORITY)
{
    enableInterrupts();//开启中断嵌套
    PIT_CLEAR_FLAG(CCU6_0, PIT_CH0);
    ///----------------------------
        static uint8 t_2ms = 0;
        static uint8 t_6ms = 0;
        static uint8 t_10ms = 0;
        static uint8 t_20ms = 0;
        static uint16 t_1s = 0;
/*************************中断执行程序********2ms进一次中断***************/
      t_2ms++;
      t_6ms++;
      t_10ms++;
      t_20ms++;
//2ms控制周期
	if(t_2ms == 1)
	{
	t_2ms = 0;
	Flag.T_2ms=1;
	}
//6ms控制周期
	if(t_6ms == 3)
	{
	t_6ms = 0;
	Flag.T_6ms=1;
	}
//10ms速度控制周期
	if (t_10ms == 5)
	{
		t_10ms = 0;
		Flag.T_10ms=1;
	}

//20ms转向控制周期
	if (t_20ms == 10)
	{
		t_20ms = 0;
		Flag.T_20ms=1;
	}
//1s控制周期
	if(ring_ok_flag == 1||san_cha_ok_flag==1)
	{
		t_1s++;
	}
	if (t_1s == 500)
	{
		t_1s = 0;
		Flag.T_1s=1;
	}
//**********************************************************
	Fuse_result();
}

IFX_INTERRUPT(cc60_pit_ch1_isr, CCU6_0_CH1_INT_VECTAB_NUM, CCU6_0_CH1_ISR_PRIORITY)
{
	enableInterrupts();//开启中断嵌套
	PIT_CLEAR_FLAG(CCU6_0, PIT_CH1);

}

IFX_INTERRUPT(cc61_pit_ch0_isr, CCU6_1_CH0_INT_VECTAB_NUM, CCU6_1_CH0_ISR_PRIORITY)
{
	enableInterrupts();//开启中断嵌套
	PIT_CLEAR_FLAG(CCU6_1, PIT_CH0);

}

IFX_INTERRUPT(cc61_pit_ch1_isr, CCU6_1_CH1_INT_VECTAB_NUM, CCU6_1_CH1_ISR_PRIORITY)
{
	enableInterrupts();//开启中断嵌套
	PIT_CLEAR_FLAG(CCU6_1, PIT_CH1);

}




IFX_INTERRUPT(eru_ch0_ch4_isr, ERU_CH0_CH4_INT_VECTAB_NUM, ERU_CH0_CH4_INT_PRIO)
{
	enableInterrupts();//开启中断嵌套
	if(GET_GPIO_FLAG(ERU_CH0_REQ4_P10_7))//通道0中断
	{
		CLEAR_GPIO_FLAG(ERU_CH0_REQ4_P10_7);
	}

	if(GET_GPIO_FLAG(ERU_CH4_REQ13_P15_5))//通道4中断
	{
		CLEAR_GPIO_FLAG(ERU_CH4_REQ13_P15_5);
	}
}

IFX_INTERRUPT(eru_ch1_ch5_isr, ERU_CH1_CH5_INT_VECTAB_NUM, ERU_CH1_CH5_INT_PRIO)
{
	enableInterrupts();//开启中断嵌套
	if(GET_GPIO_FLAG(ERU_CH1_REQ5_P10_8))//通道1中断
	{
		CLEAR_GPIO_FLAG(ERU_CH1_REQ5_P10_8);
	}

	if(GET_GPIO_FLAG(ERU_CH5_REQ1_P15_8))//通道5中断
	{
		CLEAR_GPIO_FLAG(ERU_CH5_REQ1_P15_8);
	}
}

//由于摄像头pclk引脚默认占用了 2通道，用于触发DMA，因此这里不再定义中断函数
//IFX_INTERRUPT(eru_ch2_ch6_isr, 0, ERU_CH2_CH6_INT_PRIO)
//{
//	enableInterrupts();//开启中断嵌套
//	if(GET_GPIO_FLAG(ERU_CH2_REQ7_P00_4))//通道2中断
//	{
//		CLEAR_GPIO_FLAG(ERU_CH2_REQ7_P00_4);
//
//	}
//	if(GET_GPIO_FLAG(ERU_CH6_REQ9_P20_0))//通道6中断
//	{
//		CLEAR_GPIO_FLAG(ERU_CH6_REQ9_P20_0);
//
//	}
//}



IFX_INTERRUPT(eru_ch3_ch7_isr, ERU_CH3_CH7_INT_VECTAB_NUM, ERU_CH3_CH7_INT_PRIO)
{
	enableInterrupts();//开启中断嵌套
	if(GET_GPIO_FLAG(ERU_CH3_REQ6_P02_0))//通道3中断
	{
		CLEAR_GPIO_FLAG(ERU_CH3_REQ6_P02_0);
		if		(CAMERA_GRAYSCALE == camera_type)	mt9v03x_vsync();

	}
	if(GET_GPIO_FLAG(ERU_CH7_REQ16_P15_1))//通道7中断
	{
		CLEAR_GPIO_FLAG(ERU_CH7_REQ16_P15_1);

	}
}



IFX_INTERRUPT(dma_ch5_isr, ERU_DMA_INT_VECTAB_NUM, ERU_DMA_INT_PRIO)
{
	enableInterrupts();//开启中断嵌套

	if		(CAMERA_GRAYSCALE == camera_type)	mt9v03x_dma();
}


//串口中断函数  示例
IFX_INTERRUPT(uart0_tx_isr, UART0_INT_VECTAB_NUM, UART0_TX_INT_PRIO)
{
	enableInterrupts();//开启中断嵌套
    IfxAsclin_Asc_isrTransmit(&uart0_handle);
}
IFX_INTERRUPT(uart0_rx_isr, UART0_INT_VECTAB_NUM, UART0_RX_INT_PRIO)
{
	enableInterrupts();//开启中断嵌套
    IfxAsclin_Asc_isrReceive(&uart0_handle);
}
IFX_INTERRUPT(uart0_er_isr, UART0_INT_VECTAB_NUM, UART0_ER_INT_PRIO)
{
	enableInterrupts();//开启中断嵌套
    IfxAsclin_Asc_isrError(&uart0_handle);
}

//串口1默认连接到摄像头配置串口
IFX_INTERRUPT(uart1_tx_isr, UART1_INT_VECTAB_NUM, UART1_TX_INT_PRIO)
{
	enableInterrupts();//开启中断嵌套
    IfxAsclin_Asc_isrTransmit(&uart1_handle);
}
IFX_INTERRUPT(uart1_rx_isr, UART1_INT_VECTAB_NUM, UART1_RX_INT_PRIO)
{
	enableInterrupts();//开启中断嵌套
    IfxAsclin_Asc_isrReceive(&uart1_handle);
    if		(CAMERA_GRAYSCALE == camera_type)	mt9v03x_uart_callback();
}
IFX_INTERRUPT(uart1_er_isr, UART1_INT_VECTAB_NUM, UART1_ER_INT_PRIO)
{
	enableInterrupts();//开启中断嵌套
    IfxAsclin_Asc_isrError(&uart1_handle);
}


//串口2默认连接到无线转串口模块
IFX_INTERRUPT(uart2_tx_isr, UART2_INT_VECTAB_NUM, UART2_TX_INT_PRIO)
{
	enableInterrupts();//开启中断嵌套
    IfxAsclin_Asc_isrTransmit(&uart2_handle);
}
IFX_INTERRUPT(uart2_rx_isr, UART2_INT_VECTAB_NUM, UART2_RX_INT_PRIO)
{
	enableInterrupts();//开启中断嵌套
	IfxAsclin_Asc_isrReceive(&uart2_handle);
	switch(wireless_type)
	{
		case WIRELESS_SI24R1:
		{
			wireless_uart_callback();
		}break;

		case WIRELESS_CH9141:
		{
			bluetooth_ch9141_uart_callback();
		}break;
		default:break;
	}
}
IFX_INTERRUPT(uart2_er_isr, UART2_INT_VECTAB_NUM, UART2_ER_INT_PRIO)
{
	enableInterrupts();//开启中断嵌套
    IfxAsclin_Asc_isrError(&uart2_handle);
}



IFX_INTERRUPT(uart3_tx_isr, UART3_INT_VECTAB_NUM, UART3_TX_INT_PRIO)
{
	enableInterrupts();//开启中断嵌套
    IfxAsclin_Asc_isrTransmit(&uart3_handle);
}
IFX_INTERRUPT(uart3_rx_isr, UART3_INT_VECTAB_NUM, UART3_RX_INT_PRIO)
{
	enableInterrupts();//开启中断嵌套
    IfxAsclin_Asc_isrReceive(&uart3_handle);

	Distance_measurement();
		
}
IFX_INTERRUPT(uart3_er_isr, UART3_INT_VECTAB_NUM, UART3_ER_INT_PRIO)
{
	enableInterrupts();//开启中断嵌套
    IfxAsclin_Asc_isrError(&uart3_handle);
}
