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
* 文件名称          zf_common_clock
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

#include "IfxScuEru.h"
#include "Ifxstm.h"
#include "Cpu0_Main.h"
#include "Cpu/Std/IfxCpu.h"
#include "zf_common_interrupt.h"
#include "zf_common_clock.h"

App_Cpu0 g_AppCpu0;                               // 频率信息变量

IFX_ALIGN(4) IfxCpu_syncEvent g_cpuSyncEvent = 0; // 事件同步变量

//-------------------------------------------------------------------------------------------------------------------
// 函数简介          设置系统频率
// 返回类型          void
// 使用示例          set_clock();
// 备注信息
//-------------------------------------------------------------------------------------------------------------------
void set_clock (void)
{
    IfxScuCcu_setCpuFrequency(IfxCpu_ResourceCpu_0, (float32)AURIX_MCU_FREQUENCY);
    IfxScuCcu_setCpuFrequency(IfxCpu_ResourceCpu_1, (float32)AURIX_MCU_FREQUENCY);
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介          获取系统频率
// 返回类型          void
// 使用示例          get_clock();
// 备注信息
//-------------------------------------------------------------------------------------------------------------------
void get_clock (void)
{
    // 获取时钟频率，便于查看当前系统运行频率
    g_AppCpu0.info.pllFreq = IfxScuCcu_getPllFrequency();
    g_AppCpu0.info.cpuFreq = IfxScuCcu_getCpuFrequency(IfxCpu_getCoreIndex());
    g_AppCpu0.info.sysFreq = IfxScuCcu_getSpbFrequency();
    g_AppCpu0.info.stmFreq = IfxStm_getFrequency(&MODULE_STM0);
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介          关闭看门狗
// 返回类型          void
// 使用示例          disable_Watchdog();
// 备注信息
//-------------------------------------------------------------------------------------------------------------------
void disable_Watchdog (void)
{
    IfxScuWdt_disableCpuWatchdog(IfxScuWdt_getCpuWatchdogPassword());
    IfxScuWdt_disableSafetyWatchdog(IfxScuWdt_getSafetyWatchdogPassword());
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介          时钟初始化
// 返回类型          void
// 使用示例          clock_init();
// 备注信息
//-------------------------------------------------------------------------------------------------------------------
void clock_init (void)
{
    interrupt_global_disable();     // 关闭总中断
    disable_Watchdog();             // 关闭看门狗
    get_clock();                    // 获取系统频率
    interrupt_global_enable(0);     // 打开全局中断
}


//-------------------------------------------------------------------------------------------------------------------
// 函数简介          等待所有核心初始化完毕
// 返回类型          void
// 使用示例          cpu_wait_event_ready();
// 备注信息
//-------------------------------------------------------------------------------------------------------------------
void cpu_wait_event_ready (void)
{
    IfxCpu_emitEvent(&g_cpuSyncEvent);
    IfxCpu_waitEvent(&g_cpuSyncEvent, 0xFFFF);
}
