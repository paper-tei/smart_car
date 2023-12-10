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
* 文件名称          zf_driver_pwm
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

#ifndef _zf_driver_pwm_h_
#define _zf_driver_pwm_h_

#include "zf_common_typedef.h"

#define PWM_DUTY_MAX     10000                 // PWM最大占空比  最大占空比越大占空比的步进值越小

// 此枚举定义不允许用户修改
typedef enum // 枚举PWM引脚
{
    ATOM0_CH0_P00_0,  ATOM0_CH0_P02_0,  ATOM0_CH0_P02_8,  ATOM0_CH0_P14_5,  ATOM0_CH0_P21_2,  ATOM0_CH0_P22_1,
    ATOM0_CH1_P00_1,  ATOM0_CH1_P00_2,  ATOM0_CH1_P02_1,  ATOM0_CH1_P10_1,  ATOM0_CH1_P14_4,  ATOM0_CH1_P21_3, ATOM0_CH1_P22_0,  ATOM0_CH1_P33_9,
    ATOM0_CH2_P00_3,  ATOM0_CH2_P02_2,  ATOM0_CH2_P10_2,  ATOM0_CH2_P10_5,  ATOM0_CH2_P14_3,  ATOM0_CH2_P21_4, ATOM0_CH2_P33_11,
    ATOM0_CH3_P00_4,  ATOM0_CH3_P02_3,  ATOM0_CH3_P10_3,  ATOM0_CH3_P10_6,  ATOM0_CH3_P14_2,  ATOM0_CH3_P21_5, ATOM0_CH3_P22_2,
    ATOM0_CH4_P00_5,  ATOM0_CH4_P02_4,  ATOM0_CH4_P14_1,  ATOM0_CH4_P20_3,  ATOM0_CH4_P21_6,  ATOM0_CH4_P22_3,
    ATOM0_CH5_P00_6,  ATOM0_CH5_P02_5,  ATOM0_CH5_P21_7,  ATOM0_CH5_P32_4,
    ATOM0_CH6_P00_7,  ATOM0_CH6_P02_6,  ATOM0_CH6_P20_0,  ATOM0_CH6_P23_1,
    ATOM0_CH7_P00_8,  ATOM0_CH7_P02_7,  ATOM0_CH7_P20_8,

    ATOM1_CH0_P00_0,  ATOM1_CH0_P02_0,  ATOM1_CH0_P02_8,  ATOM1_CH0_P15_5,  ATOM1_CH0_P15_6,  ATOM1_CH0_P20_12, ATOM1_CH0_P21_2, ATOM1_CH0_P22_1,
    ATOM1_CH1_P00_1,  ATOM1_CH1_P00_2,  ATOM1_CH1_P02_1,  ATOM1_CH1_P10_1,  ATOM1_CH1_P14_6,  ATOM1_CH1_P15_7,  ATOM1_CH1_P15_8, ATOM1_CH1_P20_13, ATOM1_CH1_P21_3, ATOM1_CH1_P22_0, ATOM1_CH1_P33_9,
    ATOM1_CH2_P00_3,  ATOM1_CH2_P02_2,  ATOM1_CH2_P10_2,  ATOM1_CH2_P10_5,  ATOM1_CH2_P14_0,  ATOM1_CH2_P20_14, ATOM1_CH2_P21_4, ATOM1_CH2_P33_11,
    ATOM1_CH3_P00_4,  ATOM1_CH3_P02_3,  ATOM1_CH3_P10_3,  ATOM1_CH3_P10_6,  ATOM1_CH3_P15_0,  ATOM1_CH3_P21_5,  ATOM1_CH3_P22_2,
    ATOM1_CH4_P00_5,  ATOM1_CH4_P02_4,  ATOM1_CH4_P15_1,  ATOM1_CH4_P20_3,  ATOM1_CH4_P21_6,  ATOM1_CH4_P22_3,
    ATOM1_CH5_P00_6,  ATOM1_CH5_P02_5,  ATOM1_CH5_P15_2,  ATOM1_CH5_P20_9,  ATOM1_CH5_P21_7,  ATOM1_CH5_P32_4,
    ATOM1_CH6_P00_7,  ATOM1_CH6_P02_6,  ATOM1_CH6_P15_3,  ATOM1_CH6_P20_0,  ATOM1_CH6_P20_10, ATOM1_CH6_P23_1,
    ATOM1_CH7_P00_8,  ATOM1_CH7_P02_7,  ATOM1_CH7_P15_4,  ATOM1_CH7_P20_11,

    ATOM2_CH0_P00_9,  ATOM2_CH0_P13_3,  ATOM2_CH0_P20_12, ATOM2_CH0_P33_4,  ATOM2_CH0_P33_10,
    ATOM2_CH1_P11_2,  ATOM2_CH1_P20_13, ATOM2_CH1_P33_5,
    ATOM2_CH2_P11_3,  ATOM2_CH2_P20_14, ATOM2_CH2_P33_6,
    ATOM2_CH3_P00_12, ATOM2_CH3_P11_6,  ATOM2_CH3_P15_0,  ATOM2_CH3_P33_7,
    ATOM2_CH4_P11_9,  ATOM2_CH4_P15_1,  ATOM2_CH4_P33_8,  ATOM2_CH4_P33_12,
    ATOM2_CH5_P11_10, ATOM2_CH5_P13_0,  ATOM2_CH5_P15_2,  ATOM2_CH5_P20_9,  ATOM2_CH5_P33_13,
    ATOM2_CH6_P11_11, ATOM2_CH6_P13_1,  ATOM2_CH6_P15_3,  ATOM2_CH6_P20_6,  ATOM2_CH6_P20_10, ATOM2_CH6_P32_0,
    ATOM2_CH7_P11_12, ATOM2_CH7_P13_2,  ATOM2_CH7_P15_4,  ATOM2_CH7_P20_7,  ATOM2_CH7_P20_8,  ATOM2_CH7_P20_11,

    ATOM3_CH0_P00_9,  ATOM3_CH0_P13_3,  ATOM3_CH0_P14_5,  ATOM3_CH0_P15_5,  ATOM3_CH0_P15_6,  ATOM3_CH0_P33_4,  ATOM3_CH0_P33_10,
    ATOM3_CH1_P11_2,  ATOM3_CH1_P14_4,  ATOM3_CH1_P14_6,  ATOM3_CH1_P15_7,  ATOM3_CH1_P15_8,  ATOM3_CH1_P33_5,
    ATOM3_CH2_P11_3,  ATOM3_CH2_P14_0,  ATOM3_CH2_P14_3,  ATOM3_CH2_P33_6,
    ATOM3_CH3_P00_12, ATOM3_CH3_P11_6,  ATOM3_CH3_P14_2,  ATOM3_CH3_P33_7,
    ATOM3_CH4_P11_9,  ATOM3_CH4_P14_1,  ATOM3_CH4_P33_8,  ATOM3_CH4_P33_12,
    ATOM3_CH5_P11_10, ATOM3_CH5_P13_0,  ATOM3_CH5_P33_13,
    ATOM3_CH6_P11_11, ATOM3_CH6_P13_1,  ATOM3_CH6_P20_6,  ATOM3_CH6_P32_0,
    ATOM3_CH7_P11_12, ATOM3_CH7_P13_2,  ATOM3_CH7_P20_7,
}pwm_channel_enum;

//====================================================PWM 基础函数====================================================
void pwm_all_channel_close      (void);
void pwm_init                   (pwm_channel_enum pwmch, uint32 freq, uint32 duty);
void pwm_set_duty               (pwm_channel_enum pwmch, uint32 duty);
//====================================================PWM 基础函数====================================================

#endif
