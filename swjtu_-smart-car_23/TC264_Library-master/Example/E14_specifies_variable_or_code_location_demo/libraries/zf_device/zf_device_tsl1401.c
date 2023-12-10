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
* 文件名称          zf_device_tsl1401
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
/*********************************************************************************************************************
* 接线定义：
*                  ------------------------------------
*                  模块管脚             单片机管脚
*                  CLK                查看 zf_device_tsl1401.h 中 TSL1401_CLK_PIN 宏定义
*                  SI                 查看 zf_device_tsl1401.h 中 TSL1401_SI_PIN 宏定义
*                  AO[x]              查看 zf_device_tsl1401.h 中 TSL1401_AO_PIN_BUFFER 宏定义
*                  VCC                3.3V电源
*                  GND                电源地
*                  ------------------------------------
********************************************************************************************************************/

#include "zf_common_debug.h"
#include "zf_driver_adc.h"
#include "zf_driver_delay.h"
#include "zf_driver_gpio.h"
#include "zf_driver_pit.h"
#include "zf_driver_uart.h"
#include "zf_device_tsl1401.h"

uint16 tsl1401_data[2][TSL1401_DATA_LEN];                                       // TSL1401 数据存放数组

static uint8 tsl1401_init_state = 0;
vuint8 tsl1401_finish_flag;                                                     // TSL1401 数据准备就绪标志位

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     TSL1401 线阵 CCD 数据采集
// 参数说明     void
// 返回参数     void
// 使用示例     tsl1401_collect_pit_handler();
// 备注信息     该函数在 isr.c 中对应 TSL1401_PIT_INDEX 的中断服务函数调用
//-------------------------------------------------------------------------------------------------------------------
void tsl1401_collect_pit_handler (void)
{
    if(!tsl1401_init_state) return;

    uint8 i = 0;

    TSL1401_CLK(1);
    TSL1401_SI (0);
    TSL1401_CLK(0);
    TSL1401_SI (1);
    TSL1401_CLK(1);
    TSL1401_SI (0);

    for(i = 0; i < TSL1401_DATA_LEN; i ++)
    {
        TSL1401_CLK(0);
        tsl1401_data[0][i] = adc_convert(TSL1401_AO_PIN);
        tsl1401_data[1][i] = adc_convert(TSL1401_AO_PIN1);
        TSL1401_CLK(1);
    }

    tsl1401_finish_flag = 1;  // 采集完成标志位置1
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     TSL1401 线阵 CCD 图像发送至上位机查看图像
// 参数说明     uart_n          串口号
// 参数说明     index           对应接入的哪个 TSL1401 [0-1]
// 返回参数     void
// 使用示例     tsl1401_send_data(DEBUG_UART_INDEX, 1);
// 备注信息     调用该函数前请先初始化串口
//-------------------------------------------------------------------------------------------------------------------
void tsl1401_send_data (uart_index_enum uart_n, uint8 index)
{
    uint8 i;
    uart_write_byte(uart_n, 0x00);
    uart_write_byte(uart_n, 0xff);
    uart_write_byte(uart_n, 0x01);
    uart_write_byte(uart_n, 0x00);

    for(i=0; i<TSL1401_DATA_LEN; i++)
    {
        switch(TSL1401_AD_RESOLUTION)
        {
            case ADC_8BIT:  uart_write_byte(uart_n, (uint8)(tsl1401_data[index][i]));      break;
            case ADC_10BIT: uart_write_byte(uart_n, (uint8)(tsl1401_data[index][i] >> 2)); break;
            case ADC_12BIT: uart_write_byte(uart_n, (uint8)(tsl1401_data[index][i] >> 4)); break;
        }
    }
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     TSL1401 线阵 CCD 初始化
// 参数说明     void
// 返回参数     void
// 使用示例     tsl1401_init();
// 备注信息
//-------------------------------------------------------------------------------------------------------------------
void tsl1401_init (void)
{
    adc_init(TSL1401_AO_PIN, TSL1401_AD_RESOLUTION);
    adc_init(TSL1401_AO_PIN1, TSL1401_AD_RESOLUTION);
    gpio_init(TSL1401_CLK_PIN, GPO, GPIO_LOW, GPO_PUSH_PULL);
    gpio_init(TSL1401_SI_PIN, GPO, GPIO_LOW, GPO_PUSH_PULL);
    pit_ms_init(TSL1401_PIT_INDEX, TSL1401_EXPOSURE_TIME);
    tsl1401_init_state = 1;
}

