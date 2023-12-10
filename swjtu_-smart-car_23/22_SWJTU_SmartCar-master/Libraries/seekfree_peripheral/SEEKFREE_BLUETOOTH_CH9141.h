/*********************************************************************************************************************
 * COPYRIGHT NOTICE
 * Copyright (c) 2021,逐飞科技
 * All rights reserved.
 * 技术讨论QQ群：一群：179029047(已满)  二群：244861897
 *
 * 以下所有内容版权均属逐飞科技所有，未经允许不得用于商业用途，
 * 欢迎各位使用并传播本程序，修改内容时必须保留逐飞科技的版权声明。
 *
 * @file       		逐飞科技蓝牙转串口模块
 * @company	   		成都逐飞科技有限公司
 * @author     		逐飞科技(QQ3184284598)
 * @version    		查看doc内version文件 版本说明
 * @Software 		IAR 8.3 or MDK 5.33
 * @Taobao   		https://seekfree.taobao.com/
 * @date       		2021-08-27
 * @note		
					接线定义：
					------------------------------------ 
					    蓝牙转串口      单片机                        
    					RX              查看SEEKFREE_BLUETOOTH_CH9141.h文件中的BLUETOOTH_CH9141_UART_TX宏定义
    					TX              查看SEEKFREE_BLUETOOTH_CH9141.h文件中的BLUETOOTH_CH9141_UART_RX宏定义
    					RTS             查看SEEKFREE_BLUETOOTH_CH9141.h文件中的BLUETOOTH_CH9141_RTS_PIN宏定义
                        CTS             悬空
    					CMD             悬空或者上拉
					------------------------------------ 
 ********************************************************************************************************************/
 
#ifndef SEEKFREE_BLUETOOTH_CH9141_H
#define SEEKFREE_BLUETOOTH_CH9141_H

#include "common.h"


#define BLUETOOTH_CH9141_UART           UART_2         //无线转串口模块 所使用到的串口
#define BLUETOOTH_CH9141_UART_TX        UART2_TX_P10_5    //蓝牙的RX连接单片机的D16引脚
#define BLUETOOTH_CH9141_UART_RX        UART2_RX_P10_6    //蓝牙的TX连接单片机的D17引脚
#define BLUETOOTH_CH9141_UART_BAUD      115200

#define BLUETOOTH_CH9141_RTS_PIN        P10_2             //定义流控位引脚  指示当前模块是否可以接受数据  0可以继续接收  1不可以继续接收


typedef enum
{
    TX_POWER_0DB = 0,   //0DB
    TX_POWER_1DB,       //1DB
    TX_POWER_2DB,       //2DB
    TX_POWER_3DB,       //3DB
    TX_POWER_4DB,       //4DB
    TX_POWER_MINUS_8DB, //-8DB
    TX_POWER_MINUS_14DB,//-14DB
    TX_POWER_MINUS_20DB,//-20DB
}CH9141_TX_POWEER_enum; 

typedef enum
{
    MASTER_NO_INIT = 0,        //主机未初始化
    MASTER_SCANNING,           //主机扫描中
    MASTER_CONNECTING,         //主机连接中
    MASTER_CONNECTED,          //主机已连接
    MASTER_DISCONNECTED,       //主机连接断开
    
    SLAVE_NO_INIT,         //从机未初始化
    SLAVE_OK_INIT,         //从机完成初始化
    SLAVE_BROADCAST,       //从机广播中
    SLAVE_BROADCAST_READY, //从机准备广播
    SLAVE_CONNECT_TIMEOUT, //从机连接超时
    SLAVE_CONNECTED,       //从机已连接
    SLAVE_ERROR,           //从机出现错误
}CH9141_STATUS_enum; 

typedef enum
{
    BROADCAST_MODE = 0, //广播模式
    MASTER_MODE,       //主机模式
    SLAVE_MODE,    //从机模式
}CH9141_MODE_enum; 

extern uint8 uart_flag;
extern uint8 uart_data;
extern uint8 mac_address[17];      //本机mac地址

void bluetooth_ch9141_uart_callback(void);
void bluetooth_ch9141_enter_at_mode(void);
void bluetooth_ch9141_exit_at_mode(void);
void bluetooth_ch9141_reset(void);
void bluetooth_ch9141_get_mac_address(void);
int16 bluetooth_ch9141_get_rssi(void);
uint32 bluetooth_ch9141_send_buff(uint8 *buff, uint32 len);
void bluetooth_ch9141_init(CH9141_MODE_enum mode, int8 *salve_mac_password);









#endif
