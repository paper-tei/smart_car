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
* 文件名称          zf_driver_pit
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

#ifndef _zf_driver_pit_h_
#define _zf_driver_pit_h_

#include "zf_common_typedef.h"

typedef enum // 枚举通道号
{
    CCU60_CH0,
    CCU60_CH1,
    CCU61_CH0,
    CCU61_CH1,
}pit_index_enum;


#define pit_clear_flag(pit_index) (IfxCcu6_clearInterruptStatusFlag(IfxCcu6_getAddress((IfxCcu6_Index)(pit_index / 2)), (IfxCcu6_InterruptSource)(7+((pit_index % 2)*2))))

//====================================================PIT 基础函数====================================================
void pit_close               (pit_index_enum pit_index);
void pit_start               (pit_index_enum pit_index);
void pit_all_close           (void);
void pit_disable             (pit_index_enum pit_index);
void pit_enable              (pit_index_enum pit_index);
void pit_init                (pit_index_enum pit_index, uint32 time);
//====================================================PIT 基础函数====================================================

//====================================================PIT 扩展函数====================================================
//-------------------------------------------------------------------------------------------------------------------
//  函数简介      pit_ms初始化
//  参数说明      pit_index           选择CCU6模块
//  参数说明      time                周期时间(单位：毫秒)
//  返回参数      void
//  使用示例      pit_ms_init(CCU60_CH0, 5);      // 设置周期中断5ms
//-------------------------------------------------------------------------------------------------------------------
#define pit_ms_init(pit_index, time)  pit_init((pit_index), (time*1000))   // (单位为 毫秒)
//-------------------------------------------------------------------------------------------------------------------
//  函数简介      pit_us初始化
//  参数说明      pit_index           选择CCU6模块
//  参数说明      time                周期时间(单位：微秒)
//  返回参数      void
//  使用示例      pit_us_init(CCU60_CH0, 5);      // 设置周期中断5us
//-------------------------------------------------------------------------------------------------------------------
#define pit_us_init(pit_index, time)  pit_init((pit_index), (time))        // (单位为 微秒)
//====================================================PIT 扩展函数====================================================

#endif
