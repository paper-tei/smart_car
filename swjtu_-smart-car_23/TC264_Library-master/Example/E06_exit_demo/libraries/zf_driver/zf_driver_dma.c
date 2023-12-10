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
* 文件名称          zf_driver_dma
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

#include "IfxDma_Dma.h"
#include "IfxScuEru.h"
#include "isr_config.h"
#include "zf_common_debug.h"
#include "zf_driver_dma.h"

typedef struct
{
    Ifx_DMA_CH linked_list[8];              // DMA链表
    IfxDma_Dma_Channel channel;             // DMA通道句柄
}DMA_LINK;

#if(0 == DMA_INT_SERVICE)
#pragma section all "cpu0_dsram"
IFX_ALIGN(256) DMA_LINK dma_link_list;

#elif(1 == DMA_INT_SERVICE)
#pragma section all "cpu1_dsram"
IFX_ALIGN(256) DMA_LINK dma_link_list;

#endif
#pragma section all restore
//-------------------------------------------------------------------------------------------------------------------
// 函数简介      dma初始化
// 参数说明      dma_ch              选择DMA通道
// 参数说明      source_addr         设置源地址
// 参数说明      destination_addr    设置目的地址
// 参数说明      exti_pin            设置触发的eru通道
// 参数说明      trigger             设置触发方式
// 参数说明      dma_count           设置dma搬移次数
// 返回参数      uint8
// 使用示例      dma_init(MT9V03X_DMA_CH, MT9V03X_DATA_ADD, mt9v03x_image[0], MT9V03X_PCLK_PIN, EXTI_TRIGGER_RISING, MT9V03X_IMAGE_SIZE);
// 备注信息
//-------------------------------------------------------------------------------------------------------------------
uint8 dma_init (IfxDma_ChannelId dma_ch, uint8 *source_addr, uint8 *destination_addr, exti_pin_enum exti_pin, exti_trigger_enum trigger, uint16 dma_count)
{
    IfxDma_Dma_Channel dmaChn;

    exti_init(exti_pin, trigger);               // eru触发DMA通道号   在eru文件中设置eru的优先级，即为触发的通道

    IfxDma_Dma_Config        dmaConfig;
    IfxDma_Dma_initModuleConfig(&dmaConfig, &MODULE_DMA);

    IfxDma_Dma               dma;
    IfxDma_Dma_initModule(&dma, &dmaConfig);

    IfxDma_Dma_ChannelConfig cfg;
    IfxDma_Dma_initChannelConfig(&cfg, &dma);

    uint8  list_num, i;
    uint16 single_channel_dma_count;

    zf_assert(!(dma_count%8));                  // 传输次数必须为8的倍数


    list_num = 1;
    single_channel_dma_count = dma_count / list_num;
    if(16384 < single_channel_dma_count)
    {
        while(TRUE)
        {
            single_channel_dma_count = dma_count / list_num;
            if((single_channel_dma_count <= 16384) && !(dma_count % list_num))
            {
                break;
            }
            list_num++;
            if(list_num > 8)
            {
                zf_assert(FALSE);
            }
        }
    }


    if(1 == list_num)
    {
        cfg.shadowControl               = IfxDma_ChannelShadow_none;
        cfg.operationMode               = IfxDma_ChannelOperationMode_single;
        cfg.shadowAddress               = 0;
    }
    else
    {
        cfg.shadowControl               = IfxDma_ChannelShadow_linkedList;
        cfg.operationMode               = IfxDma_ChannelOperationMode_continuous;
        cfg.shadowAddress               = IFXCPU_GLB_ADDR_DSPR(IfxCpu_getCoreId(), (unsigned)&dma_link_list.linked_list[1]);
    }

    cfg.requestMode                     = IfxDma_ChannelRequestMode_oneTransferPerRequest;
    cfg.moveSize                        = IfxDma_ChannelMoveSize_8bit;
    cfg.busPriority                     = IfxDma_ChannelBusPriority_high;

    cfg.sourceAddress                   = IFXCPU_GLB_ADDR_DSPR(IfxCpu_getCoreId(), source_addr);
    cfg.sourceAddressCircularRange      = IfxDma_ChannelIncrementCircular_none;
    cfg.sourceCircularBufferEnabled     = TRUE;

    cfg.destinationAddressIncrementStep = IfxDma_ChannelIncrementStep_1;

    cfg.channelId                       = (IfxDma_ChannelId)dma_ch;
    cfg.hardwareRequestEnabled          = FALSE;
    cfg.channelInterruptEnabled         = TRUE;
    cfg.channelInterruptPriority        = DMA_INT_PRIO;
    cfg.channelInterruptTypeOfService   = DMA_INT_SERVICE;



    cfg.destinationAddress              = IFXCPU_GLB_ADDR_DSPR(IfxCpu_getCoreId(), destination_addr);

    cfg.transferCount                   = single_channel_dma_count;

    IfxDma_Dma_initChannel(&dmaChn, &cfg);

    if(1 < list_num)
    {
        i = 0;
        while(i < list_num)
        {
            cfg.destinationAddress      = IFXCPU_GLB_ADDR_DSPR(IfxCpu_getCoreId(), destination_addr + single_channel_dma_count * i);
            if(i == (list_num - 1))
            {
                cfg.shadowAddress       = IFXCPU_GLB_ADDR_DSPR(IfxCpu_getCoreId(), (unsigned)&dma_link_list.linked_list[0]);
            }
            else
            {
                cfg.shadowAddress       = IFXCPU_GLB_ADDR_DSPR(IfxCpu_getCoreId(), (unsigned)&dma_link_list.linked_list[i+1]);
            }
            cfg.transferCount           = single_channel_dma_count;

            IfxDma_Dma_initLinkedListEntry((void *)&dma_link_list.linked_list[i], &cfg);
            i++;
        }
    }

    IfxDma_Dma_getSrcPointer(&dma_link_list.channel)->B.CLRR = 1;

    return list_num;
}


//-------------------------------------------------------------------------------------------------------------------
// 函数简介     dma 传输禁止
// 参数说明     ch              选择 dma 通道 (详见 zf_driver_dma.h 中枚举 dma_channel_enum 定义)
// 返回参数     void
// 使用示例     dma_disable(MT9V03X_DMA_CH);
// 备注信息
//-------------------------------------------------------------------------------------------------------------------
void dma_disable (IfxDma_ChannelId dma_ch)
{
    IfxDma_disableChannelTransaction(&MODULE_DMA, dma_ch);
}


//-------------------------------------------------------------------------------------------------------------------
// 函数简介     dma 传输使能
// 参数说明     ch              选择 dma 通道 (详见 zf_driver_dma.h 中枚举 dma_channel_enum 定义)
// 返回参数     void
// 使用示例     dma_enable(MT9V03X_DMA_CH);
// 备注信息
//-------------------------------------------------------------------------------------------------------------------
void dma_enable (IfxDma_ChannelId dma_ch)
{
    IfxDma_enableChannelTransaction(&MODULE_DMA, dma_ch);
}
