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
* 文件名称          zf_common_debug
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

#ifndef _zf_common_debug_h_
#define _zf_common_debug_h_

#include "zf_common_typedef.h"

#define DEBUG_UART_INDEX            (UART_0)            // 指定 debug uart 所使用的的串口
#define DEBUG_UART_BAUDRATE         (115200)            // 指定 debug uart 所使用的的串口波特率
#define DEBUG_UART_TX_PIN           (UART0_TX_P14_0)    // 指定 debug uart 所使用的的串口引脚
#define DEBUG_UART_RX_PIN           (UART0_RX_P14_1)    // 指定 debug uart 所使用的的串口引脚
#define DEBUG_UART_USE_INTERRUPT    (0)                 // 是否启用 debug uart 接收中断


//-------------------------------------------------------------------------------------------------------------------
// 函数简介     断言
// 参数说明     x           判断是否触发断言 0-触发断言 1-不触发断言
// 返回参数     void
// 使用示例     zf_assert(0);
// 备注信息     一般用于参数判断 zf_assert(0) 就断言报错
//             默认情况下会在 Debug UART 输出
//             但如果使用开源库内屏幕接口初始化了屏幕 则会在屏幕上显示
//-------------------------------------------------------------------------------------------------------------------
#define zf_assert(x)                (debug_assert_handler((x), __FILE__, __LINE__))     // 断言 一般用于参数判断 zf_assert(0) 就断言报错
//-------------------------------------------------------------------------------------------------------------------
// 函数简介     Log 信息输出
// 参数说明     x           判断是否触发输出 0-触发输出 1-不触发输出
// 参数说明     *str        需要输出的 Log 信息
// 返回参数     void
// 使用示例     zf_log(0, "Error");
// 备注信息     调试信息输出 用来做一些报错或者警告之类的输出
//             默认情况下会在 Debug UART 输出
//             但如果使用开源库内屏幕接口初始化了屏幕 则会在屏幕上显示
//-------------------------------------------------------------------------------------------------------------------
#define zf_log(x, str)              (debug_log_handler((x), (str), __FILE__, __LINE__)) // 调试信息输出 用来做一些报错或者警告之类的输出

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     CPU报错接管
// 返回参数     void
// 备注信息     当触发CPU报错时会通过log信息输出来提醒用户
//-------------------------------------------------------------------------------------------------------------------
#define IFX_CFG_CPU_TRAP_BE_HOOK(x)  zf_log(0, "Memory access failure or Use an uninitialized peripheral, please check"); while(1);
#define IFX_CFG_CPU_TRAP_IPE_HOOK(x) zf_log(0, "Accessing an null address, array access may be out of bounds, please check"); while(1);


typedef struct
{
    uint16 type_index;

    uint16 display_x_max;
    uint16 display_y_max;

    uint8 font_x_size;
    uint8 font_y_size;

    void (*output_uart)             (const char *str);
    void (*output_screen)           (uint16 x, uint16 y, const char *str);
    void (*output_screen_clear)     (void);
}debug_output_struct;


#if DEBUG_UART_USE_INTERRUPT                                                            // 如果启用 debug uart 接收中断
#define     DEBUG_RING_BUFFER_LEN   (64)                                                // 定义环形缓冲区大小 默认 64byte
void        debug_interrupr_handler (void);
uint32      debug_read_ring_buffer  (uint8 *data);
#endif


void debug_assert_enable            (void);
void debug_assert_disable           (void);
void debug_assert_handler           (uint8 pass, char *file, int line);
void debug_log_handler              (uint8 pass, char *str, char *file, int line);
void debug_output_struct_init       (debug_output_struct *info);
void debug_output_init              (debug_output_struct *info);
void debug_init                     (void);

#endif

