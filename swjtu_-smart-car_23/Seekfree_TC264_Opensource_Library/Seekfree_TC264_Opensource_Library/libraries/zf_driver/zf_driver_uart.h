/*********************************************************************************************************************
* TC264 Opensourec Library 即（TC264 开源库）是一个基于官方 SDK 接口的第三方开源库
* Copyright (c) 2022 SEEKFREE 逐飞科技
*
* 本文件是 TC264 开源库的一部分
*
* TC264 开源库 是免费软件
* 您可以根据自由软件基金会发布的 GPL（GNU General Public License，即 GNU通用公共许可证）的条款
* 即 GPL 的第3版（即 GPL3.0）或（您选择的）任何后来的版本，重新发布和/或修改它
*
* 本开源库的发布是希望它能发挥作用，但并未对其作任何的保证
* 甚至没有隐含的适销性或适合特定用途的保证
* 更多细节请参见 GPL
*
* 您应该在收到本开源库的同时收到一份 GPL 的副本
* 如果没有，请参阅<https://www.gnu.org/licenses/>
*
* 额外注明：
* 本开源库使用 GPL3.0 开源许可证协议 以上许可申明为译文版本
* 许可申明英文版在 libraries/doc 文件夹下的 GPL3_permission_statement.txt 文件中
* 许可证副本在 libraries 文件夹下 即该文件夹下的 LICENSE 文件
* 欢迎各位使用并传播本程序 但修改内容时必须保留逐飞科技的版权声明（即本声明）
*
* 文件名称          zf_driver_uart
* 公司名称          成都逐飞科技有限公司
* 版本信息          查看 libraries/doc 文件夹内 version 文件 版本说明
* 开发环境          ADS v1.8.0
* 适用平台          TC264D
* 店铺链接          https://seekfree.taobao.com/
*
* 修改记录
* 日期              作者                备注
* 2022-09-15       pudding            first version
********************************************************************************************************************/

#ifndef _zf_driver_uart_h_
#define _zf_driver_uart_h_

#include "ifxAsclin_Asc.h"
#include "zf_common_typedef.h"

#define UART0_TX_BUFFER_SIZE 256        // 定义串口0发送缓冲区大小
#define UART0_RX_BUFFER_SIZE 16         // 定义串口0接收缓冲区大小

#define UART1_TX_BUFFER_SIZE 256
#define UART1_RX_BUFFER_SIZE 16

#define UART2_TX_BUFFER_SIZE 256
#define UART2_RX_BUFFER_SIZE 16

#define UART3_TX_BUFFER_SIZE 256
#define UART3_RX_BUFFER_SIZE 16


typedef enum            // 枚举串口引脚 此枚举定义不允许用户修改
{
    UART0_TX_P14_0,     // 串口0 发送引脚可选范围
    UART0_TX_P14_1,
    UART0_TX_P15_2,
    UART0_TX_P15_3,

    UART1_TX_P02_2,     // 串口1 发送引脚可选范围
    UART1_TX_P11_12,
    UART1_TX_P15_0,
    UART1_TX_P15_1,
    UART1_TX_P15_4,
    UART1_TX_P15_5,
    UART1_TX_P20_10,
    UART1_TX_P33_12,
    UART1_TX_P33_13,

    UART2_TX_P02_0,     // 串口2 发送引脚可选范围
    UART2_TX_P10_5,
    UART2_TX_P14_2,
    UART2_TX_P14_3,
    UART2_TX_P33_8,
    UART2_TX_P33_9,

    UART3_TX_P00_0,     // 串口3 发送引脚可选范围
    UART3_TX_P00_1,
    UART3_TX_P15_6,
    UART3_TX_P15_7,
    UART3_TX_P20_0,
    UART3_TX_P20_3,
    UART3_TX_P21_7,
}uart_tx_pin_enum;


typedef enum            // 枚举串口引脚 此枚举定义不允许用户修改
{

    UART0_RX_P14_1,     // 串口0 接收引脚可选范围
    UART0_RX_P15_3,

    UART1_RX_P02_3,     // 串口1 接收引脚可选范围
    UART1_RX_P11_10,
    UART1_RX_P15_1,
    UART1_RX_P15_5,
    UART1_RX_P20_9,
    UART1_RX_P33_13,

    UART2_RX_P02_0,     // 串口2 接收引脚可选范围
    UART2_RX_P02_1,
    UART2_RX_P10_6,
    UART2_RX_P14_3,
    UART2_RX_P33_8,

    UART3_RX_P00_1,     // 串口3 接收引脚可选范围
    UART3_RX_P15_7,
    UART3_RX_P20_3,
    UART3_RX_P21_6,
}uart_rx_pin_enum;


typedef enum            // 枚举串口号 此枚举定义不允许用户修改
{
    UART_0,
    UART_1,
    UART_2,
    UART_3,
}uart_index_enum;

//创建串口handle变量
extern IfxAsclin_Asc uart0_handle;
extern IfxAsclin_Asc uart1_handle;
extern IfxAsclin_Asc uart2_handle;
extern IfxAsclin_Asc uart3_handle;

//====================================================串口 基础函数====================================================
void    uart_write_byte_wait                (uart_index_enum uart_n, const uint8 dat);
void    uart_write_byte                     (uart_index_enum uartn, const uint8 dat);
void    uart_write_buffer                   (uart_index_enum uartn, const uint8 *buff, uint32 len);
void    uart_write_string                   (uart_index_enum uartn, const char *str);

uint8   uart_read_byte                      (uart_index_enum uartn);
uint8   uart_query_byte                     (uart_index_enum uartn, uint8 *dat);

void    uart_tx_interrupt                   (uart_index_enum uartn, uint32 status);
void    uart_rx_interrupt                   (uart_index_enum uartn, uint32 status);

void    uart_init                           (uart_index_enum uartn, uint32 baud, uart_tx_pin_enum tx_pin, uart_rx_pin_enum rx_pin);
//====================================================串口 基础函数====================================================

//=================================================兼容旧版本开源库接口名称=================================================
#ifdef  COMPATIBLE_WITH_OLDER_VERSIONS
#define uart_putchar(uart_n, dat)           (uart_write_byte((uart_n), (dat)))
#define uart_putbuff(uart_n, buff, len)     (uart_write_buffer((uart_n), (buff), (len)))
#define uart_putstr(uart_n, str)            (uart_write_string((uart_n), (str)))

#define uart_getchar(uart_n, dat)           (*(dat) = uart_read_byte((uart_n)))
#define uart_query(uart_n, dat)             (uart_query_byte((uart_n), (dat)))

#define uart_tx_irq(uart_n, status)         (uart_tx_interrupt((uart_n), (status)))
#define uart_rx_irq(uart_n, status)         (uart_rx_interrupt((uart_n), (status)))
#endif
//=================================================兼容旧版本开源库接口名称=================================================

#endif
