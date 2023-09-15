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
* 文件名称          zf_driver_spi
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

#include "IFXQSPI_REGDEF.h"
#include "IfxQspi_SpiMaster.h"
#include "IfxQspi.h"
#include "zf_common_debug.h"
#include "zf_driver_gpio.h"
#include "zf_driver_delay.h"
#include "zf_driver_spi.h"

#define MAX_BAUD    50000000
Ifx_QSPI_BACON      bacon;
spi_cs_pin_enum     spi_cs_pin;

void spi_mux (spi_index_enum spi_n, spi_sck_pin_enum sck_pin, spi_mosi_pin_enum mosi_pin, spi_miso_pin_enum miso_pin, spi_cs_pin_enum cs_pin, IfxQspi_SpiMaster_Pins *set_pin, IfxQspi_SpiMaster_Output *set_cs)
{
    set_pin->mrstMode  = IfxPort_InputMode_pullDown;
    set_pin->mtsrMode  = IfxPort_OutputMode_pushPull;
    set_pin->sclkMode  = IfxPort_OutputMode_pushPull;
    set_pin->pinDriver = IfxPort_PadDriver_cmosAutomotiveSpeed1;

    set_cs->driver     = IfxPort_PadDriver_cmosAutomotiveSpeed1;
    set_cs->mode       = IfxPort_OutputMode_pushPull;

    switch(spi_n)
    {
        case SPI_0:
        {
            if      (SPI0_SCLK_P20_11 == sck_pin)   set_pin->sclk = &IfxQspi0_SCLK_P20_11_OUT;
            else if (SPI0_SCLK_P20_13 == sck_pin)   set_pin->sclk = &IfxQspi0_SCLK_P20_13_OUT;
            else    zf_assert(FALSE);

            if      (SPI0_MOSI_P20_12 == mosi_pin)  set_pin->mtsr = &IfxQspi0_MTSR_P20_12_OUT;
            else if (SPI0_MOSI_P20_14 == mosi_pin)  set_pin->mtsr = &IfxQspi0_MTSR_P20_14_OUT;
            else    zf_assert(FALSE);

            if      (SPI0_MISO_P20_12 == miso_pin)  set_pin->mrst = &IfxQspi0_MRSTA_P20_12_IN;
            else    zf_assert(FALSE);

            if      (SPI0_CS0_P20_8   == cs_pin ||
                          SPI_CS_NULL == cs_pin)    set_cs->pin   = &IfxQspi0_SLSO0_P20_8_OUT;
            else if (SPI0_CS1_P20_9   == cs_pin)    set_cs->pin   = &IfxQspi0_SLSO1_P20_9_OUT;
            else if (SPI0_CS2_P20_13  == cs_pin)    set_cs->pin   = &IfxQspi0_SLSO2_P20_13_OUT;
            else if (SPI0_CS3_P11_10  == cs_pin)    set_cs->pin   = &IfxQspi0_SLSO3_P11_10_OUT;
            else if (SPI0_CS4_P11_11  == cs_pin)    set_cs->pin   = &IfxQspi0_SLSO4_P11_11_OUT;
            else if (SPI0_CS5_P11_2   == cs_pin)    set_cs->pin   = &IfxQspi0_SLSO5_P11_2_OUT;
            else if (SPI0_CS6_P20_10  == cs_pin)    set_cs->pin   = &IfxQspi0_SLSO6_P20_10_OUT;
            else if (SPI0_CS7_P33_5   == cs_pin)    set_cs->pin   = &IfxQspi0_SLSO7_P33_5_OUT;
            else if (SPI0_CS8_P20_6   == cs_pin)    set_cs->pin   = &IfxQspi0_SLSO8_P20_6_OUT;
            else if (SPI0_CS9_P20_3   == cs_pin)    set_cs->pin   = &IfxQspi0_SLSO9_P20_3_OUT;
            else if (SPI0_CS13_P15_0  == cs_pin)    set_cs->pin   = &IfxQspi0_SLSO13_P15_0_OUT;
            else    zf_assert(FALSE);

        }break;

        case SPI_1:
        {
            if      (SPI1_SCLK_P10_2  == sck_pin)   set_pin->sclk = &IfxQspi1_SCLK_P10_2_OUT;
            else if (SPI1_SCLK_P11_6  == sck_pin)   set_pin->sclk = &IfxQspi1_SCLK_P11_6_OUT;
            else    zf_assert(FALSE);

            if      (SPI1_MOSI_P10_1  == mosi_pin)  set_pin->mtsr = &IfxQspi1_MTSR_P10_1_OUT;
            else if (SPI1_MOSI_P10_3  == mosi_pin)  set_pin->mtsr = &IfxQspi1_MTSR_P10_3_OUT;
            else if (SPI1_MOSI_P11_9  == mosi_pin)  set_pin->mtsr = &IfxQspi1_MTSR_P11_9_OUT;
            else    zf_assert(FALSE);

            if      (SPI1_MISO_P10_1  == miso_pin)  set_pin->mrst = &IfxQspi1_MRSTA_P10_1_IN;
            else if (SPI1_MISO_P11_3  == miso_pin)  set_pin->mrst = &IfxQspi1_MRSTB_P11_3_IN;
            else    zf_assert(FALSE);

            if      (SPI1_CS0_P20_8   == cs_pin  ||
                         SPI_CS_NULL  == cs_pin)    set_cs->pin   = &IfxQspi1_SLSO0_P20_8_OUT;
            else if (SPI1_CS1_P20_9   == cs_pin )   set_cs->pin   = &IfxQspi1_SLSO1_P20_9_OUT;
            else if (SPI1_CS2_P20_13  == cs_pin )   set_cs->pin   = &IfxQspi1_SLSO2_P20_13_OUT;
            else if (SPI1_CS3_P11_10  == cs_pin )   set_cs->pin   = &IfxQspi1_SLSO3_P11_10_OUT;
            else if (SPI1_CS4_P11_11  == cs_pin )   set_cs->pin   = &IfxQspi1_SLSO4_P11_11_OUT;
            else if (SPI1_CS5_P11_2   == cs_pin )   set_cs->pin   = &IfxQspi1_SLSO5_P11_2_OUT;
            else if (SPI1_CS6_P33_10  == cs_pin )   set_cs->pin   = &IfxQspi1_SLSO6_P33_10_OUT;
            else if (SPI1_CS7_P33_5   == cs_pin )   set_cs->pin   = &IfxQspi1_SLSO7_P33_5_OUT;
            else if (SPI1_CS8_P10_4   == cs_pin )   set_cs->pin   = &IfxQspi1_SLSO8_P10_4_OUT;
            else if (SPI1_CS9_P10_5   == cs_pin )   set_cs->pin   = &IfxQspi1_SLSO9_P10_5_OUT;
            else    zf_assert(FALSE);
        }break;

        case SPI_2:
        {
            if      (SPI2_SCLK_P13_0  == sck_pin)   set_pin->sclk = &IfxQspi2_SCLKN_P13_0_OUT;
            else if (SPI2_SCLK_P13_1  == sck_pin)   set_pin->sclk = &IfxQspi2_SCLKP_P13_1_OUT;
            else if (SPI2_SCLK_P15_3  == sck_pin)   set_pin->sclk = &IfxQspi2_SCLK_P15_3_OUT;
            else if (SPI2_SCLK_P15_6  == sck_pin)   set_pin->sclk = &IfxQspi2_SCLK_P15_6_OUT;
            else if (SPI2_SCLK_P15_8  == sck_pin)   set_pin->sclk = &IfxQspi2_SCLK_P15_8_OUT;
            else    zf_assert(FALSE);

            if      (SPI2_MOSI_P13_3  == mosi_pin)  set_pin->mtsr = &IfxQspi2_MTSRP_P13_3_OUT;
            else if (SPI2_MOSI_P15_5  == mosi_pin)  set_pin->mtsr = &IfxQspi2_MTSR_P15_5_OUT;
            else if (SPI2_MOSI_P15_6  == mosi_pin)  set_pin->mtsr = &IfxQspi2_MTSR_P15_6_OUT;
            else    zf_assert(FALSE);

            if      (SPI2_MISO_P15_2  == miso_pin)  set_pin->mrst = &IfxQspi2_MRSTE_P15_2_IN;
            else if (SPI2_MISO_P15_4  == miso_pin)  set_pin->mrst = &IfxQspi2_MRSTA_P15_4_IN;
            else if (SPI2_MISO_P15_7  == miso_pin)  set_pin->mrst = &IfxQspi2_MRSTB_P15_7_IN;
            else if (SPI2_MISO_P21_2  == miso_pin)  set_pin->mrst = &IfxQspi2_MRSTCN_P21_2_IN;
            else if (SPI2_MISO_P21_3  == miso_pin)  set_pin->mrst = &IfxQspi2_MRSTCP_P21_3_IN;
            else    zf_assert(FALSE);

            if      (SPI2_CS0_P15_2   == cs_pin ||
                          SPI_CS_NULL == cs_pin)    set_cs->pin   = &IfxQspi2_SLSO0_P15_2_OUT;
            else if (SPI2_CS1_P14_2   == cs_pin)    set_cs->pin   = &IfxQspi2_SLSO1_P14_2_OUT;
            else if (SPI2_CS2_P14_6   == cs_pin)    set_cs->pin   = &IfxQspi2_SLSO2_P14_6_OUT;
            else if (SPI2_CS3_P14_3   == cs_pin)    set_cs->pin   = &IfxQspi2_SLSO3_P14_3_OUT;
            else if (SPI2_CS5_P15_1   == cs_pin)    set_cs->pin   = &IfxQspi2_SLSO5_P15_1_OUT;
            else if (SPI2_CS6_P33_13  == cs_pin)    set_cs->pin   = &IfxQspi2_SLSO6_P33_13_OUT;
            else if (SPI2_CS7_P20_10  == cs_pin)    set_cs->pin   = &IfxQspi2_SLSO7_P20_10_OUT;
            else if (SPI2_CS8_P20_6   == cs_pin)    set_cs->pin   = &IfxQspi2_SLSO8_P20_6_OUT;
            else if (SPI2_CS9_P20_3   == cs_pin)    set_cs->pin   = &IfxQspi2_SLSO9_P20_3_OUT;
            else    zf_assert(FALSE);
        }break;

        case SPI_3:
        {
            if      (SPI3_SCLK_P02_7  == sck_pin)   set_pin->sclk = &IfxQspi3_SCLK_P02_7_OUT;
            else if (SPI3_SCLK_P22_0  == sck_pin)   set_pin->sclk = &IfxQspi3_SCLKN_P22_0_OUT;
            else if (SPI3_SCLK_P22_1  == sck_pin)   set_pin->sclk = &IfxQspi3_SCLKP_P22_1_OUT;
            else if (SPI3_SCLK_P22_3  == sck_pin)   set_pin->sclk = &IfxQspi3_SCLK_P22_3_OUT;
            else if (SPI3_SCLK_P33_11 == sck_pin)   set_pin->sclk = &IfxQspi3_SCLK_P33_11_OUT;
            else    zf_assert(FALSE);

            if      (SPI3_MOSI_P02_6  == mosi_pin)  set_pin->mtsr = &IfxQspi3_MTSR_P02_6_OUT;
            else if (SPI3_MOSI_P10_6  == mosi_pin)  set_pin->mtsr = &IfxQspi3_MTSR_P10_6_OUT;
            else if (SPI3_MOSI_P22_0  == mosi_pin)  set_pin->mtsr = &IfxQspi3_MTSR_P22_0_OUT;
            else if (SPI3_MOSI_P22_3  == mosi_pin)  set_pin->mtsr = &IfxQspi3_MTSRP_P22_3_OUT;
            else if (SPI3_MOSI_P33_12 == mosi_pin)  set_pin->mtsr = &IfxQspi3_MTSR_P33_12_OUT;
            else    zf_assert(FALSE);

            if      (SPI3_MISO_P02_5  == miso_pin)  set_pin->mrst = &IfxQspi3_MRSTA_P02_5_IN;
            else if (SPI3_MISO_P22_1  == miso_pin)  set_pin->mrst = &IfxQspi3_MRSTE_P22_1_IN;
            else if (SPI3_MISO_P21_2  == miso_pin)  set_pin->mrst = &IfxQspi3_MRSTFN_P21_2_IN;
            else if (SPI3_MISO_P21_3  == miso_pin)  set_pin->mrst = &IfxQspi3_MRSTFP_P21_3_IN;
            else if (SPI3_MISO_P33_13 == miso_pin)  set_pin->mrst = &IfxQspi3_MRSTD_P33_13_IN;
            else    zf_assert(FALSE);

            if      (SPI3_CS0_P02_4   == cs_pin ||
                          SPI_CS_NULL == cs_pin)    set_cs->pin   = &IfxQspi3_SLSO0_P02_4_OUT;
            else if (SPI3_CS1_P02_0   == cs_pin)    set_cs->pin   = &IfxQspi3_SLSO1_P02_0_OUT;
            else if (SPI3_CS1_P33_9   == cs_pin)    set_cs->pin   = &IfxQspi3_SLSO1_P33_9_OUT;
            else if (SPI3_CS2_P02_1   == cs_pin)    set_cs->pin   = &IfxQspi3_SLSO2_P02_1_OUT;
            else if (SPI3_CS2_P33_8   == cs_pin)    set_cs->pin   = &IfxQspi3_SLSO2_P33_8_OUT;
            else if (SPI3_CS3_P02_2   == cs_pin)    set_cs->pin   = &IfxQspi3_SLSO3_P02_2_OUT;
            else if (SPI3_CS4_P02_3   == cs_pin)    set_cs->pin   = &IfxQspi3_SLSO4_P02_3_OUT;
            else if (SPI3_CS5_P02_8   == cs_pin)    set_cs->pin   = &IfxQspi3_SLSO5_P02_8_OUT;
            else if (SPI3_CS6_P00_8   == cs_pin)    set_cs->pin   = &IfxQspi3_SLSO6_P00_8_OUT;
            else if (SPI3_CS7_P00_9   == cs_pin)    set_cs->pin   = &IfxQspi3_SLSO7_P00_9_OUT;
            else if (SPI3_CS7_P33_7   == cs_pin)    set_cs->pin   = &IfxQspi3_SLSO7_P33_7_OUT;
            else if (SPI3_CS8_P10_5   == cs_pin)    set_cs->pin   = &IfxQspi3_SLSO8_P10_5_OUT;
            else if (SPI3_CS11_P33_10 == cs_pin)    set_cs->pin   = &IfxQspi3_SLSO11_P33_10_OUT;
            else if (SPI3_CS12_P22_2  == cs_pin)    set_cs->pin   = &IfxQspi3_SLSO12_P22_2_OUT;
            else if (SPI3_CS13_P23_1  == cs_pin)    set_cs->pin   = &IfxQspi3_SLSO13_P23_1_OUT;
            else    zf_assert(FALSE);
        }break;
    }
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介       SPI清除接收缓存区数据
// 参数说明       spi_n           SPI 模块号 参照 zf_driver_spi.h 内 spi_index_enum 枚举体定义
// 返回参数       void
// 使用示例       spi_clear_fifo(SPI_1);
// 备注信息       内部调用，用户无需关心
//-------------------------------------------------------------------------------------------------------------------
static void spi_clear_fifo (Ifx_QSPI *moudle)
{
    uint32 fifo_num;
    // 将之前fifo中的数据全读读取出来
    fifo_num = moudle->STATUS.B.RXFIFOLEVEL;
    while(fifo_num --)
    {
        (uint8)IfxQspi_readReceiveFifo(moudle);
    }

}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介       SPI 接口写 8bit 数据
// 参数说明       spi_n           SPI 模块号 参照 zf_driver_spi.h 内 spi_index_enum 枚举体定义
// 参数说明       data            数据
// 返回参数       void
// 使用示例       spi_write_8bit(SPI_1, 0x11);
// 备注信息
//-------------------------------------------------------------------------------------------------------------------
void spi_write_8bit (spi_index_enum spi_n, const uint8 data)
{
    volatile Ifx_QSPI *moudle;                                  // 定义SPI模块对象

    moudle = IfxQspi_getAddress((IfxQspi_Index)spi_n);          // 获取模块地址

    IfxQspi_writeBasicConfigurationEndStream(moudle, bacon.U);  // 发送数据后CS拉高

    IfxQspi_writeTransmitFifo(moudle, data);                    // 将发送的数据写入缓冲区

    while(moudle->STATUS.B.TXFIFOLEVEL != 0);                   // 等待发送完毕

    while(moudle->STATUS.B.PT1F == 0);                          // 等待结束标志位

    IfxQspi_clearAllEventFlags(moudle);                         // 清除发送结束标志位
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介       SPI 接口写 8bit 数组
// 参数说明       spi_n           SPI 模块号 参照 zf_driver_spi.h 内 spi_index_enum 枚举体定义
// 参数说明       *data           数据存放缓冲区
// 参数说明       len             缓冲区长度
// 返回参数       void
// 使用示例       spi_write_8bit_array(SPI_1, data, 64);
// 备注信息
//-------------------------------------------------------------------------------------------------------------------
void spi_write_8bit_array (spi_index_enum spi_n, const uint8 *data, uint32 len)
{
    volatile Ifx_QSPI *moudle;                                  // 定义SPI模块对象

    moudle = IfxQspi_getAddress((IfxQspi_Index)spi_n);          // 获取模块地址

    IfxQspi_writeBasicConfigurationBeginStream(moudle, bacon.U);// 发送数据后CS继续保持为低

    do
    {
        if(len == 1)
        {
            IfxQspi_writeBasicConfigurationEndStream(moudle, bacon.U);  // 发送数据后CS拉高
        }

        IfxQspi_writeTransmitFifo(moudle, *data ++);                    // 将发送的数据写入缓冲区

        while(moudle->STATUS.B.TXFIFOLEVEL != 0);               // 等待发送完毕
    }while(-- len);

    while(moudle->STATUS.B.PT1F == 0);                          // 等待结束标志位

    IfxQspi_clearAllEventFlags(moudle);                         // 清除发送结束标志位
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介       SPI 接口写 16bit 数据
// 参数说明       spi_n           SPI 模块号 参照 zf_driver_spi.h 内 spi_index_enum 枚举体定义
// 参数说明       data            数据
// 返回参数       void
// 使用示例       spi_write_16bit(SPI_1, 0x1101);
// 备注信息
//-------------------------------------------------------------------------------------------------------------------
void spi_write_16bit (spi_index_enum spi_n, const uint16 data)
{
    volatile Ifx_QSPI *moudle;                                  // 定义SPI模块对象

    moudle = IfxQspi_getAddress((IfxQspi_Index)spi_n);          // 获取模块地址

    IfxQspi_writeBasicConfigurationBeginStream(moudle, bacon.U);// 发送数据后CS继续保持为低

    IfxQspi_writeTransmitFifo(moudle, (uint8)((data & 0xFF00) >> 8)); // 将发送的数据写入缓冲区

    while(moudle->STATUS.B.TXFIFOLEVEL != 0);                   // 等待发送完毕

    IfxQspi_writeBasicConfigurationEndStream(moudle, bacon.U);  // 发送数据后CS拉高

    IfxQspi_writeTransmitFifo(moudle, (uint8)(data & 0x00FF));  // 将发送的数据写入缓冲区

    while(moudle->STATUS.B.PT1F == 0);                          // 等待结束标志位

    IfxQspi_clearAllEventFlags(moudle);                         // 清除发送结束标志位
}


//-------------------------------------------------------------------------------------------------------------------
// 函数简介       SPI 接口写 16bit 数组
// 参数说明       spi_n           SPI 模块号 参照 zf_driver_spi.h 内 spi_index_enum 枚举体定义
// 参数说明       *data           数据存放缓冲区
// 参数说明       len             缓冲区长度
// 返回参数       void
// 使用示例       spi_write_16bit_array(SPI_1, data, 64);
// 备注信息
//-------------------------------------------------------------------------------------------------------------------
void spi_write_16bit_array (spi_index_enum spi_n, const uint16 *data, uint32 len)
{
    volatile Ifx_QSPI *moudle;                                      // 定义SPI模块对象

    moudle = IfxQspi_getAddress((IfxQspi_Index)spi_n);              // 获取模块地址

    IfxQspi_writeBasicConfigurationBeginStream(moudle, bacon.U);    // 发送数据后CS继续保持为低

    do
    {

        IfxQspi_writeTransmitFifo(moudle, (uint8)((*data & 0xFF00) >> 8));  // 将发送的数据写入缓冲区

        while(moudle->STATUS.B.TXFIFOLEVEL != 0);                   // 等待发送完毕

        if(len == 1)                                                // 最后一个数据
        {
            IfxQspi_writeBasicConfigurationEndStream(moudle, bacon.U);      // 发送数据后CS拉高
        }

        IfxQspi_writeTransmitFifo(moudle, (uint8)(*data++ & 0x00FF));       // 将发送的数据写入缓冲区

        while(moudle->STATUS.B.TXFIFOLEVEL != 0);                   // 等待发送完毕

    }while(-- len);

    while(moudle->STATUS.B.PT1F == 0);                              // 等待结束标志位

    IfxQspi_clearAllEventFlags(moudle);                             // 清除发送结束标志位
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介       SPI 接口向传感器的寄存器写 8bit 数据
// 参数说明       spi_n           SPI 模块号 参照 zf_driver_spi.h 内 spi_index_enum 枚举体定义
// 参数说明       register_name   寄存器地址
// 参数说明       data            数据
// 返回参数       void
// 使用示例        spi_write_8bit_register(SPI_1, 0x11, 0x01);
// 备注信息
//-------------------------------------------------------------------------------------------------------------------
void spi_write_8bit_register (spi_index_enum spi_n, const uint8 register_name, const uint8 data)
{
    volatile Ifx_QSPI *moudle;                                  // 定义SPI模块对象

    moudle = IfxQspi_getAddress((IfxQspi_Index)spi_n);          // 获取模块地址

    IfxQspi_writeBasicConfigurationBeginStream(moudle, bacon.U);    // 发送数据后CS继续保持为低

    spi_clear_fifo(moudle);                                         // 清除接收缓存区

    IfxQspi_writeTransmitFifo(moudle, register_name);           // 将发送的数据写入缓冲区

    while(moudle->STATUS.B.TXFIFOLEVEL != 0);                   // 等待发送完毕

    IfxQspi_writeBasicConfigurationEndStream(moudle, bacon.U);  // 发送数据后CS拉高

    IfxQspi_writeTransmitFifo(moudle, data);                    // 将发送的数据写入缓冲区

    while(moudle->STATUS.B.TXFIFOLEVEL != 0);                   // 等待发送完毕

    while(moudle->STATUS.B.PT1F == 0);                          // 等待结束标志位

    IfxQspi_clearAllEventFlags(moudle);                         // 清除发送结束标志位
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介       SPI 接口向传感器的寄存器写 8bit 数组
// 参数说明       spi_n           SPI 模块号 参照 zf_driver_spi.h 内 spi_index_enum 枚举体定义
// 参数说明       register_name   寄存器地址
// 参数说明       *data           数据存放缓冲区
// 参数说明       len             缓冲区长度
// 返回参数       void
// 使用示例       spi_write_8bit_registers(SPI_1, 0x11, data, 32);
// 备注信息
//-------------------------------------------------------------------------------------------------------------------
void spi_write_8bit_registers (spi_index_enum spi_n, const uint8 register_name, const uint8 *data, uint32 len)
{
    volatile Ifx_QSPI *moudle;                                  // 定义SPI模块对象

    moudle = IfxQspi_getAddress((IfxQspi_Index)spi_n);          // 获取模块地址

    IfxQspi_writeBasicConfigurationBeginStream(moudle, bacon.U);        // 发送数据后CS继续保持为低

    IfxQspi_writeTransmitFifo(moudle, register_name);           // 将发送的数据写入缓冲区

    while(moudle->STATUS.B.TXFIFOLEVEL != 0);                   // 等待发送完毕

    do
    {
        if(len == 1)                                            // 最后一个数据
        {
            IfxQspi_writeBasicConfigurationEndStream(moudle, bacon.U);  // 发送数据后CS拉高
        }

        IfxQspi_writeTransmitFifo(moudle, *data ++);            // 将发送的数据写入缓冲区

        while(moudle->STATUS.B.TXFIFOLEVEL != 0);               // 等待发送完毕

    }while(-- len);

    while(moudle->STATUS.B.PT1F == 0);                          // 等待结束标志位

    IfxQspi_clearAllEventFlags(moudle);                         // 清除发送结束标志位
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介       SPI 接口向传感器的寄存器写 16bit 数据
// 参数说明       spi_n           SPI 模块号 参照 zf_driver_spi.h 内 spi_index_enum 枚举体定义
// 参数说明       register_name   寄存器地址
// 参数说明       data            数据
// 返回参数       void
// 使用示例       spi_write_16bit_register(SPI_1, 0x1011, 0x0101);
// 备注信息
//-------------------------------------------------------------------------------------------------------------------
void spi_write_16bit_register (spi_index_enum spi_n, const uint16 register_name, const uint16 data)
{
    volatile Ifx_QSPI *moudle;                                  // 定义SPI模块对象

    moudle = IfxQspi_getAddress((IfxQspi_Index)spi_n);          // 获取模块地址

    IfxQspi_writeBasicConfigurationBeginStream(moudle, bacon.U);// 发送数据后CS继续保持为低

    IfxQspi_writeTransmitFifo(moudle, (uint8)((register_name & 0xFF00) >> 8));  // 将发送的数据写入缓冲区

    while(moudle->STATUS.B.TXFIFOLEVEL != 0);                   // 等待发送完毕

    IfxQspi_writeTransmitFifo(moudle, (uint8)(register_name & 0x00FF));         // 将发送的数据写入缓冲区

    while(moudle->STATUS.B.TXFIFOLEVEL != 0);                   // 等待发送完毕

    IfxQspi_writeTransmitFifo(moudle, (uint8)((data & 0xFF00) >> 8));           // 将发送的数据写入缓冲区

    while(moudle->STATUS.B.TXFIFOLEVEL != 0);                   // 等待发送完毕

    IfxQspi_writeBasicConfigurationEndStream(moudle, bacon.U);  // 发送数据后CS拉高

    IfxQspi_writeTransmitFifo(moudle, (uint8)(data & 0x00FF));  // 将发送的数据写入缓冲区

    while(moudle->STATUS.B.TXFIFOLEVEL != 0);                   // 等待发送完毕

    while(moudle->STATUS.B.PT1F == 0);                          // 等待结束标志位

    IfxQspi_clearAllEventFlags(moudle);                         // 清除发送结束标志位
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介       SPI 接口向传感器的寄存器写 16bit 数组
// 参数说明       spi_n           SPI 模块号 参照 zf_driver_spi.h 内 spi_index_enum 枚举体定义
// 参数说明       register_name   寄存器地址
// 参数说明       *data           数据存放缓冲区
// 参数说明       len             缓冲区长度
// 返回参数       void
// 使用示例       spi_write_16bit_registers(SPI_1, 0x1011, data, 32);
// 备注信息
//-------------------------------------------------------------------------------------------------------------------
void spi_write_16bit_registers (spi_index_enum spi_n, const uint16 register_name, const uint16 *data, uint32 len)
{
    volatile Ifx_QSPI *moudle;                                  // 定义SPI模块对象

    moudle = IfxQspi_getAddress((IfxQspi_Index)spi_n);          // 获取模块地址

    IfxQspi_writeBasicConfigurationBeginStream(moudle, bacon.U);// 发送数据后CS继续保持为低

    IfxQspi_writeTransmitFifo(moudle, (uint8)((register_name & 0xFF00) >> 8));  // 将发送的数据写入缓冲区

    while(moudle->STATUS.B.TXFIFOLEVEL != 0);                   // 等待发送完毕

    IfxQspi_writeTransmitFifo(moudle, (uint8)(register_name & 0x00FF));         // 将发送的数据写入缓冲区

    while(moudle->STATUS.B.TXFIFOLEVEL != 0);                   // 等待发送完毕

    do
    {
        IfxQspi_writeTransmitFifo(moudle, (uint8)((*data & 0xFF00) >> 8));      // 将发送的数据写入缓冲区

        while(moudle->STATUS.B.TXFIFOLEVEL != 0);               // 等待发送完毕

        if(len == 1)                                            // 最后一个数据
        {
           IfxQspi_writeBasicConfigurationEndStream(moudle, bacon.U);           // 发送数据后CS拉高
        }

        IfxQspi_writeTransmitFifo(moudle, (uint8)(*data ++ & 0x00FF));          // 将发送的数据写入缓冲区

        while(moudle->STATUS.B.TXFIFOLEVEL != 0);               // 等待发送完毕

    }while(-- len);

    while(moudle->STATUS.B.PT1F == 0);                          // 等待结束标志位

    IfxQspi_clearAllEventFlags(moudle);                         // 清除发送结束标志位
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介       SPI 接口读 8bit 数据
// 参数说明       spi_n           SPI 模块号 参照 zf_driver_spi.h 内 spi_index_enum 枚举体定义
// 参数说明       register_name   寄存器地址
// 返回参数       uint8           数据
// 使用示例       spi_read_8bit(SPI_1);
// 备注信息
//-------------------------------------------------------------------------------------------------------------------
uint8 spi_read_8bit (spi_index_enum spi_n)
{
    volatile Ifx_QSPI *moudle;                                  // 定义SPI模块对象

    moudle = IfxQspi_getAddress((IfxQspi_Index)spi_n);          // 获取模块地址

    IfxQspi_writeBasicConfigurationEndStream(moudle, bacon.U);  // 发送数据后CS拉高

    spi_clear_fifo(moudle);                                     // 清除接收缓存区

    IfxQspi_writeTransmitFifo(moudle, 0);                       // 将发送的数据写入缓冲区

    while(moudle->STATUS.B.RXFIFOLEVEL == 0);                   // 等待接收完毕

    while(moudle->STATUS.B.PT1F == 0);                          // 等待结束标志位

    IfxQspi_clearAllEventFlags(moudle);                         // 清除发送结束标志位

    return (uint8)IfxQspi_readReceiveFifo(moudle);              // 返回接收到的数据
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介       SPI 接口读 8bit 数组
// 参数说明       spi_n           SPI 模块号 参照 zf_driver_spi.h 内 spi_index_enum 枚举体定义
// 参数说明       *data           数据存放缓冲区
// 参数说明       len             发送缓冲区长度
// 返回参数       void
// 使用示例       spi_read_8bit_array(SPI_1, data, 64);
// 备注信息
//-------------------------------------------------------------------------------------------------------------------
void spi_read_8bit_array (spi_index_enum spi_n, uint8 *data, uint32 len)
{
    volatile Ifx_QSPI *moudle;                                  // 定义SPI模块对象

    moudle = IfxQspi_getAddress((IfxQspi_Index)spi_n);          // 获取模块地址

    IfxQspi_writeBasicConfigurationBeginStream(moudle, bacon.U);// 发送数据后CS继续保持为低

    spi_clear_fifo(moudle);                                     // 清除接收缓存区

    do
    {
        if(len == 1)                                            // 最后一个数据
        {
           IfxQspi_writeBasicConfigurationEndStream(moudle, bacon.U);           // 发送数据后CS拉高
        }

        IfxQspi_writeTransmitFifo(moudle, 0);                   // 将发送的数据写入缓冲区

        while(moudle->STATUS.B.RXFIFOLEVEL == 0);               // 等待接收完毕

        *data++ = (uint8)IfxQspi_readReceiveFifo(moudle);       // 保存接收到的数据
    }while(-- len);

    while(moudle->STATUS.B.PT1F == 0);                          // 等待结束标志位

    IfxQspi_clearAllEventFlags(moudle);                         // 清除发送结束标志位
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介       SPI 接口读 16bit 数据
// 参数说明       spi_n           SPI 模块号 参照 zf_driver_spi.h 内 spi_index_enum 枚举体定义
// 参数说明       register_name   寄存器地址
// 返回参数       uint16          数据
// 使用示例       spi_read_16bit(SPI_1);
// 备注信息
//-------------------------------------------------------------------------------------------------------------------
uint16 spi_read_16bit (spi_index_enum spi_n)
{
    uint16 data = 0;

    volatile Ifx_QSPI *moudle;                                  // 定义SPI模块对象

    moudle = IfxQspi_getAddress((IfxQspi_Index)spi_n);          // 获取模块地址

    IfxQspi_writeBasicConfigurationBeginStream(moudle, bacon.U);// 发送数据后CS继续保持为低

    spi_clear_fifo(moudle);                                     // 清除接收缓存区

    IfxQspi_writeTransmitFifo(moudle, 0);                       // 将发送的数据写入缓冲区

    while(moudle->STATUS.B.RXFIFOLEVEL == 0);                   // 等待接收完毕

    data = (uint16)IfxQspi_readReceiveFifo(moudle);             // 保存接收到的数据

    IfxQspi_writeBasicConfigurationEndStream(moudle, bacon.U);  // 发送数据后CS拉高

    IfxQspi_writeTransmitFifo(moudle, 0);                       // 将发送的数据写入缓冲区

    while(moudle->STATUS.B.RXFIFOLEVEL == 0);                   // 等待接收完毕

    data = ((data << 8) | (uint16)IfxQspi_readReceiveFifo(moudle));             // 拟合接收到的数据

    while(moudle->STATUS.B.PT1F == 0);                          // 等待结束标志位

    IfxQspi_clearAllEventFlags(moudle);                         // 清除发送结束标志位

    return data;                                                // 返回接收到的数据
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介       SPI 接口读 16bit 数组
// 参数说明       spi_n           SPI 模块号 参照 zf_driver_spi.h 内 spi_index_enum 枚举体定义
// 参数说明       *data           数据存放缓冲区
// 参数说明       len             发送缓冲区长度
// 返回参数       void
// 使用示例       spi_read_16bit_array(SPI_1, data, 64);
// 备注信息
//-------------------------------------------------------------------------------------------------------------------
void spi_read_16bit_array (spi_index_enum spi_n, uint16 *data, uint32 len)
{
    volatile Ifx_QSPI *moudle;                                      // 定义SPI模块对象

    moudle = IfxQspi_getAddress((IfxQspi_Index)spi_n);              // 获取模块地址

    IfxQspi_writeBasicConfigurationBeginStream(moudle, bacon.U);    // 发送数据后CS继续保持为低

    spi_clear_fifo(moudle);                                         // 清除接收缓存区

    do
    {
        IfxQspi_writeTransmitFifo(moudle, 0);                       // 将发送的数据写入缓冲区

        while(moudle->STATUS.B.RXFIFOLEVEL == 0);                   // 等待接收完毕

        *data = (uint16)IfxQspi_readReceiveFifo(moudle);            // 保存接收到的数据

        if(len == 1)
        {
            IfxQspi_writeBasicConfigurationEndStream(moudle, bacon.U);              // 发送数据后CS拉高
        }

        IfxQspi_writeTransmitFifo(moudle, 0);                       // 将发送的数据写入缓冲区

        while(moudle->STATUS.B.RXFIFOLEVEL == 0);                   // 等待接收完毕

        *data = ((*data << 8) | (uint16)IfxQspi_readReceiveFifo(moudle));            // 拟合接收到的数据

        data ++;

    }while(-- len);

    while(moudle->STATUS.B.PT1F == 0);                              // 等待结束标志位

    IfxQspi_clearAllEventFlags(moudle);                             // 清除发送结束标志位
}


//-------------------------------------------------------------------------------------------------------------------
// 函数简介       SPI 接口从传感器的寄存器读 8bit 数据
// 参数说明       spi_n           SPI 模块号 参照 zf_driver_spi.h 内 spi_index_enum 枚举体定义
// 参数说明       register_name   寄存器地址
// 返回参数       uint8           数据
// 使用示例       spi_read_8bit_register(SPI_1, 0x11);
// 备注信息
//-------------------------------------------------------------------------------------------------------------------
uint8 spi_read_8bit_register (spi_index_enum spi_n, const uint8 register_name)
{
    volatile Ifx_QSPI *moudle;                                      // 定义SPI模块对象

    moudle = IfxQspi_getAddress((IfxQspi_Index)spi_n);              // 获取模块地址

    spi_clear_fifo(moudle);                                         // 清除接收缓存区

    IfxQspi_writeBasicConfigurationBeginStream(moudle, bacon.U);    // 发送数据后CS继续保持为低

    IfxQspi_writeTransmitFifo(moudle, register_name);               // 将发送的数据写入缓冲区

    while(moudle->STATUS.B.RXFIFOLEVEL == 0);                       // 等待接收完毕

    spi_clear_fifo(moudle);                                         // 清除接收缓存区

    IfxQspi_writeBasicConfigurationEndStream(moudle, bacon.U);      // 发送数据后CS拉高

    IfxQspi_writeTransmitFifo(moudle, 0);                           // 将发送的数据写入缓冲区

    while(moudle->STATUS.B.RXFIFOLEVEL == 0);                       // 等待接收完毕

    while(moudle->STATUS.B.PT1F == 0);                              // 等待结束标志位

    IfxQspi_clearAllEventFlags(moudle);                             // 清除发送结束标志位

    return (uint8)IfxQspi_readReceiveFifo(moudle);                  // 返回接收到的数据
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介       SPI 接口从传感器的寄存器读 8bit 数组
// 参数说明       spi_n           SPI 模块号 参照 zf_driver_spi.h 内 spi_index_enum 枚举体定义
// 参数说明       register_name   寄存器地址
// 参数说明       *data           数据存放缓冲区
// 参数说明       len             发送缓冲区长度
// 返回参数       void
// 使用示例       spi_read_8bit_registers(SPI_1, 0x11, data, 32);
// 备注信息
//-------------------------------------------------------------------------------------------------------------------
void spi_read_8bit_registers (spi_index_enum spi_n, const uint8 register_name, uint8 *data, uint32 len)
{
    volatile Ifx_QSPI *moudle;                                      // 定义SPI模块对象

    moudle = IfxQspi_getAddress((IfxQspi_Index)spi_n);              // 获取模块地址

    spi_clear_fifo(moudle);                                         // 清除接收缓存区

    IfxQspi_writeBasicConfigurationBeginStream(moudle, bacon.U);    // 发送数据后CS继续保持为低

    IfxQspi_writeTransmitFifo(moudle, register_name);               // 将发送的数据写入缓冲区

    while(moudle->STATUS.B.RXFIFOLEVEL == 0);                       // 等待接收完毕

    spi_clear_fifo(moudle);                                         // 清除接收缓存区

    do
    {
        if(len == 1)
        {
            IfxQspi_writeBasicConfigurationEndStream(moudle, bacon.U);              // 发送数据后CS拉高
        }

        IfxQspi_writeTransmitFifo(moudle, 0);                       // 将发送的数据写入缓冲区

        while(moudle->STATUS.B.RXFIFOLEVEL == 0);                   // 等待接收完毕

        *data ++ = (uint8)IfxQspi_readReceiveFifo(moudle);          // 保存接收到的数据

    }while(-- len);

    while(moudle->STATUS.B.PT1F == 0);                              // 等待结束标志位

    IfxQspi_clearAllEventFlags(moudle);                             // 清除发送结束标志位

}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介       SPI 接口从传感器的寄存器读 16bit 数据
// 参数说明       spi_n           SPI 模块号 参照 zf_driver_spi.h 内 spi_index_enum 枚举体定义
// 参数说明       register_name   寄存器地址
// 返回参数       uint16          数据
// 使用示例       spi_read_16bit_register(SPI_1, 0x1011);
// 备注信息
//-------------------------------------------------------------------------------------------------------------------
uint16 spi_read_16bit_register (spi_index_enum spi_n, const uint16 register_name)
{
    uint16 data = 0;

    volatile Ifx_QSPI *moudle;                                      // 定义SPI模块对象

    moudle = IfxQspi_getAddress((IfxQspi_Index)spi_n);              // 获取模块地址

    spi_clear_fifo(moudle);                                         // 清除接收缓存区

    IfxQspi_writeBasicConfigurationBeginStream(moudle, bacon.U);    // 发送数据后CS继续保持为低

    IfxQspi_writeTransmitFifo(moudle, (uint8)((register_name & 0xFF00) >> 8));      // 将发送的数据写入缓冲区

    while(moudle->STATUS.B.RXFIFOLEVEL == 0);                       // 等待接收完毕

    IfxQspi_writeTransmitFifo(moudle, (uint8)(register_name & 0x00FF));             // 将发送的数据写入缓冲区

    while(moudle->STATUS.B.RXFIFOLEVEL == 0);                       // 等待接收完毕

    spi_clear_fifo(moudle);                                         // 清除接收缓存区

    IfxQspi_writeTransmitFifo(moudle, 0);                           // 将发送的数据写入缓冲区

    while(moudle->STATUS.B.RXFIFOLEVEL == 0);                       // 等待接收完毕

    data |= (uint8)IfxQspi_readReceiveFifo(moudle);                 // 保存接收到的数据

    IfxQspi_writeBasicConfigurationEndStream(moudle, bacon.U);      // 发送数据后CS拉高

    IfxQspi_writeTransmitFifo(moudle, 0);                           // 将发送的数据写入缓冲区

    while(moudle->STATUS.B.RXFIFOLEVEL == 0);                       // 等待接收完毕

    data = ((data << 8) | (uint16)IfxQspi_readReceiveFifo(moudle)); // 保存接收到的数据

    while(moudle->STATUS.B.PT1F == 0);                              // 等待结束标志位

    IfxQspi_clearAllEventFlags(moudle);                             // 清除发送结束标志位

    return data;
}


//-------------------------------------------------------------------------------------------------------------------
// 函数简介       SPI 接口从传感器的寄存器读 16bit 数组
// 参数说明       spi_n           SPI 模块号 参照 zf_driver_spi.h 内 spi_index_enum 枚举体定义
// 参数说明       register_name   寄存器地址
// 参数说明       *data           数据存放缓冲区
// 参数说明       len             发送缓冲区长度
// 返回参数       void
// 使用示例       spi_read_16bit_registers(SPI_1, 0x1101, data, 32);
// 备注信息
//-------------------------------------------------------------------------------------------------------------------
void spi_read_16bit_registers (spi_index_enum spi_n, const uint16 register_name, uint16 *data, uint32 len)
{
    volatile Ifx_QSPI *moudle;                                      // 定义SPI模块对象

    moudle = IfxQspi_getAddress((IfxQspi_Index)spi_n);              // 获取模块地址

    spi_clear_fifo(moudle);                                         // 清除接收缓存区

    IfxQspi_writeBasicConfigurationBeginStream(moudle, bacon.U);    // 发送数据后CS继续保持为低

    IfxQspi_writeTransmitFifo(moudle, (uint8)((register_name & 0xFF00) >> 8));      // 将发送的数据写入缓冲区

    while(moudle->STATUS.B.RXFIFOLEVEL == 0);                       // 等待接收完毕

    IfxQspi_writeTransmitFifo(moudle, (uint8)(register_name & 0x00FF));             // 将发送的数据写入缓冲区

    while(moudle->STATUS.B.RXFIFOLEVEL == 0);                       // 等待接收完毕

    spi_clear_fifo(moudle);                                         // 清除接收缓存区

    do
    {
        IfxQspi_writeTransmitFifo(moudle, 0);                       // 将发送的数据写入缓冲区

        while(moudle->STATUS.B.RXFIFOLEVEL == 0);                   // 等待接收完毕

        *data |= (uint8)IfxQspi_readReceiveFifo(moudle);            // 保存接收到的数据

        if(len == 1)
        {
            IfxQspi_writeBasicConfigurationEndStream(moudle, bacon.U);              // 发送数据后CS拉高
        }

        IfxQspi_writeTransmitFifo(moudle, 0);                       // 将发送的数据写入缓冲区

        while(moudle->STATUS.B.RXFIFOLEVEL == 0);                   // 等待接收完毕

        *data = ((*data << 8) | (uint16)IfxQspi_readReceiveFifo(moudle));             // 保存接收到的数据

        data ++;
    }while(-- len);

    while(moudle->STATUS.B.PT1F == 0);                              // 等待结束标志位

    IfxQspi_clearAllEventFlags(moudle);                             // 清除发送结束标志位
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介       SPI 8bit 数据传输 发送与接收数据是同时进行的
// 参数说明       spi_n           SPI 模块号 参照 zf_driver_spi.h 内 spi_index_enum 枚举体定义
// 参数说明       write_buffer    发送的数据缓冲区地址
// 参数说明       read_buffer     发送数据时接收到的数据的存储地址(不需要接收则传 NULL)
// 参数说明       len             缓冲区长度
// 返回参数       void
// 使用示例       spi_transfer_8bit(SPI_1, buf, buf, 1);
// 备注信息
//-------------------------------------------------------------------------------------------------------------------
void spi_transfer_8bit (spi_index_enum spi_n, const uint8 *write_buffer, uint8 *read_buffer, uint32 len)
{
    volatile Ifx_QSPI *moudle;                                      // 定义SPI模块对象

    moudle = IfxQspi_getAddress((IfxQspi_Index)spi_n);              // 获取模块地址

    IfxQspi_writeBasicConfigurationBeginStream(moudle, bacon.U);    // 发送数据后CS继续保持为低

    spi_clear_fifo(moudle);                                         // 清除接收缓存区

    do
    {
        if(len == 1)
        {
            IfxQspi_writeBasicConfigurationEndStream(moudle, bacon.U);              // 发送数据后CS拉高
        }

        IfxQspi_writeTransmitFifo(moudle, *write_buffer ++);                        // 将发送的数据写入缓冲区

        if(read_buffer != NULL)
        {
            while(moudle->STATUS.B.RXFIFOLEVEL == 0);               // 等待接收完毕
            *read_buffer ++ = (uint8)IfxQspi_readReceiveFifo(moudle);               // 保存接收到的数据
        }
        else
        {
            while(moudle->STATUS.B.TXFIFOLEVEL != 0);               // 等待发送完毕
        }
    }while(-- len);

    while(moudle->STATUS.B.PT1F == 0);                              // 等待结束标志位

    IfxQspi_clearAllEventFlags(moudle);                             // 清除发送结束标志位
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介       SPI 16bit 数据传输 发送与接收数据是同时进行的
// 参数说明       spi_n           SPI 模块号 参照 zf_driver_spi.h 内 spi_index_enum 枚举体定义
// 参数说明       write_buffer    发送的数据缓冲区地址
// 参数说明       read_buffer     发送数据时接收到的数据的存储地址(不需要接收则传 NULL)
// 参数说明       len             缓冲区长度
// 返回参数       void
// 使用示例       spi_transfer_16bit(SPI_1, buf, buf, 1);
// 备注信息
//-------------------------------------------------------------------------------------------------------------------
void spi_transfer_16bit (spi_index_enum spi_n, const uint16 *write_buffer, uint16 *read_buffer, uint32 len)
{
    volatile Ifx_QSPI *moudle;                                      // 定义SPI模块对象

    moudle = IfxQspi_getAddress((IfxQspi_Index)spi_n);              // 获取模块地址

    IfxQspi_writeBasicConfigurationBeginStream(moudle, bacon.U);    // 发送数据后CS继续保持为低

    spi_clear_fifo(moudle);                                         // 清除接收缓存区

    do
    {
        IfxQspi_writeTransmitFifo(moudle, (uint8)(*write_buffer & 0xFF00) >> 8);        // 将发送的数据写入缓冲区

        if(read_buffer != NULL)
        {
            while(moudle->STATUS.B.RXFIFOLEVEL == 0);               // 等待接收完毕
            *read_buffer = ((uint16)IfxQspi_readReceiveFifo(moudle) & 0x00FF);          // 保存接收到的数据
        }
        else
        {
            while(moudle->STATUS.B.TXFIFOLEVEL != 0);               // 等待发送完毕
        }

        if(len == 1)
        {
            IfxQspi_writeBasicConfigurationEndStream(moudle, bacon.U);                  // 发送数据后CS拉高
        }

        IfxQspi_writeTransmitFifo(moudle, (uint8)(*write_buffer & 0x00FF));             // 将发送的数据写入缓冲区

        if(read_buffer != NULL)
        {
            while(moudle->STATUS.B.RXFIFOLEVEL == 0);               // 等待接收完毕
            *read_buffer = (*read_buffer << 8) | (uint16)IfxQspi_readReceiveFifo(moudle);// 保存接收到的数据
        }
        else
        {
            while(moudle->STATUS.B.TXFIFOLEVEL != 0);               // 等待发送完毕
        }

        write_buffer ++;
        read_buffer ++;
    }while(-- len);

    while(moudle->STATUS.B.PT1F == 0);                              // 等待结束标志位

    IfxQspi_clearAllEventFlags(moudle);                             // 清除发送结束标志位
}

//-------------------------------------------------------------------------------------------------------------------
//  函数简介      SPI初始化
//  参数说明      spi_n           选择SPI模块(SPI_1-SPI_4)
//  参数说明      cs_pin          选择SPI片选引脚
//  参数说明      sck_pin         选择SPI时钟引脚
//  参数说明      mosi_pin        选择SPI MOSI引脚
//  参数说明      miso_pin        选择SPI MISO引脚
//  参数说明      mode            SPI模式 0：CPOL=0 CPHA=0    1：CPOL=0 CPHA=1   2：CPOL=1 CPHA=0   3：CPOL=1 CPHA=1  // 具体细节可自行查阅资料
//  参数说明      baud            设置SPI的波特率
//  返回参数      void
//  使用示例      spi_init(SPI_2, SPI2_SCLK_P15_3, SPI2_MOSI_P15_5, SPI2_MISO_P15_4, SPI2_CS0_P15_2, 0, 1*1000*1000); // 硬件SPI初始化  模式0 波特率为1Mhz
//  备注信息
//-------------------------------------------------------------------------------------------------------------------
void spi_init (spi_index_enum spi_n, spi_mode_enum mode, uint32 baud, spi_sck_pin_enum sck_pin, spi_mosi_pin_enum mosi_pin, spi_miso_pin_enum miso_pin, spi_cs_pin_enum cs_pin)
{
    IfxQspi_SpiMaster_Config MasterConfig;
    IfxQspi_SpiMaster MasterHandle;
    IfxQspi_SpiMaster_Channel MasterChHandle;
    IfxQspi_SpiMaster_Pins MasterPins;
    IfxQspi_SpiMaster_Output SlsoPin;
    volatile Ifx_QSPI *moudle;

    // 检查引脚是否正确
    // 如果从此处进入断言，则说明初始化SPI时，模块号和引脚并不是同一模块
    zf_assert(spi_n == (sck_pin / 100));
    zf_assert(spi_n == (mosi_pin / 100));
    zf_assert(spi_n == (miso_pin / 100));
    if(SPI_CS_NULL != cs_pin)
    {
        zf_assert(spi_n == (cs_pin / 100));
    }
    spi_cs_pin  = cs_pin;

    moudle = IfxQspi_getAddress((IfxQspi_Index)spi_n);

    spi_mux(spi_n, sck_pin, mosi_pin, miso_pin, cs_pin, &MasterPins, &SlsoPin);

    IfxQspi_SpiMaster_initModuleConfig(&MasterConfig, moudle);
    MasterConfig.base.mode              = SpiIf_Mode_master;
    MasterConfig.base.maximumBaudrate   = MAX_BAUD;
    MasterConfig.base.isrProvider       = IfxSrc_Tos_cpu0;


    MasterConfig.pins = &MasterPins;
    IfxQspi_SpiMaster_initModule(&MasterHandle, &MasterConfig);

    IfxQspi_SpiMaster_ChannelConfig MasterChConfig;
    IfxQspi_SpiMaster_initChannelConfig(&MasterChConfig, &MasterHandle);


    MasterChConfig.base.baudrate = (float)baud;
    switch(mode)
    {
        case 0:
        {
            MasterChConfig.base.mode.clockPolarity = SpiIf_ClockPolarity_idleLow;                       // CPOL
            MasterChConfig.base.mode.shiftClock    = SpiIf_ShiftClock_shiftTransmitDataOnTrailingEdge;  // CPHA
        }break;
        case 1:
        {
            MasterChConfig.base.mode.clockPolarity = SpiIf_ClockPolarity_idleLow;
            MasterChConfig.base.mode.shiftClock    = SpiIf_ShiftClock_shiftTransmitDataOnLeadingEdge;
        }break;
        case 2:
        {
            MasterChConfig.base.mode.clockPolarity = SpiIf_ClockPolarity_idleHigh;
            MasterChConfig.base.mode.shiftClock    = SpiIf_ShiftClock_shiftTransmitDataOnTrailingEdge;
        }break;
        case 3:
        {
            MasterChConfig.base.mode.clockPolarity = SpiIf_ClockPolarity_idleHigh;
            MasterChConfig.base.mode.shiftClock    = SpiIf_ShiftClock_shiftTransmitDataOnLeadingEdge;
        }break;
    }

    MasterChConfig.base.mode.dataHeading = SpiIf_DataHeading_msbFirst;
    MasterChConfig.base.mode.dataWidth   = 8;

    MasterChConfig.base.mode.csActiveLevel = Ifx_ActiveState_low;
    MasterChConfig.sls.output = SlsoPin;
    IfxQspi_SpiMaster_initChannel(&MasterChHandle, &MasterChConfig);

    if(SPI_CS_NULL == cs_pin)
    {
        IfxQspi_setSlaveSelectOutputControl(moudle, IfxQspi_ChannelId_0, FALSE, FALSE);
        spi_cs_pin      = SPI_CS_NULL;
        switch(spi_n)
        {
            case SPI_0: cs_pin = SPI0_CS0_P20_8; break;
            case SPI_1: cs_pin = SPI1_CS0_P20_8; break;
            case SPI_2: cs_pin = SPI2_CS0_P15_2; break;
            case SPI_3: cs_pin = SPI3_CS0_P02_4; break;
        }
    }

    IfxQspi_configPT1Event(moudle, IfxQspi_PhaseTransitionEvent_endOfFrame);

    bacon.U = moudle->BACON.U;
    bacon.B.DL      = 7;        // Data Length
    bacon.B.IDLE    = 1;        // Idle Delay Length
    bacon.B.IPRE    = 1;        // Prescaler for the Idle Delay
    bacon.B.LEAD    = 1;        // Leading Delay Length
    bacon.B.LPRE    = 1;        // Prescaler for the Leading Delay
    bacon.B.MSB     = 1;        // Shift MSB or LSB First
    bacon.B.PARTYP  = 0;        // Parity Type
    bacon.B.BYTE    = 0;        // Byte
    bacon.B.TRAIL   = 1;        // Trailing Delay Length
    bacon.B.TPRE    = 1;        // Prescaler for the Trailing Delay
    bacon.B.CS      = cs_pin%102/6-3;
}

