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

#include "IfxCcu6_Timer.h"
#include "SysSe/Bsp/Bsp.h"
#include "isr_config.h"
#include "zf_driver_pit.h"

//-------------------------------------------------------------------------------------------------------------------
//  函数简介      pit关闭
//  参数说明      pit_index        选择CCU6模块
//  返回参数      void
//  使用示例      pit_close(CCU60_CH0); // 关闭CCU60 通道0的计时器
//  备注信息
//-------------------------------------------------------------------------------------------------------------------
void pit_close (pit_index_enum pit_index)
{
    volatile Ifx_CCU6 *module;
    IfxCcu6_Timer g_Ccu6Timer;

    module = IfxCcu6_getAddress((IfxCcu6_Index)(pit_index / 2));

    g_Ccu6Timer.ccu6 = module;
    g_Ccu6Timer.timer = (IfxCcu6_TimerId)((pit_index % 2));

    IfxCcu6_Timer_stop(&g_Ccu6Timer);
}

//-------------------------------------------------------------------------------------------------------------------
//  函数简介      pit启动
//  参数说明      pit_index           选择CCU6模块
//  返回参数      void
//  使用示例      pit_start(CCU60_CH0); // 打开CCU60 通道0的计时器
//  备注信息
//-------------------------------------------------------------------------------------------------------------------
void pit_start (pit_index_enum pit_index)
{
    volatile Ifx_CCU6 *module;
    IfxCcu6_Timer g_Ccu6Timer;

    module = IfxCcu6_getAddress((IfxCcu6_Index)(pit_index / 2));

    g_Ccu6Timer.ccu6 = module;
    g_Ccu6Timer.timer = (IfxCcu6_TimerId)((pit_index % 2));

    IfxCcu6_Timer_start(&g_Ccu6Timer);
}

//-------------------------------------------------------------------------------------------------------------------
//  函数简介      禁止所有pit中断
//  返回参数      void
//  使用示例      pit_interrupt_all_close();
//  备注信息
//-------------------------------------------------------------------------------------------------------------------
void pit_all_close (void)
{
    IfxCcu6_disableModule((Ifx_CCU6 *)IfxCcu6_cfg_indexMap[0].module);
    IfxCcu6_disableModule((Ifx_CCU6 *)IfxCcu6_cfg_indexMap[1].module);
}


//-------------------------------------------------------------------------------------------------------------------
//  函数简介      失能pit中断
//  参数说明      pit_index           选择CCU6模块
//  返回参数      void
//  使用示例      pit_disable_interrupt(CCU60_CH0); // 禁止CCU60 通道0的中断
//  备注信息
//-------------------------------------------------------------------------------------------------------------------
void pit_disable (pit_index_enum pit_index)
{
    volatile Ifx_CCU6 *module;
    module = IfxCcu6_getAddress((IfxCcu6_Index)(pit_index / 2));
    IfxCcu6_disableInterrupt(module, (pit_index % 2) * 2 + 7);
}

//-------------------------------------------------------------------------------------------------------------------
//  函数简介      使能pit中断
//  参数说明      pit_index           选择CCU6模块
//  返回参数      void
//  使用示例      pit_enable_interrupt(CCU60_CH0);  // 开启CCU60 通道0的中断
//  备注信息
//-------------------------------------------------------------------------------------------------------------------
void pit_enable (pit_index_enum pit_index)
{
    volatile Ifx_CCU6 *module;
    module = IfxCcu6_getAddress((IfxCcu6_Index)(pit_index / 2));
    IfxCcu6_enableInterrupt(module, (pit_index % 2) * 2 + 7);
}

//-------------------------------------------------------------------------------------------------------------------
//  函数简介      pit初始化
//  参数说明      pit_index           选择CCU6模块
//  参数说明      time                周期时间
//  返回参数      void
//  使用示例      pit_init(CCU60_CH0, 5000);      // 设置周期中断5000us
//  备注信息      请使用.h文件中 带时间单位的宏定义函数
//-------------------------------------------------------------------------------------------------------------------
void pit_init (pit_index_enum pit_index, uint32 time)
{
    uint8 i;
    volatile Ifx_CCU6 *module;
    uint64 timer_input_clk;
    IfxCcu6_Timer g_Ccu6Timer;
    IfxCcu6_Timer_Config timerConfig;
    uint32 timer_period;

    boolean interrupt_state = disableInterrupts();

    module = IfxCcu6_getAddress((IfxCcu6_Index)(pit_index/2));

    IfxCcu6_Timer_initModuleConfig(&timerConfig, module);



    timer_input_clk = IfxScuCcu_getSpbFrequency();
    i = 0;
    while(i < 16)
    {
        timer_period = (uint32)(timer_input_clk * time / 1000000);
        if(timer_period < 0xffff)   break;
        timer_input_clk >>= 1;
        i++;
    }
    if(16 <= i) IFX_ASSERT(IFX_VERBOSE_LEVEL_ERROR, FALSE);


    switch(pit_index)
    {
        case CCU60_CH0:
        {
            timerConfig.interrupt1.typeOfService  = CCU6_0_CH0_INT_SERVICE;
            timerConfig.interrupt1.priority       = CCU6_0_CH0_ISR_PRIORITY;
            break;
        }
        case CCU60_CH1:
        {
            timerConfig.interrupt2.typeOfService  = CCU6_0_CH1_INT_SERVICE;
            timerConfig.interrupt2.priority       = CCU6_0_CH1_ISR_PRIORITY;
            break;
        }
        case CCU61_CH0:
        {
            timerConfig.interrupt1.typeOfService  = CCU6_1_CH0_INT_SERVICE;
            timerConfig.interrupt1.priority       = CCU6_1_CH0_ISR_PRIORITY;
            break;
        }
        case CCU61_CH1:
        {
            timerConfig.interrupt2.typeOfService  = CCU6_1_CH1_INT_SERVICE;
            timerConfig.interrupt2.priority       = CCU6_1_CH1_ISR_PRIORITY;
            break;
        }
    }

    if((pit_index % 2) == 0)
    {
        timerConfig.timer = IfxCcu6_TimerId_t12;
        timerConfig.interrupt1.source          = IfxCcu6_InterruptSource_t12PeriodMatch;
        timerConfig.interrupt1.serviceRequest  = IfxCcu6_ServiceRequest_1;
        timerConfig.base.t12Period             = timer_period;
        timerConfig.base.t12Frequency          = (float)timer_input_clk;
        timerConfig.clock.t12countingInputMode = IfxCcu6_CountingInputMode_internal;
    }
    else
    {
        timerConfig.timer = IfxCcu6_TimerId_t13;
        timerConfig.interrupt2.source          = IfxCcu6_InterruptSource_t13PeriodMatch;
        timerConfig.interrupt2.serviceRequest  = IfxCcu6_ServiceRequest_2;
        timerConfig.base.t13Period             = timer_period;
        timerConfig.base.t13Frequency          = (float)timer_input_clk;
        timerConfig.clock.t13countingInputMode = IfxCcu6_CountingInputMode_internal;
    }
    timerConfig.timer12.counterValue     = 0;
    timerConfig.timer13.counterValue     = 0;
    timerConfig.trigger.t13InSyncWithT12 = FALSE;

    IfxCcu6_Timer_initModule(&g_Ccu6Timer, &timerConfig);

    restoreInterrupts(interrupt_state);

    IfxCcu6_setSuspendMode(module, IfxCcu6_SuspendMode_hard);
    IfxCcu6_Timer_start(&g_Ccu6Timer);
}
