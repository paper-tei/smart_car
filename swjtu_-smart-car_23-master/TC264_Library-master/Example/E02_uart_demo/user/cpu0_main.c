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
* 文件名称          cpu0_main
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

#include "zf_common_headfile.h"
#include "isr_config.h"
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

// *************************** 例程测试说明 ***************************
// 1.核心板烧录完成本例程，单独使用核心板与调试下载器或者 USB-TTL 模块，在断电情况下完成连接
// 2.将调试下载器或者 USB-TTL 模块连接电脑，完成上电
// 3.电脑上使用串口助手打开对应的串口，串口波特率为 DEBUG_UART_BAUDRATE 宏定义 默认 115200，核心板按下复位按键
// 4.可以在串口助手上看到如下串口信息：
//      UART Text.
// 5.通过串口助手发送数据，会收到相同的反馈数据
//      UART get data:.......
// 如果发现现象与说明严重不符 请参照本文件最下方 例程常见问题说明 进行排查

// **************************** 代码区域 ****************************
#define UART_INDEX              (DEBUG_UART_INDEX   )                           // 默认 UART_0
#define UART_BAUDRATE           (DEBUG_UART_BAUDRATE)                           // 默认 115200
#define UART_TX_PIN             (DEBUG_UART_TX_PIN  )                           // 默认 UART0_TX_P14_0
#define UART_RX_PIN             (DEBUG_UART_RX_PIN  )                           // 默认 UART0_RX_P14_1

uint8 uart_get_data[64];                                                        // 串口接收数据缓冲区
uint8 fifo_get_data[64];                                                        // fifo 输出读出缓冲区

uint8  get_data = 0;                                                            // 接收数据变量
uint32 fifo_data_count = 0;                                                     // fifo 数据个数

fifo_struct uart_data_fifo;

int core0_main(void)
{
    clock_init();                   // 获取时钟频率<务必保留>
    debug_init();                   // 初始化默认调试串口
    // 此处编写用户代码 例如外设初始化代码等

    fifo_init(&uart_data_fifo, FIFO_DATA_8BIT, uart_get_data, 64);              // 初始化 fifo 挂载缓冲区

    uart_init(UART_INDEX, UART_BAUDRATE, UART_TX_PIN, UART_RX_PIN);             // 初始化串口
    uart_rx_interrupt(UART_INDEX, 1);                                           // 开启 UART_INDEX 的接收中断

    uart_write_string(UART_INDEX, "UART Text.");                                // 输出测试信息
    uart_write_byte(UART_INDEX, '\r');                                          // 输出回车
    uart_write_byte(UART_INDEX, '\n');                                          // 输出换行

    // 此处编写用户代码 例如外设初始化代码等
    cpu_wait_event_ready();         // 等待所有核心初始化完毕
    uart_rx_interrupt(UART_INDEX, 1);
	while (TRUE)
	{
        // 此处编写需要循环执行的代码

        fifo_data_count = fifo_used(&uart_data_fifo);                           // 查看 fifo 是否有数据
        if(fifo_data_count != 0)                                                // 读取到数据了
        {
            fifo_read_buffer(&uart_data_fifo, fifo_get_data, &fifo_data_count, FIFO_READ_AND_CLEAN);    // 将 fifo 中数据读出并清空 fifo 挂载的缓冲
            uart_write_string(UART_INDEX, "\r\nUART get data:");                // 输出测试信息
            uart_write_buffer(UART_INDEX, fifo_get_data, fifo_data_count);      // 将读取到的数据发送出去
        }
        system_delay_ms(10);

        // 此处编写需要循环执行的代码
	}
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介       UART_INDEX 的接收中断处理函数 这个函数将在 UART_INDEX 对应的中断调用
// 参数说明       void
// 返回参数       void
// 使用示例       uart_rx_interrupt_handler();
//-------------------------------------------------------------------------------------------------------------------
void uart_rx_interrupt_handler (void)
{
//    get_data = uart_read_byte(UART_INDEX);                                      // 接收数据 while 等待式 不建议在中断使用
    uart_query_byte(UART_INDEX, &get_data);                                     // 接收数据 查询式 有数据会返回 TRUE 没有数据会返回 FALSE
    fifo_write_buffer(&uart_data_fifo, &get_data, 1);                           // 将数据写入 fifo 中
}


IFX_INTERRUPT(uart0_rx_isr, 0, UART0_RX_INT_PRIO)
{
    interrupt_global_enable(0);                     // 开启中断嵌套
    IfxAsclin_Asc_isrReceive(&uart0_handle);

    uart_rx_interrupt_handler();                    // 串口接收处理
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
