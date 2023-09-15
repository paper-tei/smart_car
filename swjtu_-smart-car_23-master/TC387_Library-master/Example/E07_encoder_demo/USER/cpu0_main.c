/*********************************************************************************************************************
* TC387 Opensourec Library 即（TC387 开源库）是一个基于官方 SDK 接口的第三方开源库
* Copyright (c) 2022 SEEKFREE 逐飞科技
*
* 本文件是 TC387 开源库的一部分
*
* TC387 开源库 是免费软件
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
* 文件名称          cpu0_main
* 公司名称          成都逐飞科技有限公司
* 版本信息          查看 libraries/doc 文件夹内 version 文件 版本说明
* 开发环境          ADS v1.8.0
* 适用平台          TC387QP
* 店铺链接          https://seekfree.taobao.com/
*
* 修改记录
* 日期              作者                备注
* 2022-11-04       pudding            first version
********************************************************************************************************************/

#include "isr_config.h"
#include "zf_common_headfile.h"
#pragma section all "cpu0_dsram"
// 将本语句与#pragma section all restore语句之间的全局变量都放在CPU0的RAM中


// 工程导入到软件之后，应该选中工程然后点击refresh刷新一下之后再编译
// 工程默认设置为关闭优化，可以自己右击工程选择properties->C/C++ Build->Setting
// 然后在右侧的窗口中找到C/C++ Compiler->Optimization->Optimization level处设置优化等级
// 一般默认新建立的工程都会默认开2级优化，因此大家也可以设置为2级优化

// 对于TC系列默认是不支持中断嵌套的，希望支持中断嵌套需要在中断内使用 interrupt_global_enable(0); 来开启中断嵌套
// 简单点说实际上进入中断后TC系列的硬件自动调用了 interrupt_global_disable(); 来拒绝响应任何的中断，因此需要我们自己手动调用 interrupt_global_enable(0); 来开启中断的响应。

// *************************** 例程硬件连接说明 ***************************
// 使用逐飞科技 英飞凌TriCore 调试下载器连接
//      直接将下载器正确连接在核心板的调试下载接口即可
// 使用 USB-TTL 模块连接
//      模块管脚            单片机管脚
//      USB-TTL-RX          查看 zf_common_debug.h 文件中 DEBUG_UART_TX_PIN 宏定义的引脚 默认 P14_0
//      USB-TTL-TX          查看 zf_common_debug.h 文件中 DEBUG_UART_RX_PIN 宏定义的引脚 默认 P14_1
//      USB-TTL-GND         核心板电源地 GND
//      USB-TTL-3V3         核心板 3V3 电源
// 接入正交编码器连接
//      模块管脚            单片机管脚
//      A                   ENCODER_QUADDEC_A 宏定义的引脚 默认 B4
//      B                   ENCODER_QUADDEC_B 宏定义的引脚 默认 B5
//      GND                 核心板电源地 GND
//      3V3                 核心板 3V3 电源
// 接入方向编码器连接
//      模块管脚            单片机管脚
//      LSB                 ENCODER_DIR_PULSE 宏定义的引脚 默认 B6
//      DIR                 ENCODER_DIR_DIR 宏定义的引脚 默认 B7
//      GND                 核心板电源地 GND
//      3V3                 核心板 3V3 电源



// *************************** 例程测试说明 ***************************
// 1.核心板烧录完成本例程，单独使用核心板与调试下载器或者 USB-TTL 模块，并连接好编码器，在断电情况下完成连接
// 2.将调试下载器或者 USB-TTL 模块连接电脑，完成上电
// 3.电脑上使用串口助手打开对应的串口，串口波特率为 zf_common_debug.h 文件中 DEBUG_UART_BAUDRATE 宏定义 默认 115200，核心板按下复位按键
// 4.可以在串口助手上看到如下串口信息：
//      ENCODER_QUADDEC counter     x .
//      ENCODER_DIR counter         x .
// 5.转动编码器就会看到数值变化
// 如果发现现象与说明严重不符 请参照本文件最下方 例程常见问题说明 进行排查


// **************************** 代码区域 ****************************

#define PIT0                            (CCU60_CH0 )                            // 使用的周期中断编号

#define ENCODER_QUADDEC                 (TIM5_ENCOEDER)                         // 正交编码器对应使用的编码器接口
#define ENCODER_QUADDEC_A               (TIM5_ENCOEDER_CH1_P10_3)               // A 相对应的引脚
#define ENCODER_QUADDEC_B               (TIM5_ENCOEDER_CH2_P10_1)               // B 相对应的引脚

#define ENCODER_DIR                     (TIM6_ENCOEDER)                         // 带方向编码器对应使用的编码器接口
#define ENCODER_DIR_PULSE               (TIM6_ENCOEDER_CH1_P20_3)               // PULSE 对应的引脚
#define ENCODER_DIR_DIR                 (TIM6_ENCOEDER_CH2_P20_0)               // DIR 对应的引脚

int16 encoder_data_quaddec = 0;
int16 encoder_data_dir = 0;

int core0_main(void)
{
    clock_init();                   // 获取时钟频率<务必保留>
    debug_init();                   // 初始化默认调试串口
    // 此处编写用户代码 例如外设初始化代码等

    encoder_quad_init(ENCODER_QUADDEC, ENCODER_QUADDEC_A, ENCODER_QUADDEC_B);   // 初始化编码器模块与引脚 正交解码编码器模式
    encoder_dir_init(ENCODER_DIR, ENCODER_DIR_PULSE, ENCODER_DIR_DIR);          // 初始化编码器模块与引脚 带方向增量编码器模式

    pit_ms_init(PIT0, 100);                                                     // 初始化 PIT0 为周期中断 100ms 周


    // 此处编写用户代码 例如外设初始化代码等
    cpu_wait_event_ready();         // 等待所有核心初始化完毕
	while (TRUE)
	{
        // 此处编写需要循环执行的代码

        printf("ENCODER_QUADDEC counter \t%d .\r\n", encoder_data_quaddec);     // 输出编码器计数信息
        printf("ENCODER_DIR counter \t\t%d .\r\n", encoder_data_dir);           // 输出编码器计数信息
        system_delay_ms(500);

        // 此处编写需要循环执行的代码
	}
}

IFX_INTERRUPT(cc60_pit_ch0_isr, 0, CCU6_0_CH0_ISR_PRIORITY)
{
    interrupt_global_enable(0);                     // 开启中断嵌套
    pit_clear_flag(CCU60_CH0);

    encoder_data_quaddec = encoder_get_count(ENCODER_QUADDEC);                  // 获取编码器计数
    encoder_data_dir = encoder_get_count(ENCODER_DIR);                          // 获取编码器计数

    encoder_clear_count(ENCODER_QUADDEC);                                       // 清空编码器计数
    encoder_clear_count(ENCODER_DIR);                                           // 清空编码器计数
}

#pragma section all restore
// **************************** 代码区域 ****************************
// *************************** 例程常见问题说明 ***************************
// 遇到问题时请按照以下问题检查列表检查
// 问题1：串口没有数据
//      查看串口助手打开的是否是正确的串口，检查打开的 COM 口是否对应的是调试下载器或者 USB-TTL 模块的 COM 口
//      如果是使用逐飞科技 英飞凌TriCore 调试下载器连接，那么检查下载器线是否松动，检查核心板串口跳线是否已经焊接，串口跳线查看核心板原理图即可找到
//      如果是使用 USB-TTL 模块连接，那么检查连线是否正常是否松动，模块 TX 是否连接的核心板的 RX，模块 RX 是否连接的核心板的 TX
// 问题2：串口数据乱码
//      查看串口助手设置的波特率是否与程序设置一致，程序中 zf_common_debug.h 文件中 DEBUG_UART_BAUDRATE 宏定义为 debug uart 使用的串口波特率
// 问题3：数值一直在正负一跳转
//      不要把方向编码器接在正交编码器模式的接口下
// 问题4：数值不随编码转动变化
//      如果使用主板测试，主板必须要用电池供电
//      检查编码器是否是正常的，线是否松动，编码器是否发热烧了，是否接错线
