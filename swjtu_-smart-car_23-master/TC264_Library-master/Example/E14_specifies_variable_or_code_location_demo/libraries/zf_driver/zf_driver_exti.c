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
* 日期              作者                备注
* 2022-09-15       pudding             first version
********************************************************************************************************************/

#include "Src/Std/IfxSrc.h"
#include "SysSe/Bsp/Bsp.h"
#include "isr_config.h"
#include "zf_common_debug.h"
#include "zf_driver_exti.h"

//-------------------------------------------------------------------------------------------------------------------
//  函数简介     获取端口参数
//  返回参数     IfxScu_Req_In
//  使用示例     get_exit_pin(ERU_CH0_REQ4_P10_7);
//  备注信息     内部调用
//-------------------------------------------------------------------------------------------------------------------
static IfxScu_Req_In* get_exit_pin (exti_pin_enum exti_pin)
{
    IfxScu_Req_In* get_exit_pin_config;

    switch(exti_pin)
    {
        case ERU_CH0_REQ0_P15_4:   get_exit_pin_config =  &IfxScu_REQ0_P15_4_IN;   break;
        case ERU_CH1_REQ10_P14_3:  get_exit_pin_config =  &IfxScu_REQ10_P14_3_IN;  break;
        case ERU_CH2_REQ7_P00_4:   get_exit_pin_config =  &IfxScu_REQ7_P00_4_IN;   break;
        case ERU_CH2_REQ14_P02_1:  get_exit_pin_config =  &IfxScu_REQ14_P02_1_IN;  break;
        case ERU_CH2_REQ2_P10_2:   get_exit_pin_config =  &IfxScu_REQ2_P10_2_IN;   break;
        case ERU_CH3_REQ6_P02_0:   get_exit_pin_config =  &IfxScu_REQ6_P02_0_IN;   break;
        case ERU_CH3_REQ3_P10_3:   get_exit_pin_config =  &IfxScu_REQ3_P10_3_IN;   break;
        case ERU_CH3_REQ15_P14_1:  get_exit_pin_config =  &IfxScu_REQ15_P14_1_IN;  break;
        case ERU_CH4_REQ13_P15_5:  get_exit_pin_config =  &IfxScu_REQ13_P15_5_IN;  break;
        case ERU_CH4_REQ8_P33_7:   get_exit_pin_config =  &IfxScu_REQ8_P33_7_IN;   break;
        case ERU_CH5_REQ1_P15_8:   get_exit_pin_config =  &IfxScu_REQ1_P15_8_IN;   break;
        case ERU_CH6_REQ12_P11_10: get_exit_pin_config =  &IfxScu_REQ12_P11_10_IN; break;
        case ERU_CH6_REQ9_P20_0:   get_exit_pin_config =  &IfxScu_REQ9_P20_0_IN;   break;
        case ERU_CH7_REQ16_P15_1:  get_exit_pin_config =  &IfxScu_REQ16_P15_1_IN;  break;
        case ERU_CH7_REQ11_P20_9:  get_exit_pin_config =  &IfxScu_REQ11_P20_9_IN;  break;
        default: zf_assert(FALSE); get_exit_pin_config =  NULL;
    }

    return get_exit_pin_config;
}



//-------------------------------------------------------------------------------------------------------------------
// 函数简介     EXTI 中断使能
// 参数说明     pin             选择 EXTI 引脚 (可选择范围由 zf_driver_exti.h 内 exti_pin_enum 枚举值确定)
// 返回参数     void
// 使用示例     exti_enable(ERU_CH0_REQ4_P10_7);
// 备注信息
//-------------------------------------------------------------------------------------------------------------------
void exti_enable (exti_pin_enum eru_pin)
{
    IfxScuEru_OutputChannel    outputChannel = (IfxScuEru_OutputChannel)(eru_pin/3);

    volatile Ifx_SRC_SRCR *src = &MODULE_SRC.SCU.SCU.ERU[(int)outputChannel % 4];
    IfxSrc_enable(src);
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     EXTI 中断失能
// 参数说明     pin             选择 EXTI 引脚 (可选择范围由 zf_driver_exti.h 内 exti_pin_enum 枚举值确定)
// 返回参数     void
// 使用示例     exti_disable(A0);
// 备注信息
//-------------------------------------------------------------------------------------------------------------------
void exti_disable (exti_pin_enum eru_pin)
{
    IfxScuEru_OutputChannel    outputChannel = (IfxScuEru_OutputChannel)(eru_pin/3);

    volatile Ifx_SRC_SRCR *src = &MODULE_SRC.SCU.SCU.ERU[(int)outputChannel % 4];
    IfxSrc_disable(src);
}

//-------------------------------------------------------------------------------------------------------------------
//  函数简介      EXTI 失能
//  返回参数      void
//  使用示例      exti_disable(ERU_CH0_REQ0_P15_4);
//-------------------------------------------------------------------------------------------------------------------
void exti_all_close (void)
{
    volatile Ifx_SRC_SRCR *src;
    int8 channel;
    for(channel = 0; channel < 4; channel ++)
    {
        src = &MODULE_SRC.SCU.SCU.ERU[channel];
        IfxSrc_deinit(src);
    }
}
//-------------------------------------------------------------------------------------------------------------------
//  函数简介      EXTI 中断初始化
//  参数说明      eru_pin         设置eru通道及引脚
//  参数说明      trigger         设置触发方式
//  返回参数      void
//  使用示例      exti_init(ERU_CH0_REQ0_P15_4, EXTI_TRIGGER_RISING);   // eru通道0 使用P15_4引脚，上升沿触发中断
//  备注信息
//-------------------------------------------------------------------------------------------------------------------
void exti_init (exti_pin_enum exti_pin, exti_trigger_enum trigger)
{
    boolean interrupt_state = disableInterrupts();

    IfxScu_Req_In *reqPin;

    reqPin = get_exit_pin(exti_pin);

    IfxScuEru_initReqPin(reqPin, IfxPort_InputMode_pullUp);

    IfxScuEru_InputChannel inputChannel = (IfxScuEru_InputChannel)reqPin->channelId;

    IfxScuEru_InputNodePointer triggerSelect = (IfxScuEru_InputNodePointer)(exti_pin/3);
    IfxScuEru_OutputChannel    outputChannel = (IfxScuEru_OutputChannel)(exti_pin/3);

    switch(trigger)
    {
        case EXTI_TRIGGER_RISING:
        {
            IfxScuEru_disableFallingEdgeDetection(inputChannel);
            IfxScuEru_enableRisingEdgeDetection(inputChannel);
        }break;

        case EXTI_TRIGGER_FALLING:
        {
            IfxScuEru_enableFallingEdgeDetection(inputChannel);
            IfxScuEru_disableRisingEdgeDetection(inputChannel);
        }break;

        case EXTI_TRIGGER_BOTH:
        {
            IfxScuEru_enableFallingEdgeDetection(inputChannel);
            IfxScuEru_enableRisingEdgeDetection(inputChannel);
        }break;

        default: zf_assert(FALSE);
    }

    IfxScuEru_enableTriggerPulse(inputChannel);
    IfxScuEru_connectTrigger(inputChannel, triggerSelect);

    IfxScuEru_setFlagPatternDetection(outputChannel, inputChannel, TRUE);
    IfxScuEru_enablePatternDetectionTrigger(outputChannel);
    IfxScuEru_setInterruptGatingPattern(outputChannel, IfxScuEru_InterruptGatingPattern_alwaysActive);


    volatile Ifx_SRC_SRCR *src = &MODULE_SRC.SCU.SCU.ERU[(int)outputChannel % 4];
    IfxSrc_Tos exit_service;
    uint8 exit_priority;
    switch((exti_pin/3)%4)
    {
        case 0:
        {
            exit_service  = EXTI_CH0_CH4_INT_SERVICE;
            exit_priority = EXTI_CH0_CH4_INT_PRIO;
        }break;

        case 1:
        {
            exit_service  = EXTI_CH1_CH5_INT_SERVICE;
            exit_priority = EXTI_CH1_CH5_INT_PRIO;
        }break;

        case 2:
        {
            exit_service  = EXTI_CH2_CH6_INT_SERVICE;
            exit_priority = EXTI_CH2_CH6_INT_PRIO;
        }break;

        case 3:
        {
            exit_service  = EXTI_CH3_CH7_INT_SERVICE;
            exit_priority = EXTI_CH3_CH7_INT_PRIO;
        }break;

    }
#pragma warning 507
    IfxSrc_init(src, exit_service, exit_priority);
#pragma warning default
    IfxSrc_enable(src);

    restoreInterrupts(interrupt_state);
}
