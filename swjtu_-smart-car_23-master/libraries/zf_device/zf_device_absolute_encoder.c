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
* 文件名称          zf_device_absolute_encoder
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
/*********************************************************************************************************************
* 接线定义：
*                   ------------------------------------
*                   模块管脚            单片机管脚
*                   SCLK               查看 zf_device_absolute_encoder.h 中 ABSOLUTE_ENCODER_SCLK_PIN 宏定义
*                   MOSI               查看 zf_device_absolute_encoder.h 中 ABSOLUTE_ENCODER_MOSI_PIN 宏定义
*                   MISO               查看 zf_device_absolute_encoder.h 中 ABSOLUTE_ENCODER_MISO_PIN 宏定义
*                   CS                 查看 zf_device_absolute_encoder.h 中 ABSOLUTE_ENCODER_CS_PIN 宏定义
*                   VCC                3.3V电源
*                   GND                电源地
*                   ------------------------------------
********************************************************************************************************************/


#include "zf_common_debug.h"
#include "zf_common_function.h"
#include "zf_driver_delay.h"
#include "zf_driver_soft_spi.h"
#include "zf_driver_spi.h"

#include "zf_device_absolute_encoder.h"

static int16 now_location = 0;
static int16 last_location = 0;

#if ABSOLUTE_ENCODER_USE_SOFT_SPI
static soft_spi_info_struct                 absolute_encoder_spi;
#define absolute_encoder_read()             (soft_spi_read_8bit(&absolute_encoder_spi))
#define absolute_encoder_write(data)        (soft_spi_write_8bit(&absolute_encoder_spi, (data)))
#else
#define absolute_encoder_read()             (spi_read_8bit(ABSOLUTE_ENCODER_SPI))
#define absolute_encoder_write(data)        (spi_write_8bit(ABSOLUTE_ENCODER_SPI, (data)))
#endif

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     绝对值编码器写寄存器
// 参数说明     reg             寄存器地址
// 参数说明     data            数据
// 返回参数     void
// 使用示例     absolute_encoder_write_register(i + 1, dat[i]);
// 备注信息     内部调用
//-------------------------------------------------------------------------------------------------------------------
static void absolute_encoder_write_register(uint8 reg, uint8 data)
{
    ABSOLUTE_ENCODER_CSN(0);                                                    // 片选拉低选中
    absolute_encoder_write(reg | ABSOLUTE_ENCODER_SPI_W);                       // 寄存器
    absolute_encoder_write(data);                                               // 数据
    ABSOLUTE_ENCODER_CSN(1);                                                    // 片选拉高释放
    system_delay_us(1);                                                         // 必要操作
    ABSOLUTE_ENCODER_CSN(0);                                                    // 片选拉低选中
    absolute_encoder_read();                                                    // 这里会返回写入是否成功 但不作判断
    absolute_encoder_read();                                                    // 必要操作
    ABSOLUTE_ENCODER_CSN(1);                                                    // 片选拉高释放
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     绝对值编码器读寄存器 内部调用
// 参数说明     reg             寄存器地址
// 返回参数     uint8           数据
// 使用示例     absolute_encoder_read_register(6);
// 备注信息     内部调用
//-------------------------------------------------------------------------------------------------------------------
static uint8 absolute_encoder_read_register(uint8 reg)
{
    uint8 data = 0;
    ABSOLUTE_ENCODER_CSN(0);                                                    // 片选拉低选中
    absolute_encoder_write(reg | ABSOLUTE_ENCODER_SPI_R);                       // 寄存器
    absolute_encoder_write(0x00);                                               // 占位
    ABSOLUTE_ENCODER_CSN(1);                                                    // 片选拉高释放
    system_delay_us(1);                                                         // 必要操作
    ABSOLUTE_ENCODER_CSN(0);                                                    // 片选拉低选中
    data = absolute_encoder_read();                                             // 获取读取的数据
    absolute_encoder_read();                                                    // 必要操作
    ABSOLUTE_ENCODER_CSN(1);                                                    // 片选拉高释放
    return data;
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     绝对值编码器读位置 内部调用
// 参数说明     void
// 返回参数     uint16          位置值
// 使用示例     absolute_encoder_read_data();
// 备注信息     内部调用
//-------------------------------------------------------------------------------------------------------------------
static uint16 absolute_encoder_read_data (void)
{
    uint16 data = 0;
    ABSOLUTE_ENCODER_CSN(0);                                                    // 片选拉低选中
    data = absolute_encoder_read();                                             // 获取高八位数据
    data = (data & 0x00FF) << 8;                                                // 数据位移
    data |= absolute_encoder_read();                                            // 获取低八位数据
    ABSOLUTE_ENCODER_CSN(1);                                                    // 片选拉高释放
    return data;
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     绝对值编码器自检 内部调用
// 参数说明     void
// 返回参数     uint8           自检状态
// 使用示例     absolute_encoder_self_check();
// 备注信息     内部调用
//-------------------------------------------------------------------------------------------------------------------
static uint8 absolute_encoder_self_check (void)
{
    uint8 i = 0, return_state = 0;
    uint8 dat[6] = {0, 0, 0, 0xC0, 0xFF, 0x1C};
    uint16 time_count = 0;
    while(0x1C != absolute_encoder_read_register(6))                            // 获取状态寄存器
    {
        for(i = 0; i < 6; i ++)
        {
            absolute_encoder_write_register(i + 1, dat[i]);                     // 写入默认配置参数
            system_delay_ms(1);
        }
        if(time_count ++ > ABSOLUTE_ENCODER_TIMEOUT_COUNT)                      // 等待超时
        {
            return_state = 1;
            break;
        }
    }
    return return_state;
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     绝对值编码器获取当前角度值
// 参数说明     void
// 返回参数     int16           角度值
// 使用示例     absolute_encoder_get_location();
// 备注信息
//-------------------------------------------------------------------------------------------------------------------
int16 absolute_encoder_get_location (void)
{
    last_location = now_location;
    now_location = absolute_encoder_read_data() >> 4;
    return now_location;
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     绝对值编码器获取相较上次位置的偏移值
// 参数说明     void
// 返回参数     int16           偏移值
// 使用示例     absolute_encoder_get_offset();
// 备注信息
//-------------------------------------------------------------------------------------------------------------------
int16 absolute_encoder_get_offset (void)
{
    int16 result_data = 0;
    if(func_abs(now_location - last_location) > 2048)
    {
        result_data = (now_location > 2048 ? (now_location - 4096 - last_location) : (now_location + 4096 - last_location));
    }
    else
    {
        result_data = (now_location - last_location);
    }
    return result_data;
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     绝对值编码器初始化
// 参数说明     void
// 返回参数     uint8           初始化状态 0-成功 1-失败
// 使用示例     absolute_encoder_init();
// 备注信息
//-------------------------------------------------------------------------------------------------------------------
uint8 absolute_encoder_init (void)
{
    uint8 return_state = 0;
    uint16 zero_position = ABSOLUTE_ENCODER_DEFAULT_ZERO;
#if ABSOLUTE_ENCODER_USE_SOFT_SPI
    soft_spi_init(&absolute_encoder_spi, 0, ABSOLUTE_ENCODER_SOFT_SPI_DELAY, ABSOLUTE_ENCODER_SCLK_PIN, ABSOLUTE_ENCODER_MOSI_PIN, ABSOLUTE_ENCODER_MISO_PIN, SOFT_SPI_PIN_NULL);
#else
    spi_init(ABSOLUTE_ENCODER_SPI, SPI_MODE0, ABSOLUTE_ENCODER_SPI_SPEED, ABSOLUTE_ENCODER_SCLK_PIN, ABSOLUTE_ENCODER_MOSI_PIN, ABSOLUTE_ENCODER_MISO_PIN, SPI_CS_NULL);
#endif
    gpio_init(ABSOLUTE_ENCODER_CS_PIN, GPO, GPIO_LOW, GPO_PUSH_PULL);

    do
    {
        if(absolute_encoder_self_check())
        {
            // 如果程序在输出了断言信息 并且提示出错位置在这里
            // 那么就是绝对值编码器自检出错并超时退出了
            // 检查一下接线有没有问题 如果没问题可能就是坏了
            return_state = 1;
            zf_log(0, "absolute encoder init errror.");
            break;
        }
        absolute_encoder_write_register(ABSOLUTE_ENCODER_DIR_REG, 0x00);                    // 设置旋转方向 正转数值变小：0x00   反转数值变大：0x80
        zero_position = (uint16)(4096 - zero_position);
        zero_position = zero_position << 4;
        absolute_encoder_write_register(ABSOLUTE_ENCODER_ZERO_L_REG, (uint8)zero_position); // 设置零位
        absolute_encoder_write_register(ABSOLUTE_ENCODER_ZERO_H_REG, zero_position >> 8);
    }while(0);
    return return_state;
}
