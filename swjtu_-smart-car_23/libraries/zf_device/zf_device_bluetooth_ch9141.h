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
* 文件名称          main
* 公司名称          成都逐飞科技有限公司
* 版本信息          查看 libraries/doc 文件夹内 version 文件 版本说明
* 开发环境          ADS v1.8.0
* 适用平台          TC264D
* 店铺链接          https://seekfree.taobao.com/
*
* 修改记录
* 日期              作者                备注
* 2022-09-15       pudding             first version
********************************************************************************************************************/
/*********************************************************************************************************************
* 接线定义：
*                 ------------------------------------
*                 蓝牙转串口           单片机
*                 RX                  查看zf_device_bluetooth_ch9141.h文件中的BLUETOOTH_CH9141_TX_PIN宏定义
*                 TX                  查看zf_device_bluetooth_ch9141.h文件中的BLUETOOTH_CH9141_RX_PIN宏定义
*                 RTS                 查看zf_device_bluetooth_ch9141.h文件中的BLUETOOTH_CH9141_RTS_PIN宏定义
*                 CTS                 悬空
*                 CMD                 悬空或者上拉
*                 ------------------------------------
*********************************************************************************************************************/

#ifndef _zf_device_bluetooth_ch9141_h_
#define _zf_device_bluetooth_ch9141_h_

#include "zf_common_typedef.h"


//=================================================9141蓝牙 驱动配置====================================================
#define BLUETOOTH_CH9141_INDEX              (UART_2)                        // 蓝牙模块对应使用的串口号
#define BLUETOOTH_CH9141_BUAD_RATE          (115200)                        // 蓝牙模块对应使用的串口波特率
#define BLUETOOTH_CH9141_TX_PIN             (UART2_RX_P10_6)                // 蓝牙模块对应模块的 TX 要接到单片机的 RX
#define BLUETOOTH_CH9141_RX_PIN             (UART2_TX_P10_5)                // 蓝牙模块对应模块的 RX 要接到单片机的 TX
#define BLUETOOTH_CH9141_RTS_PIN            (P10_2)                         // 蓝牙模块对应模块的 RTS 引脚
//=================================================9141蓝牙 驱动配置====================================================

#define BLUETOOTH_CH9141_BUFFER_SIZE        (64)
#define BLUETOOTH_CH9141_TIMEOUT_COUNT      (500)

//=================================================9141蓝牙 基础函数====================================================
uint32  bluetooth_ch9141_send_byte          (const uint8 data);
uint32  bluetooth_ch9141_send_buff          (const uint8 *buff, uint32 len);
uint32  bluetooth_ch9141_send_string        (const char *str);
void    bluetooth_ch9141_send_image         (const uint8 *image_addr, uint32 image_size);

uint32  bluetooth_ch9141_read_buff          (uint8 *buff, uint32 len);

void    bluetooth_ch9141_uart_callback      (void);

uint8   bluetooth_ch9141_init               (void);
//=================================================9141蓝牙 基础函数====================================================

#endif
