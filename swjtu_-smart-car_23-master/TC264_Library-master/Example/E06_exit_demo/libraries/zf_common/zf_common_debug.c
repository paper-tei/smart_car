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

#include "zf_common_interrupt.h"
#include "zf_common_fifo.h"
#include "zf_driver_uart.h"
#include "zf_common_debug.h"

static debug_output_struct  debug_output_info;
static volatile uint8       zf_debug_init_flag = 0;
static volatile uint8       zf_debug_assert_enable = 1;

#if DEBUG_UART_USE_INTERRUPT                                                    // 如果启用 debug uart 接收中断
uint8                       debug_uart_buffer[DEBUG_RING_BUFFER_LEN];           // 数据存放数组
uint8                       debug_uart_data;
fifo_struct                 debug_uart_fifo;
#endif

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     debug 软延时函数 在 200MHz 下是一秒多的时间 各单片机需要根据各自时钟试验
// 参数说明     void
// 返回参数     void
// 使用示例     debug_delay();
// 备注信息     本函数在文件内部调用 用户不用关注 也不可修改
//-------------------------------------------------------------------------------------------------------------------
static void debug_delay (void)
{
    vuint32 loop_1 = 0, loop_2 = 0;
    for(loop_1 = 0; loop_1 <= 0xFFF; loop_1 ++)
        for(loop_2 = 0; loop_2 <= 0x1FFF; loop_2 ++);
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     debug 串口输出接口
// 参数说明     *str        需要输出的字符串
// 返回参数     void
// 使用示例     debug_uart_str_output("Log message");
// 备注信息     本函数在文件内部调用 用户不用关注 也不可修改
//-------------------------------------------------------------------------------------------------------------------
static void debug_uart_str_output (const char *str)
{
    uart_write_string(DEBUG_UART_INDEX, str);
}



//-------------------------------------------------------------------------------------------------------------------
// 函数简介     debug 输出接口
// 参数说明     *type       log 类型
// 参数说明     *file       文件名
// 参数说明     line        目标行数
// 参数说明     *str        信息
// 返回参数     void
// 使用示例     debug_output("Log message", file, line, str);
// 备注信息     本函数在文件内部调用 用户不用关注 也不可修改
//-------------------------------------------------------------------------------------------------------------------
static void debug_output (char *type, char *file, int line, char *str)
{
    char *file_str;
    vuint16 i = 0, j = 0;
    vint32 len_origin = 0;
    vint16 show_len = 0;
    vint16 show_line_index = 0;
    len_origin = strlen(file);

    char output_buffer[256];
    char file_path_buffer[64];

    if(debug_output_info.type_index)
    {
        debug_output_info.output_screen_clear();
    }

    if(zf_debug_init_flag)
    {
        if(debug_output_info.type_index)
        {
            // 需要分行将文件的路径和行数输出
            // <不输出完整路径 只输出一级目录 例如 src/main.c>
            // 输出 line : xxxx
            debug_output_info.output_screen(0, show_line_index ++, type);

            file_str = file;
            len_origin = strlen(file);
            show_len = (debug_output_info.display_x_max / debug_output_info.font_x_size);

            while(*file_str++ != '\0');

            // 只取一级目录 如果文件放在盘符根目录 或者 MDK 的工程根目录 就会直接输出当前目录
            for(j = 0; (j < 2) && (len_origin >= 0); len_origin --)             // 查找两个 '/'
            {
                file_str --;
                if((*file_str == '/') || (*file_str == 0x5C))
                {
                    j ++;
                }
            }

            // 文件路径保存到数组中
            if(len_origin >= 0)
            {
                file_str ++;
                sprintf(output_buffer, "file: %s", file_str);
            }
            else
            {
                if(0 == j)
                {
                    sprintf(output_buffer, "file: mdk/%s", file_str);
                }
                else
                {
                    sprintf(output_buffer, "file: %s", file_str);
                }
            }

            // 屏幕显示路径
            for(i = 0; i < ((strlen(output_buffer) / show_len) + 1); i ++)
            {
                for(j = 0; j < show_len; j ++)
                {
                    if(strlen(output_buffer) < (j + i * show_len))
                    {
                        break;
                    }
                    file_path_buffer[j] = output_buffer[j + i * show_len];
                }

                file_path_buffer[j] = '\0';                                     // 末尾添加\0

                debug_output_info.output_screen(0, debug_output_info.font_y_size * show_line_index ++, file_path_buffer);
            }

            // 屏幕显示行号
            sprintf(output_buffer, "line: %d", line);
            debug_output_info.output_screen(0, debug_output_info.font_y_size * show_line_index ++, output_buffer);

            // 屏幕显示 Log 如果有的话
            if(NULL != str)
            {
                for(i = 0; i < ((strlen(str) / show_len) + 1); i ++)
                {
                    for(j = 0; j < show_len; j ++)
                    {
                        if(strlen(str) < (j + i * show_len))
                        {
                            break;
                        }
                        file_path_buffer[j] = str[j + i * show_len];
                    }

                    file_path_buffer[j] = '\0';                                 // 末尾添加\0

                    debug_output_info.output_screen(0, debug_output_info.font_y_size * show_line_index ++, file_path_buffer);
                }
            }
        }
        else
        {
            char output_buffer[256];
            memset(output_buffer, 0, 256);
            debug_output_info.output_uart(type);
            if(NULL != str)
            {
                sprintf(output_buffer, "\r\nfile %s line %d: %s.\r\n", file, line, str);
            }
            else
            {
                sprintf(output_buffer, "\r\nfile %s line %d.\r\n", file, line);
            }
            debug_output_info.output_uart(output_buffer);
        }
    }
}

#if DEBUG_UART_USE_INTERRUPT                                                    // 条件编译 只有在启用串口中断才编译

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     读取 debug 环形缓冲区数据
// 参数说明     *data       读出数据存放的数组指针
// 返回参数     uint32      读出数据的实际长度
// 使用示例     uint8 data[64]; uint32 len = debug_read_ring_buffer(data);
// 备注信息     本函数需要开启 DEBUG_UART_USE_INTERRUPT 宏定义才可使用
//-------------------------------------------------------------------------------------------------------------------
uint32 debug_read_ring_buffer (uint8 *data)
{
    uint32 data_len = sizeof(data);
    fifo_read_buffer(&debug_uart_fifo, data, &data_len, FIFO_READ_AND_CLEAN);
    return data_len;
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     debug 串口中断处理函数 isr.c 中对应串口中断服务函数调用
// 参数说明     void
// 返回参数     void
// 使用示例     debug_interrupr_handler();
// 备注信息     本函数需要开启 DEBUG_UART_USE_INTERRUPT 宏定义才可使用
//              并且本函数默认放置在 UART1 的串口接收中断处理处
//-------------------------------------------------------------------------------------------------------------------
void debug_interrupr_handler (void)
{
    if(zf_debug_init_flag)
    {
        uart_query_byte(DEBUG_UART_INDEX, &debug_uart_data);                    // 读取串口数据
        fifo_write_buffer(&debug_uart_fifo, &debug_uart_data, 1);               // 存入 FIFO
    }
}

#endif


//-------------------------------------------------------------------------     // printf 重定向 此部分不允许用户更改
//-------------------------------------------------------------------------------------------------------------------
//  @brief      重定向printf 到串口
//  @param      ch      需要打印的字节
//  @param      stream  数据流
//  @note       此函数由编译器自带库里的printf所调用
//-------------------------------------------------------------------------------------------------------------------
int fputc(int ch, FILE *stream)
{
    uart_write_byte(DEBUG_UART_INDEX, (char)ch);
    return(ch);
}
//-------------------------------------------------------------------------------------------------------------------
//  @brief      重定向printf 到串口
//  @param      ch      需要打印的字节
//  @param      stream  数据流
//  @note       此函数由编译器自带库里的printf所调用
//-------------------------------------------------------------------------------------------------------------------
int fgetc(FILE *f)
{
    return uart_read_byte(DEBUG_UART_INDEX);
}
//-------------------------------------------------------------------------     // printf 重定向 此部分不允许用户更改

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     启用断言
// 参数说明     void
// 返回参数     void
// 使用示例     debug_assert_enable();
// 备注信息     断言默认开启 建议开启断言
//-------------------------------------------------------------------------------------------------------------------
void debug_assert_enable (void)
{
    zf_debug_assert_enable = 1;
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     禁用断言
// 参数说明     void
// 返回参数     void
// 使用示例     debug_assert_disable();
// 备注信息     断言默认开启 不建议禁用断言
//-------------------------------------------------------------------------------------------------------------------
void debug_assert_disable (void)
{
    zf_debug_assert_enable = 0;
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     debug 断言处理函数
// 参数说明     pass        判断是否触发断言
// 参数说明     *file       文件名
// 参数说明     line        目标行数
// 返回参数     void
// 使用示例     zf_assert(0);
// 备注信息     这个函数不是直接调用的 此部分不允许用户更改
//             使用 zf_commmon_debug.h 中的 zf_assert(x) 接口
//-------------------------------------------------------------------------------------------------------------------
void debug_assert_handler (uint8 pass, char *file, int line)
{
    if(pass || !zf_debug_assert_enable)
    {
        return;
    }

    static uint8 assert_nest_index = 0;

    if(0 != assert_nest_index)
    {
        while(TRUE);
    }
    assert_nest_index ++;

    assert_interrupt_config();

    while(TRUE)
    {
        // 如果代码跳转到这里停住了
        // 一般你的函数参数传递出错了
        // 或者你自己调用的 zf_assert(x) 接口处报错了

        // 如果调用了 debug_init 初始化了 log 输出
        // 就在对应串口输出去查看是哪个文件的哪一行报错

        // 如果没有初始化 debug
        // 那就看看这个 file 的字符串值和 line 的行数
        // 那代表报错的文件路径名称和对应报错行数

        // 再去调试看看是为什么参数出错
        debug_output("Assert error", file, line, NULL);
        debug_delay();
    }
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     debug 调试信息处理函数
// 参数说明     pass        判断是否触发断言
// 参数说明     *str        输出的信息
// 参数说明     *file       文件名
// 参数说明     line        目标行数
// 返回参数     void
// 使用示例     zf_log(0, "Log Message");
// 备注信息     这个函数不是直接调用的 此部分不允许用户更改
//             使用 zf_commmon_debug.h 中的 zf_log(x, str) 接口
//-------------------------------------------------------------------------------------------------------------------
void debug_log_handler (uint8 pass, char *str, char *file, int line)
{
    if(pass)
    {
        return;
    }
    if(zf_debug_init_flag)
    {
        debug_output("Log message", file, line, str);
    }
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     debug 输出绑定信息初始化
// 参数说明     *info       debug 输出的信息结构体
// 返回参数     void
// 使用示例     debug_output_struct_init(info);
// 备注信息     这个函数一般不由用户调用
//-------------------------------------------------------------------------------------------------------------------
void debug_output_struct_init (debug_output_struct *info)
{
    info->type_index            = 0;                        // 屏幕信息输出标志

    info->display_x_max         = 0xFFFF;                   // 屏幕显示X轴最大值
    info->display_y_max         = 0xFFFF;                   // 屏幕显示Y轴最大值

    info->font_x_size           = 0xFF;                     // 字体X轴最大值
    info->font_y_size           = 0xFF;                     // 字体Y轴最大值

    info->output_uart           = NULL;                     // 输出调试信息的串口
    info->output_screen         = NULL;                     // 输出调试信息的屏幕
    info->output_screen_clear   = NULL;                     // 屏幕清屏
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     debug 输出绑定初始化
// 参数说明     *info       debug 输出的信息结构体
// 返回参数     void
// 使用示例     debug_output_init(info);
// 备注信息     这个函数一般不由用户调用
//-------------------------------------------------------------------------------------------------------------------
void debug_output_init (debug_output_struct *info)
{
    debug_output_info.type_index            = info->type_index;

    debug_output_info.display_x_max         = info->display_x_max;
    debug_output_info.display_y_max         = info->display_y_max;

    debug_output_info.font_x_size           = info->font_x_size;
    debug_output_info.font_y_size           = info->font_y_size;

    debug_output_info.output_uart           = info->output_uart;
    debug_output_info.output_screen         = info->output_screen;
    debug_output_info.output_screen_clear   = info->output_screen_clear;

    zf_debug_init_flag = 1;
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     debug 串口初始化
// 参数说明     void
// 返回参数     void
// 使用示例     debug_init();
// 备注信息     开源库示例默认调用 但默认禁用中断接收
//-------------------------------------------------------------------------------------------------------------------
void debug_init (void)
{
    debug_output_struct info;                   // 声明调试信息结构体
    debug_output_struct_init(&info);            // 初始化断言调试信息
    info.output_uart = debug_uart_str_output;   // 配置断言输出串口
    debug_output_init(&info);

    uart_init(DEBUG_UART_INDEX,                 // 在 zf_common_debug.h 中查看对应值
              DEBUG_UART_BAUDRATE,              // 在 zf_common_debug.h 中查看对应值
              DEBUG_UART_TX_PIN,                // 在 zf_common_debug.h 中查看对应值
              DEBUG_UART_RX_PIN);               // 在 zf_common_debug.h 中查看对应值
#if DEBUG_UART_USE_INTERRUPT                                                    // 条件编译 只有在启用串口中断才编译
    fifo_init(&debug_uart_fifo, FIFO_DATA_8BIT, debug_uart_buffer, DEBUG_RING_BUFFER_LEN);
    uart_rx_interrupt(DEBUG_UART_INDEX, 1);                                     // 使能对应串口接收中断
#endif
}



