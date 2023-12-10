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
* 文件名称          zf_device_type
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

#ifndef _zf_device_type_h_
#define _zf_device_type_h_

#include "zf_common_debug.h"

//==============================================定义 外设 枚举体==================================================
typedef enum
{
    NO_CAMERE = 0,                                                              // 无摄像头
    CAMERA_BIN_IIC,                                                             // 小钻风 IIC 版本
    CAMERA_BIN_UART,                                                            // 小钻风 UART 版本
    CAMERA_GRAYSCALE,                                                           // 总钻风
    CAMERA_COLOR,                                                               // 凌瞳
}camera_type_enum;

typedef enum
{
    NO_WIRELESS = 0,                                                            // 无设备
    WIRELESS_UART,                                                              // 无线串口
    BLUETOOTH_CH9141,                                                           // 蓝牙 CH9141
    GPS_TAU1201,
    WIFI_UART,
    WIFI_SPI,
}wireless_type_enum;
//==============================================定义 外设 枚举体==================================================

typedef void (*callback_function)(void);

//===========================================声明回调函数指针及外设类型==================================================
extern wireless_type_enum wireless_type;
extern callback_function wireless_module_uart_handler;                          // 无线串口接收中断函数指针，根据初始化时设置的函数进行跳转

extern camera_type_enum camera_type;
extern callback_function camera_dma_handler;                                    // 串口通讯中断函数指针，根据初始化时设置的函数进行跳转
extern callback_function camera_vsync_handler;                                  // 串口通讯中断函数指针，根据初始化时设置的函数进行跳转
extern callback_function camera_uart_handler;                                   // 串口通讯中断函数指针，根据初始化时设置的函数进行跳转
//===========================================声明回调函数指针及外设类型==================================================

//=============================================中断回调 基础函数===================================================
void   set_camera_type          (camera_type_enum type_set, callback_function vsync_callback, callback_function dma_callback, callback_function uart_callback);
void   set_wireless_type        (wireless_type_enum type_set, callback_function uart_callback);
//=============================================中断回调 基础函数===================================================

#endif
