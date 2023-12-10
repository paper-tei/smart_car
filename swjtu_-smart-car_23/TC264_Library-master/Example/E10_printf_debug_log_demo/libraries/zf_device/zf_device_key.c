/*********************************************************************************************************************
* MM32F327X-G9P Opensourec Library 即（MM32F327X-G9P 开源库）是一个基于官方 SDK 接口的第三方开源库
* Copyright (c) 2022 SEEKFREE 逐飞科技
*
* 本文件是 MM32F327X-G9P 开源库的一部分
*
* MM32F327X-G9P 开源库 是免费软件
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
* 文件名称          zf_device_key
* 公司名称          成都逐飞科技有限公司
* 版本信息          查看 libraries/doc 文件夹内 version 文件 版本说明
* 开发环境          IAR 8.32.4 or MDK 5.37
* 适用平台          MM32F327X_G9P
* 店铺链接          https://seekfree.taobao.com/
*
* 修改记录
* 日期              作者                备注
* 2022-08-10        Teternal            first version
********************************************************************************************************************/
/*********************************************************************************************************************
* 接线定义：
*                   ------------------------------------
*                   模块管脚            单片机管脚
*                   // 一般是主板按键对应的引脚
*                   KEY1/S1             查看 zf_device_key.h 中 KEY_LIST[0]
*                   KEY2/S2             查看 zf_device_key.h 中 KEY_LIST[1]
*                   KEY3/S3             查看 zf_device_key.h 中 KEY_LIST[2]
*                   KEY4/S4             查看 zf_device_key.h 中 KEY_LIST[3]
*                   ------------------------------------
********************************************************************************************************************/

#include "zf_common_debug.h"
#include "zf_device_key.h"

static uint32               scanner_period = 0;                                 // 按键的扫描周期
static uint32               key_press_time[KEY_NUMBER];                         // 按键信号持续时长
static key_state_enum       key_state[KEY_NUMBER];                              // 按键状态

static const gpio_pin_enum  key_index[KEY_NUMBER] = KEY_LIST;

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     按键状态扫描
// 参数说明     void
// 返回参数     void
// 使用示例     key_scanner();
// 备注信息     这个函数放在主循环或者 PIT 中断中
//-------------------------------------------------------------------------------------------------------------------
void key_scanner (void)
{
    uint8 i = 0;
    for(i = 0; i < KEY_NUMBER; i ++)
    {
        switch(key_state[i])
        {
            case KEY_RELEASE:
                if(KEY_RELEASE_LEVEL != gpio_get_level(key_index[i]))
                {
                    if(++ key_press_time[i] >= KEY_MAX_SHOCK_PERIOD / scanner_period)
                    {
                        key_state[i] = KEY_SHORT_PRESS;
                    }
                    else
                    {
                        key_state[i] = KEY_CHECK_SHOCK;
                    }
                }
                break;
            case KEY_CHECK_SHOCK:
                if(KEY_RELEASE_LEVEL != gpio_get_level(key_index[i]))
                {
                    if(++ key_press_time[i] >= KEY_MAX_SHOCK_PERIOD / scanner_period)
                    {
                        key_state[i] = KEY_SHORT_PRESS;
                    }
                }
                else
                {
                    key_state[i] = KEY_RELEASE;
                    key_press_time[i] = 0;
                }
                break;
            case KEY_SHORT_PRESS:
                if(++ key_press_time[i] >= KEY_LONG_PRESS_PERIOD / scanner_period)
                {
                    key_state[i] = KEY_LONG_PRESS;
                }
                if(KEY_RELEASE_LEVEL == gpio_get_level(key_index[i]))
                {
                    key_state[i] = KEY_RELEASE;
                    key_press_time[i] = 0;
                }
                break;
            case KEY_LONG_PRESS:
                if(KEY_RELEASE_LEVEL == gpio_get_level(key_index[i]))
                {
                    key_state[i] = KEY_RELEASE;
                    key_press_time[i] = 0;
                }
                break;

        }
    }
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     获取按键状态
// 参数说明     key_n           按键索引
// 返回参数     key_state_enum  按键状态
// 使用示例     key_get_state(KEY_1);
// 备注信息
//-------------------------------------------------------------------------------------------------------------------
key_state_enum key_get_state (key_index_enum key_n)
{
    return key_state[key_n];
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     按键初始化
// 参数说明     period          按键扫描周期 以毫秒为单位
// 返回参数     void
// 使用示例     key_init(10);
// 备注信息
//-------------------------------------------------------------------------------------------------------------------
uint8 key_init (uint32 period)
{
    uint8 loop_temp = 0;

    zf_assert(0 < period);      // 扫描周期必须大于0

    for(loop_temp = 0; loop_temp < KEY_NUMBER; loop_temp ++)
    {
        gpio_init(key_index[loop_temp], GPI, GPIO_HIGH, GPI_PULL_UP);
        key_state[loop_temp] = KEY_RELEASE;
    }
    scanner_period = period;
    return 0;
}
