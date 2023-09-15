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
#include "zf_common_headfile.h"

// 工程导入到软件之后，应该选中工程然后点击refresh刷新一下之后再编译
// 工程默认设置为关闭优化，可以自己右击工程选择properties->C/C++ Build->Setting
// 然后在右侧的窗口中找到C/C++ Compiler->Optimization->Optimization level处设置优化等级
// 一般默认新建立的工程都会默认开2级优化，因此大家也可以设置为2级优化

// 对于TC系列默认是不支持中断嵌套的，希望支持中断嵌套需要在中断内使用 interrupt_global_enable(0); 来开启中断嵌套
// 简单点说实际上进入中断后TC系列的硬件自动调用了 interrupt_global_disable(); 来拒绝响应任何的中断，因此需要我们自己手动调用 interrupt_global_enable(0); 来开启中断的响应。

// *************************** 例程硬件连接说明 ***********************
// 核心板正常供电即可 无需额外连接


// *************************** 例程测试说明 **************************
// 1.核心板烧录完成本例程，完成上电
// 2.可以看到核心板上两个 LED 按照相同的频率交替闪烁
// 如果发现现象与说明严重不符 请参照本文件最下方 例程常见问题说明 进行排查

// **************************** 代码区域 ****************************

// 如果是新手可以先不学习本例程，本例程主要针对单片机基础较好的同学，想自己优化RAM分配提高性能的同学参考学习
// 学习的时候请从上往下认真阅读。

// 在TC264中的两个核心，实际上他们的流水线数量是不一样的，CPU0是一级流水线，CPU1是三级流水线，因此CPU1更适合做运算，速度会比CPU0快一些。

// TC264D单片机有如下几个RAM可供用户使用
// CPU0 psram 程序RAM 大小为16KB
// CPU0 dsram 数据RAM 大小为72KB

// CPU1 psram 程序RAM 大小为32KB
// CPU1 dsram 数据RAM 大小为120KB

// TC264D中两个核心都有自己的程序缓存
// CPU0 Cache大小为8KB
// CPU1 Cache大小为16KB

// 程序RAM用来放代码，将代码放在RAM中，可以提高执行速度，特别是当周期运行的代码大小超过了cache之后效果比较显著。
// 数据RAM用来放变量，任意CPU都可以访问dsram0和dsram1，也就是说CPU0可以访问dsram1，这种行为叫做跨核访问，带来的问题就是访问延迟有所增大
// 最好的是CPU0访问dsram0，CPU1访问dsram1，这样速度最快。
// 默认情况下定义的全局变量都是保存在dsram1中的，这就会导致在CPU0中跑的代码对RAM读取会有一定的延迟

// 那么接下来我们就要学习如何指定变量存放位置

// 使用#pragma section all "cpu0_dsram" 和 #pragma section all restore可以将变量放在cpu0_dsram
// 我们只需要将变量定义放在上面两句话中间即可，使用示例如下

#pragma section all "cpu0_dsram"  // 将变量放在cpu0_dsram区域内
uint8 test_value = 0;
// 如果将cpu0_dsram改为cpu1_dsram  则变量放在cpu1_dsram区域内
// 我们建议程序由哪个CPU执行，则将所使用的变量放置在哪个区域内，默认不指定的情况下变量被放置在cpu1_dsram区域
#pragma section all restore

// 使用#pragma section all "cpu0_psram" 和 #pragma section all restore可以将函数加载到cpu0_psram
// 我们只需要将函数定义放在上面两句话中间即可，使用示例如下
#pragma section all "cpu0_psram"  // 将函数加载到cpu0_psram区域内执行
void delay_tset(void)
{
    uint32 i;
    i = 16000000;
    while(i--);
}
// 如果将cpu0_psram改为cpu1_psram  则函数加载到cpu1_psram区域内
// 我们建议程序由哪个CPU执行，则将函数加载到在哪个区域内，默认不指定的情况下，程序都是从flash加载并执行
// 通常我们都不需要将函数放在RAM运行，一般在周期执行的程序大小超过了cache后，会出现程序执行速度降低，这个时候我们就可以将部分代码放入RAM执行，可以提高运算速度
#pragma section all restore

#define LED1 P20_8
#define LED2 P20_9

int core0_main(void)
{
    clock_init();                   // 获取时钟频率<务必保留>
    debug_init();                   // 初始化默认调试串口
    // 此处编写用户代码 例如外设初始化代码等

	gpio_init(LED1, GPO, GPIO_HIGH, GPO_PUSH_PULL);
	gpio_init(LED2, GPO, GPIO_LOW, GPO_PUSH_PULL);

    // 此处编写用户代码 例如外设初始化代码等
    cpu_wait_event_ready();         // 等待所有核心初始化完毕
	while (TRUE)
	{
        // 此处编写需要循环执行的代码

	    delay_tset();
	    gpio_toggle_level(LED1);
	    gpio_toggle_level(LED2);

        // 此处编写需要循环执行的代码
	}
}
// **************************** 代码区域 ****************************

// *************************** 例程常见问题说明 ***************************
// 遇到问题时请按照以下问题检查列表检查
// 问题1：LED 不闪烁
//      查看程序是否正常烧录，是否下载报错，确认正常按下复位按键
//      万用表测量对应 LED 引脚电压是否变化，如果不变化证明程序未运行，如果变化证明 LED 灯珠损坏

