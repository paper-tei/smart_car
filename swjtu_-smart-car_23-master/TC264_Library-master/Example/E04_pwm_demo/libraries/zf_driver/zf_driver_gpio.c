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
* 文件名称          zf_driver_gpio
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

#include "zf_driver_gpio.h"

//-------------------------------------------------------------------------------------------------------------------
//  函数简介      获取GPIO基地址
//  参数说明      pin         选择的引脚 (可选择范围由 zf_driver_gpio.h 内gpio_pin_enum枚举值确定)
//  返回参数      void
//  使用示例      get_port(P00_0)
//  备注信息      本函数在文件内部调用 用户不用关注 也不可修改
//-------------------------------------------------------------------------------------------------------------------
Ifx_P* get_port (gpio_pin_enum pin)
{
    volatile Ifx_P *port;

    switch(pin&0xffe0)
    {
        case P00_0: port = &MODULE_P00; break;
        case P02_0: port = &MODULE_P02; break;
        case P10_0: port = &MODULE_P10; break;
        case P11_0: port = &MODULE_P11; break;
        case P13_0: port = &MODULE_P13; break;
        case P14_0: port = &MODULE_P14; break;
        case P15_0: port = &MODULE_P15; break;
        case P20_0: port = &MODULE_P20; break;
        case P21_0: port = &MODULE_P21; break;
        case P22_0: port = &MODULE_P22; break;
        case P23_0: port = &MODULE_P23; break;
        case P32_0: port = &MODULE_P32; break;
        case P33_0: port = &MODULE_P33; break;
        default:break;
    }
#pragma warning 507
    return port;
#pragma warning default
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     gpio 输出设置
// 参数说明     pin         选择的引脚 (可选择范围由 zf_driver_gpio.h 内 gpio_pin_enum 枚举值确定)
// 参数说明     dat         0：低电平 1：高电平
// 返回参数     void
// 使用示例     gpio_set_level(P00_0, 1);// P00_0 输出高电平
// 备注信息
//-------------------------------------------------------------------------------------------------------------------
void gpio_set_level (gpio_pin_enum pin, uint8 dat)
{
    if(dat)
    {
        IfxPort_setPinHigh(get_port(pin), pin&0x1f);
    }
    else
    {
        IfxPort_setPinLow(get_port(pin), pin&0x1f);
    }
}
//-------------------------------------------------------------------------------------------------------------------
// 函数简介     gpio 电平获取
// 参数说明     pin         选择的引脚 (可选择范围由 zf_driver_gpio.h 内 gpio_pin_enum 枚举值确定)
// 返回参数     uint8       引脚当前电平
// 使用示例     uint8 status = gpio_get_level(P00_0);// 获取P00_0引脚电平
// 备注信息
//-------------------------------------------------------------------------------------------------------------------
uint8 gpio_get_level (gpio_pin_enum pin)
{
    return IfxPort_getPinState(get_port(pin), pin&0x1f);
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     gpio 翻转电平
// 参数说明     pin         选择的引脚 (可选择范围由 zf_driver_gpio.h 内 gpio_pin_enum 枚举值确定)
// 返回参数     void
// 使用示例     gpio_toggle_level(P00_0);// P00_0引脚电平翻转
// 备注信息
//-------------------------------------------------------------------------------------------------------------------
void gpio_toggle_level (gpio_pin_enum pin)
{
    IfxPort_togglePin(get_port(pin), pin&0x1f);
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     gpio 方向设置
// 参数说明     pin         选择的引脚 (可选择范围由 zf_driver_gpio.h 内 gpio_pin_enum 枚举值确定)
// 参数说明     dir         引脚的方向   输出：GPO   输入：GPI
// 参数说明     mode        引脚的模式 (可选择范围由 zf_driver_gpio.h 内 gpio_mode_enum 枚举值确定)
// 返回参数     void
// 使用示例     gpio_dir(P00_0, GPO, GPO_PUSH_PULL);// 设置P00_0为推挽输出模式
// 备注信息
//-------------------------------------------------------------------------------------------------------------------
void gpio_set_dir (gpio_pin_enum pin, gpio_dir_enum dir, gpio_mode_enum pinmode)
{
    IfxPort_Mode port_mode;
    if(dir == GPI)
    {
        switch(pinmode)
        {
            case GPI_FLOATING_IN:port_mode = IfxPort_Mode_inputNoPullDevice ;       break; // 浮空输入
            case GPI_PULL_DOWN  :port_mode = IfxPort_Mode_inputPullDown     ;       break; // 下拉输入
            default:             port_mode = IfxPort_Mode_inputPullUp       ;       break; // 默认为上拉输入
        }
    }
    else
    {
        switch(pinmode)
        {
            case GPO_OPEN_DTAIN :port_mode = IfxPort_Mode_outputOpenDrainGeneral;   break;// 开漏输出
            default:             port_mode = IfxPort_Mode_outputPushPullGeneral ;   break;// 默认为推挽输出
        }
    }

    IfxPort_setPinMode(get_port(pin), pin&0x1f, port_mode);
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     gpio 初始化
// 参数说明     pin         选择的引脚 (可选择范围由 zf_driver_gpio.h 内 gpio_pin_enum 枚举值确定)
// 参数说明     mode        引脚的方向 [GPI/GPIO]
// 参数说明     dat         引脚初始化时设置的电平状态，输出时有效 0：低电平 1：高电平 仅在设置为输出模式时有效
// 参数说明     mode        引脚的模式 (可选择范围由 zf_driver_gpio.h 内 gpio_mode_enum 枚举值确定)
// 返回参数     void
// 使用示例     gpio_init(P00_0, GPO, 1, GPO_PUSH_PULL); // P00_0初始化为GPIO功能、输出模式、输出高电平、推挽输出
// 备注信息     需要特别注意P20_2是不能用于输出的，仅仅只有输入的功能
//-------------------------------------------------------------------------------------------------------------------
void gpio_init (gpio_pin_enum pin, gpio_dir_enum dir, uint8 dat, gpio_mode_enum pinmode)
{
    IfxPort_Mode port_mode;

    if(dir == GPI)
    {
        switch(pinmode)
        {
            case GPI_FLOATING_IN:port_mode = IfxPort_Mode_inputNoPullDevice ;       break; // 浮空输入
            case GPI_PULL_DOWN  :port_mode = IfxPort_Mode_inputPullDown     ;       break; // 下拉输入
            default:             port_mode = IfxPort_Mode_inputPullUp       ;       break; // 默认为上拉输入
        }
    }
    else
    {
        switch(pinmode)
        {
            case GPO_OPEN_DTAIN :port_mode = IfxPort_Mode_outputOpenDrainGeneral;   break;// 开漏输出
            default:             port_mode = IfxPort_Mode_outputPushPullGeneral ;   break;// 默认为推挽输出
        }
    }

    IfxPort_setPinMode(get_port(pin), pin&0x1f, port_mode);

    IfxPort_setPinPadDriver(get_port(pin), pin&0x1f, IfxPort_PadDriver_cmosAutomotiveSpeed1);

    if(dir == GPO)
    {
        if(dat)
        {
            IfxPort_setPinHigh(get_port(pin), pin&0x1f);
        }
        else
        {
            IfxPort_setPinLow(get_port(pin), pin&0x1f);
        }
    }

}
