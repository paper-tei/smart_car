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
* 文件名称          zf_driver_soft_spi
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

#include "zf_common_debug.h"
#include "zf_driver_soft_spi.h"

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     软件 SPI 延时
// 参数说明     void
// 返回参数     void
// 使用示例     soft_spi_delay(1);
// 备注信息     内部调用
//-------------------------------------------------------------------------------------------------------------------
#define soft_spi_delay(x)  for(uint32 i = x; i --; )

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     软件 SPI 8bit 数据读写
// 参数说明     *soft_spi_obj   软件 SPI 指定信息存放结构体的指针
// 参数说明     data            数据
// 返回参数     uint8           读取的数据
// 使用示例     soft_spi_8bit_data_handler(soft_spi_obj, 1);
// 备注信息     内部调用
//-------------------------------------------------------------------------------------------------------------------
static uint8 soft_spi_8bit_data_handler (soft_spi_info_struct *soft_spi_obj, const uint8 data)
{
    uint8 temp = 0;
    uint8 write_data = data;
    uint8 read_data = 0;

    if(soft_spi_obj->config.use_cs)
    {
        gpio_low(soft_spi_obj->cs_pin);
    }

    if(0 == soft_spi_obj->config.mode || 1 == soft_spi_obj->config.mode)        // CPOL = 0 SCK 空闲低电平
    {
        gpio_low(soft_spi_obj->sck_pin);
    }
    else                                                                        // CPOL = 1 SCK 空闲高电平
    {
        gpio_high(soft_spi_obj->sck_pin);
    }

    if(0 == soft_spi_obj->config.mode % 2)                                      // CPHA = 0 第一个边沿采样
    {
        for(temp = 8; temp > 0; temp --)
        {
            if(0x80 & write_data)
            {
                gpio_high(soft_spi_obj->mosi_pin);
            }
            else
            {
                gpio_low(soft_spi_obj->mosi_pin);
            }
            soft_spi_delay(soft_spi_obj->delay);
            gpio_toggle_level(soft_spi_obj->sck_pin);
            write_data = write_data << 1;
            read_data = read_data << 1;
            if(soft_spi_obj->config.use_miso)
            {
                read_data |= gpio_get_level(soft_spi_obj->miso_pin);
            }
            soft_spi_delay(soft_spi_obj->delay);
            gpio_toggle_level(soft_spi_obj->sck_pin);
        }
    }
    else                                                                        // CPHA = 1 第二个边沿采样
    {
        for(temp = 8; 0 < temp; temp --)
        {
            gpio_toggle_level(soft_spi_obj->sck_pin);
            if(0x80 & write_data)
            {
                gpio_high(soft_spi_obj->mosi_pin);
            }
            else
            {
                gpio_low(soft_spi_obj->mosi_pin);
            }
            soft_spi_delay(soft_spi_obj->delay);
            gpio_toggle_level(soft_spi_obj->sck_pin);
            write_data = write_data << 1;
            read_data = read_data << 1;
            if(soft_spi_obj->config.use_miso)
            {
                read_data |= gpio_get_level(soft_spi_obj->miso_pin);
            }
            soft_spi_delay(soft_spi_obj->delay);
        }
    }

    if(soft_spi_obj->config.use_cs)
    {
        gpio_high(soft_spi_obj->cs_pin);
    }
    return read_data;
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     软件 SPI 16bit 数据读写
// 参数说明     *soft_spi_obj   软件 SPI 指定信息存放结构体的指针
// 参数说明     data            数据
// 返回参数     uint16          读取的数据
// 使用示例     soft_spi_16bit_data_handler(soft_spi_obj, 1);
// 备注信息     内部调用
//-------------------------------------------------------------------------------------------------------------------
static uint16 soft_spi_16bit_data_handler (soft_spi_info_struct *soft_spi_obj, const uint16 data)
{
    uint8 temp = 0;
    uint16 write_data = data;
    uint16 read_data = 0;

    if(soft_spi_obj->config.use_cs)
    {
        gpio_low(soft_spi_obj->cs_pin);
    }

    if(0 == soft_spi_obj->config.mode || 1 == soft_spi_obj->config.mode)        // CPOL = 0 SCK 空闲低电平
    {
        gpio_low(soft_spi_obj->sck_pin);
    }
    else                                                                        // CPOL = 1 SCK 空闲高电平
    {
        gpio_high(soft_spi_obj->sck_pin);
    }

    if(0 == soft_spi_obj->config.mode % 2)                                      // CPHA = 0 第一个边沿采样
    {
        for(temp = 16; 0 < temp; temp --)
        {
            if(0x8000 & write_data)
            {
                gpio_high(soft_spi_obj->mosi_pin);
            }
            else
            {
                gpio_low(soft_spi_obj->mosi_pin);
            }
            soft_spi_delay(soft_spi_obj->delay);
            gpio_toggle_level(soft_spi_obj->sck_pin);
            write_data = write_data << 1;
            read_data = read_data << 1;
            if(soft_spi_obj->config.use_miso)
            {
                read_data |= gpio_get_level(soft_spi_obj->miso_pin);
            }
            soft_spi_delay(soft_spi_obj->delay);
            gpio_toggle_level(soft_spi_obj->sck_pin);
        }
    }
    else                                                                        // CPHA = 1 第二个边沿采样
    {
        for(temp = 16; 0 < temp; temp --)
        {
            gpio_toggle_level(soft_spi_obj->sck_pin);
            if(write_data & 0x8000)
            {
                gpio_high(soft_spi_obj->mosi_pin);
            }
            else
            {
                gpio_low(soft_spi_obj->mosi_pin);
            }
            soft_spi_delay(soft_spi_obj->delay);
            gpio_toggle_level(soft_spi_obj->sck_pin);
            write_data = write_data << 1;
            read_data = read_data << 1;
            if(soft_spi_obj->config.use_miso)
            {
                read_data |= gpio_get_level(soft_spi_obj->miso_pin);
            }
            soft_spi_delay(soft_spi_obj->delay);
        }
    }

    if(soft_spi_obj->config.use_cs)
    {
        gpio_high(soft_spi_obj->cs_pin);
    }
    return read_data;
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     软件 SPI 接口写 8bit 数据
// 参数说明     *soft_spi_obj   软件 SPI 指定信息存放结构体的指针
// 参数说明     data            发送的数据
// 返回参数     void
// 使用示例     soft_spi_write_8bit(&soft_spi_obj, 1);
// 备注信息
//-------------------------------------------------------------------------------------------------------------------
void soft_spi_write_8bit (soft_spi_info_struct *soft_spi_obj, const uint8 data)
{
    soft_spi_8bit_data_handler(soft_spi_obj, data);
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     软件 SPI 接口写 8bit 数组
// 参数说明     *soft_spi_obj   软件 SPI 指定信息存放结构体的指针
// 参数说明     *data           数据存放缓冲区
// 参数说明     len             缓冲区长度
// 返回参数     void
// 使用示例     soft_spi_write_8bit_array(&soft_spi_obj, buf, 16);
// 备注信息
//-------------------------------------------------------------------------------------------------------------------
void soft_spi_write_8bit_array (soft_spi_info_struct *soft_spi_obj, const uint8 *data, uint32 len)
{
    while(len --)
    {
        soft_spi_8bit_data_handler(soft_spi_obj, *data ++);
    }
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     软件 SPI 接口写 16bit 数据
// 参数说明     *soft_spi_obj   软件 SPI 指定信息存放结构体的指针
// 参数说明     data            发送的数据
// 返回参数     void
// 使用示例     soft_spi_write_16bit(&soft_spi_obj, 1);
// 备注信息
//-------------------------------------------------------------------------------------------------------------------
void soft_spi_write_16bit (soft_spi_info_struct *soft_spi_obj, uint16 data)
{
    soft_spi_16bit_data_handler(soft_spi_obj, data);
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     软件 SPI 接口写 16bit 数组
// 参数说明     *soft_spi_obj   软件 SPI 指定信息存放结构体的指针
// 参数说明     *data           数据存放缓冲区
// 参数说明     len             缓冲区长度
// 返回参数     void
// 使用示例     soft_spi_write_16bit_array(&soft_spi_obj, buf, 16);
// 备注信息
//-------------------------------------------------------------------------------------------------------------------
void soft_spi_write_16bit_array (soft_spi_info_struct *soft_spi_obj, const uint16 *data, uint32 len)
{
    while(len --)
    {
        soft_spi_16bit_data_handler(soft_spi_obj, *data ++);
    }
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     软件 SPI 接口向传感器的寄存器写 8bit 数据
// 参数说明     *soft_spi_obj   软件 SPI 指定信息存放结构体的指针
// 参数说明     register_name   寄存器地址
// 参数说明     data            发送的数据
// 返回参数     void
// 使用示例     soft_spi_write_8bit_register(&soft_spi_obj, 1, 1);
// 备注信息
//-------------------------------------------------------------------------------------------------------------------
void soft_spi_write_8bit_register (soft_spi_info_struct *soft_spi_obj, const uint8 register_name, const uint8 data)
{
    soft_spi_8bit_data_handler(soft_spi_obj, register_name);
    soft_spi_8bit_data_handler(soft_spi_obj, data);
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     软件 SPI 接口向传感器的寄存器写 8bit 数组
// 参数说明     *soft_spi_obj   软件 SPI 指定信息存放结构体的指针
// 参数说明     register_name   寄存器地址
// 参数说明     *data           数据存放缓冲区
// 参数说明     len             缓冲区长度
// 返回参数     void
// 使用示例     soft_spi_write_8bit_registers(&soft_spi_obj, 1, buf, 16);
//-------------------------------------------------------------------------------------------------------------------
void soft_spi_write_8bit_registers (soft_spi_info_struct *soft_spi_obj, const uint8 register_name, const uint8 *data, uint32 len)
{
    soft_spi_8bit_data_handler(soft_spi_obj, register_name);
    while(len --)
    {
        soft_spi_8bit_data_handler(soft_spi_obj, *data ++);
    }
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     软件 SPI 接口向传感器的寄存器写 16bit 数据
// 参数说明     *soft_spi_obj   软件 SPI 指定信息存放结构体的指针
// 参数说明     register_name   寄存器地址
// 参数说明     data            发送的数据
// 返回参数     void
// 使用示例     soft_spi_write_16bit_register(&soft_spi_obj, 1, 1);
// 备注信息
//-------------------------------------------------------------------------------------------------------------------
void soft_spi_write_16bit_register (soft_spi_info_struct *soft_spi_obj, const uint16 register_name, uint16 data)
{
    soft_spi_16bit_data_handler(soft_spi_obj, register_name);
    soft_spi_16bit_data_handler(soft_spi_obj, data);
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     软件 SPI 接口向传感器的寄存器写 16bit 数组
// 参数说明     *soft_spi_obj   软件 SPI 指定信息存放结构体的指针
// 参数说明     register_name   寄存器地址
// 参数说明     *data           数据存放缓冲区
// 参数说明     len             缓冲区长度
// 返回参数     void
// 使用示例     soft_spi_write_16bit_registers(&soft_spi_obj, 1, buf, 16);
// 备注信息
//-------------------------------------------------------------------------------------------------------------------
void soft_spi_write_16bit_registers (soft_spi_info_struct *soft_spi_obj, const uint16 register_name, const uint16 *data, uint32 len)
{
    soft_spi_16bit_data_handler(soft_spi_obj, register_name);
    while(len --)
    {
        soft_spi_16bit_data_handler(soft_spi_obj, *data ++);
    }
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     软件 SPI 接口读 8bit 数据
// 参数说明     *soft_spi_obj   软件 SPI 指定信息存放结构体的指针
// 参数说明     data            发送的数据
// 返回参数     uint8           返回读取的 8bit 数据
// 使用示例     soft_spi_read_8bit(&soft_spi_obj);
// 备注信息
//-------------------------------------------------------------------------------------------------------------------
uint8 soft_spi_read_8bit (soft_spi_info_struct *soft_spi_obj)
{
    return soft_spi_8bit_data_handler(soft_spi_obj, 0);
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     软件 SPI 接口读 8bit 数组
// 参数说明     *soft_spi_obj   软件 SPI 指定信息存放结构体的指针
// 参数说明     *data           数据存放缓冲区
// 参数说明     len             缓冲区长度
// 返回参数     void
// 使用示例     soft_spi_read_8bit_array(&soft_spi_obj, buf, 16);
// 备注信息
//-------------------------------------------------------------------------------------------------------------------
void soft_spi_read_8bit_array (soft_spi_info_struct *soft_spi_obj, uint8 *data, uint32 len)
{
    while(len --)
    {
        *data ++ = soft_spi_8bit_data_handler(soft_spi_obj, 0);
    }
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     软件 SPI 接口读 16bit 数据
// 参数说明     *soft_spi_obj   软件 SPI 指定信息存放结构体的指针
// 参数说明     data            发送的数据
// 返回参数     uint16          返回读取的 16bit 数据
// 使用示例     soft_spi_read_16bit(&soft_spi_obj);
// 备注信息
//-------------------------------------------------------------------------------------------------------------------
uint16 soft_spi_read_16bit (soft_spi_info_struct *soft_spi_obj)
{
    return soft_spi_16bit_data_handler(soft_spi_obj, 0);
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     软件 SPI 接口读 16bit 数组
// 参数说明     *soft_spi_obj   软件 SPI 指定信息存放结构体的指针
// 参数说明     *data           数据存放缓冲区
// 参数说明     len             缓冲区长度
// 返回参数     void
// 使用示例     soft_spi_read_16bit_array(&soft_spi_obj, buf, 16);
// 备注信息
//-------------------------------------------------------------------------------------------------------------------
void soft_spi_read_16bit_array (soft_spi_info_struct *soft_spi_obj, uint16 *data, uint32 len)
{
    while(len --)
    {
        *data ++ = soft_spi_16bit_data_handler(soft_spi_obj, 0);
    }
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     软件 SPI 接口从传感器的寄存器读 8bit 数据
// 参数说明     *soft_spi_obj   软件 SPI 指定信息存放结构体的指针
// 参数说明     register_name   寄存器地址
// 参数说明     data            发送的数据
// 返回参数     uint8           返回读取的 8bit 数据
// 使用示例     soft_spi_read_8bit_register(&soft_spi_obj, 0x01, 0x01);
// 备注信息
//-------------------------------------------------------------------------------------------------------------------
uint8 soft_spi_read_8bit_register (soft_spi_info_struct *soft_spi_obj, const uint8 register_name)
{
    soft_spi_8bit_data_handler(soft_spi_obj, register_name);
    return soft_spi_8bit_data_handler(soft_spi_obj, 0);
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     软件 SPI 接口从传感器的寄存器读 8bit 数组
// 参数说明     *soft_spi_obj   软件 SPI 指定信息存放结构体的指针
// 参数说明     register_name   寄存器地址
// 参数说明     *data           数据存放缓冲区
// 参数说明     len             缓冲区长度
// 返回参数     void
// 使用示例     soft_spi_read_8bit_registers(&soft_spi_obj, 0x01, buf, 16);
// 备注信息
//-------------------------------------------------------------------------------------------------------------------
void soft_spi_read_8bit_registers (soft_spi_info_struct *soft_spi_obj, const uint8 register_name, uint8 *data, uint32 len)
{
    soft_spi_8bit_data_handler(soft_spi_obj, register_name);
    while(len --)
    {
        *data ++ = soft_spi_8bit_data_handler(soft_spi_obj, 0);
    }
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     软件 SPI 接口从传感器的寄存器读 16bit 数据
// 参数说明     *soft_spi_obj   软件 SPI 指定信息存放结构体的指针
// 参数说明     register_name   寄存器地址
// 参数说明     data            发送的数据
// 返回参数     uint16          返回读取的 16bit 数据
// 使用示例     soft_spi_read_16bit_register(&soft_spi_obj, 0x0101);
// 备注信息
//-------------------------------------------------------------------------------------------------------------------
uint16 soft_spi_read_16bit_register (soft_spi_info_struct *soft_spi_obj, const uint16 register_name)
{
    soft_spi_16bit_data_handler(soft_spi_obj, register_name);
    return soft_spi_16bit_data_handler(soft_spi_obj, 0);
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     软件 SPI 接口从传感器的寄存器读 16bit 数组
// 参数说明     *soft_spi_obj   软件 SPI 指定信息存放结构体的指针
// 参数说明     register_name   寄存器地址
// 参数说明     *data           数据存放缓冲区
// 参数说明     len             缓冲区长度
// 返回参数     void
// 使用示例     soft_spi_read_16bit_registers(&soft_spi_obj, 0x0101, buf, 16);
// 备注信息
//-------------------------------------------------------------------------------------------------------------------
void soft_spi_read_16bit_registers (soft_spi_info_struct *soft_spi_obj, const uint16 register_name, uint16 *data, uint32 len)
{
    soft_spi_16bit_data_handler(soft_spi_obj, register_name);
    while(len --)
    {
        *data ++ = soft_spi_16bit_data_handler(soft_spi_obj, 0);
    }
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     软件 SPI 8bit 数据传输 发送与接收数据是同时进行的
// 参数说明     *soft_spi_obj   软件 SPI 指定信息存放结构体的指针
// 参数说明     write_buffer    发送的数据缓冲区地址
// 参数说明     read_buffer     发送数据时接收到的数据的存储地址(不需要接收则传 NULL)
// 参数说明     len             发送的字节数
// 返回参数     void
// 使用示例     soft_spi_8bit_transfer(&soft_spi_obj, buf, buf, 1);
// 备注信息
//-------------------------------------------------------------------------------------------------------------------
void soft_spi_8bit_transfer (soft_spi_info_struct *soft_spi_obj, const uint8 *write_buffer, uint8 *read_buffer, uint32 len)
{
    while(len --)
    {
        *read_buffer = soft_spi_8bit_data_handler(soft_spi_obj, *write_buffer);
        write_buffer ++;
        read_buffer ++;
    }
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     软件 SPI 16bit 数据传输 发送与接收数据是同时进行的
// 参数说明     *soft_spi_obj   软件 SPI 指定信息存放结构体的指针
// 参数说明     write_buffer    发送的数据缓冲区地址
// 参数说明     read_buffer     发送数据时接收到的数据的存储地址(不需要接收则传 NULL)
// 参数说明     len             发送的字节数
// 返回参数     void
// 使用示例     soft_spi_16bit_transfer(&soft_spi_obj, buf, buf, 1);
// 备注信息
//-------------------------------------------------------------------------------------------------------------------
void soft_spi_16bit_transfer (soft_spi_info_struct *soft_spi_obj, const uint16 *write_buffer, uint16 *read_buffer, uint32 len)
{
    while(len --)
    {
        *read_buffer = soft_spi_16bit_data_handler(soft_spi_obj, *write_buffer);
        write_buffer ++;
        read_buffer ++;
    }
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     软件 SPI 接口初始化
// 参数说明     *soft_spi_obj   软件 SPI 指定信息存放结构体的指针
// 参数说明     mode            SPI 模式 参照 zf_driver_spi.h 内 spi_mode_enum 枚举体定义
// 参数说明     delay           软件 SPI 延时 就是时钟高电平时间 越短 SPI 速率越高
// 参数说明     sck_pin         选择 SCK 引脚 参照 zf_driver_gpio.h 内 gpio_pin_enum 枚举体定义
// 参数说明     mosi_pin        选择 MOSI 引脚 参照 zf_driver_gpio.h 内 gpio_pin_enum 枚举体定义
// 参数说明     miso_pin        选择 MISO 引脚 如果不需要这个引脚 就填 SOFT_SPI_PIN_NULL
// 参数说明     cs_pin          选择 CS 引脚 如果不需要这个引脚 就填 SOFT_SPI_PIN_NULL
// 返回参数     void
// 使用示例     spi_init(SPI_1, 0, 1*1000*1000, A5, A7, A6, A4);
// 备注信息
//-------------------------------------------------------------------------------------------------------------------
void soft_spi_init (soft_spi_info_struct *soft_spi_obj, uint8 mode, uint32 delay, gpio_pin_enum sck_pin, gpio_pin_enum mosi_pin, uint32 miso_pin, uint32 cs_pin)
{
    zf_assert(sck_pin != mosi_pin);                                             // sck_pin  与 mosi_pin 怎么能填同一个引脚？
    zf_assert(sck_pin != miso_pin);                                             // sck_pin  与 miso_pin 怎么能填同一个引脚？
    zf_assert(sck_pin != cs_pin);                                               // sck_pin  与 cs_pin   怎么能填同一个引脚？
    zf_assert(mosi_pin != miso_pin);                                            // mosi_pin 与 miso_pin 怎么能填同一个引脚？
    zf_assert(mosi_pin != cs_pin);                                              // mosi_pin 与 cs_pin   怎么能填同一个引脚？
    zf_assert((miso_pin != cs_pin) || (cs_pin == SOFT_SPI_PIN_NULL));           // miso_pin 与 cs_pin   怎么能填同一个引脚？

    zf_assert(4 > mode);                                                        // 参照 zf_driver_spi.h 内 spi_mode_enum 枚举体定义

    soft_spi_obj->config.mode = mode;
    soft_spi_obj->delay = delay;

    soft_spi_obj->sck_pin = sck_pin;
    soft_spi_obj->mosi_pin = mosi_pin;
    if(0 == mode || 1 == mode)
    {
        gpio_init(sck_pin, GPO, GPIO_LOW, GPO_PUSH_PULL);                       // IO 初始化
    }
    else
    {
        gpio_init(sck_pin, GPO, GPIO_HIGH, GPO_PUSH_PULL);                      // IO 初始化
    }
    gpio_init(mosi_pin, GPO, GPIO_HIGH, GPO_PUSH_PULL);                         // IO 初始化

    if(SOFT_SPI_PIN_NULL == miso_pin)
    {
        soft_spi_obj->config.use_miso = 0;
    }
    else
    {
        soft_spi_obj->config.use_miso = 1;
        soft_spi_obj->miso_pin = (gpio_pin_enum)miso_pin;
        gpio_init(soft_spi_obj->miso_pin, GPI, GPIO_HIGH, GPI_FLOATING_IN);     // IO 初始化
    }
    if(SOFT_SPI_PIN_NULL == cs_pin)
    {
        soft_spi_obj->config.use_cs = 0;
    }
    else
    {
        soft_spi_obj->config.use_cs = 1;
        soft_spi_obj->cs_pin = (gpio_pin_enum)cs_pin;
        gpio_init(soft_spi_obj->cs_pin, GPO, GPIO_HIGH, GPO_PUSH_PULL);         // IO 初始化
    }
}

