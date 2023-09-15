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
* 文件名称          zf_driver_exti
* 公司名称          成都逐飞科技有限公司
* 版本信息          查看 libraries/doc 文件夹内 version 文件 版本说明
* 开发环境          ADS v1.8.0
* 适用平台          TC264D
* 店铺链接          https://seekfree.taobao.com/
*
* 修改记录
* 日期              作者               备注
* 2022-09-15       pudding            first version
********************************************************************************************************************/

#ifndef _zf_driver_exti_h_
#define _zf_driver_exti_h_

#include "IfxScuEru.h"
#include "zf_common_typedef.h"

typedef enum  // 枚举ERU通道
{
    // 一个通道只能选择其中一个引脚作为 外部中断的输入
    ERU_CH0_REQ0_P15_4   = 0*3 + 1,                                                 // 通道0可选引脚
    ERU_CH1_REQ10_P14_3  = 1*3 + 1,                                                 // 通道1可选引脚
    // 特别注意通道2 与 通道3都被摄像头占用
    // 特别注意通道2 与 通道3都被摄像头占用
    // 特别注意通道2 与 通道3都被摄像头占用
    ERU_CH2_REQ7_P00_4   = 2*3,  ERU_CH2_REQ14_P02_1, ERU_CH2_REQ2_P10_2,           // 通道2可选引脚
    ERU_CH3_REQ6_P02_0   = 3*3,  ERU_CH3_REQ3_P10_3,  ERU_CH3_REQ15_P14_1,          // 通道3可选引脚

    // 通道4与通道0 共用中断函数 在中断内通过判断标志位来识别是哪个通道触发的中断
    ERU_CH4_REQ13_P15_5  = 4*3,  ERU_CH4_REQ8_P33_7,                                // 通道4可选引脚
    // 通道5与通道1 共用中断函数
    ERU_CH5_REQ1_P15_8   = 5*3,                                                     // 通道5可选引脚
    // 通道6与通道2 共用中断函数
    ERU_CH6_REQ12_P11_10 = 6*3,  ERU_CH6_REQ9_P20_0,                                // 通道6可选引脚
    // 通道7与通道3 共用中断函数
    ERU_CH7_REQ16_P15_1  = 7*3,  ERU_CH7_REQ11_P20_9,                               // 通道7可选引脚
}exti_pin_enum;



typedef enum  // 枚举触发方式
{
    EXTI_TRIGGER_RISING,                                                            // 上升沿触发模式
    EXTI_TRIGGER_FALLING,                                                           // 下降沿触发模式
    EXTI_TRIGGER_BOTH,                                                              // 双边沿触发模式
}exti_trigger_enum;



// 中断标志位获取
#define exti_flag_get(eru_pin)      IfxScuEru_getEventFlagStatus((IfxScuEru_InputChannel)(eru_pin/3))
// 中断标志位清除
#define exti_flag_clear(eru_pin)    IfxScuEru_clearEventFlag((IfxScuEru_InputChannel)(eru_pin/3))

//====================================================EXIT 基础函数====================================================
void exti_all_close         (void);                                                 // EXTI 失能
void exti_enable            (exti_pin_enum eru_pin);                                // EXTI 中断使能
void exti_disable           (exti_pin_enum eru_pin);                                // EXTI 中断失能
void exti_init              (exti_pin_enum eru_pin, exti_trigger_enum trigger);     // EXTI 中断初始化
//====================================================EXIT 基础函数====================================================


#endif
