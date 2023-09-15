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
* 文件名称          zf_device_wifi_uart
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
/*********************************************************************************************************************
* 接线定义：
*                   ------------------------------------
*                   模块管脚            单片机管脚
*                   RX                  查看 zf_device_wifi_uart.h 中 WIFI_UART_RX_PIN 宏定义
*                   TX                  查看 zf_device_wifi_uart.h 中 WIFI_UART_TX_PIN 宏定义
*                   RTS                 查看 zf_device_wifi_uart.h 中 WIFI_UART_RTS_PIN 宏定义
*                   RST                 查看 zf_device_wifi_uart.h 中 WIFI_UART_RST_PIN 宏定义
*                   VCC                 5V 电源
*                   GND                 电源地
*                   其余引脚悬空
*                   ------------------------------------
*********************************************************************************************************************/

#include "zf_common_clock.h"
#include "zf_common_debug.h"
#include "zf_common_fifo.h"
#include "zf_common_function.h"
#include "zf_common_interrupt.h"
#include "zf_driver_delay.h"
#include "zf_driver_gpio.h"
#include "zf_driver_uart.h"
#include "zf_device_type.h"
#include "zf_device_wifi_uart.h"

#define WAIT_TIME_OUT                   (10000)                                 // 单指令等待时间  单位：ms

wifi_uart_information_struct    wifi_uart_information;                          // 模块自身参数

static  fifo_struct             wifi_uart_fifo;
static  uint8                   wifi_uart_buffer[WIFI_UART_BUFFER_SIZE];        // 数据存放数组
static  uint8                   wifi_uart_data;

//--------------------------------------------------------------------------------------------------
// 函数简介     等待模块响应
// 参数说明     *wait_buffer    等待的响应的字符串
// 参数说明     timeout         超时时间
// 返回参数     uint8           0：模块响应指定数据   1：模块未响应指定数据或超时
// 备注信息     内部调用
//--------------------------------------------------------------------------------------------------
static uint8 wifi_uart_wait_ack (char *wait_buffer, uint32 timeout)
{
    uint8 return_state = 1;
    char receiver_buffer[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    uint32 receiver_len = 8;

    do
    {
        system_delay_ms(1);
        // 判断接收缓冲区内是否有需要响应的指定数据 如果有 则跳出循环并且返回0
        receiver_len = 8;
        fifo_read_tail_buffer(&wifi_uart_fifo, (uint8 *)receiver_buffer, &receiver_len, FIFO_READ_ONLY);
        if(strstr(receiver_buffer, wait_buffer))
        {
            return_state = 0;
            break;
        }
        else if(strstr(receiver_buffer, "ERROR") || strstr(receiver_buffer, "busy"))
        {
        // 如果接收到报错或者模块忙 则跳出循环并且返回1
            return_state = 1;
            break;
        }
    }while(timeout --);
    return return_state;
}

//--------------------------------------------------------------------------------------------------
// 函数简介     清除WiFi接收缓冲区内容
// 参数说明     void
// 返回参数     void
// 使用示例     wifi_uart_clear_receive_buffer();
// 备注信息     内部调用
//--------------------------------------------------------------------------------------------------
static void wifi_uart_clear_receive_buffer (void)
{
    // 清空WiFi接收缓冲区
    fifo_clear(&wifi_uart_fifo);
}

//--------------------------------------------------------------------------------------------------
// 函数简介     模块数据解析
// 参数说明     *target_buffer  目标存放地址指针 字符串数组
// 参数说明     *origin_buffer  数据来源地址指针 字符串数组
// 参数说明     start_char      起始提取字节 例如从 "1234" 中从 '2' 开始提取 就应该填入 '2'
// 参数说明     end_char        结束提取字节 例如从 "1234" 中在 '4' 结束提取 就应该填入 '\0'(0x00 空字符 一般是字符串结尾)
// 返回参数     uint8           0：成功   1：失败
// 使用示例     wifi_data_parse(wifi_uart_information.mac, wifi_uart_receive_buffer, '"', '"'); // 调用获取本机mac地址后，调用此函数提取mac地址
// 备注信息     内部调用
//--------------------------------------------------------------------------------------------------
static uint8 wifi_data_parse (uint8 *target_buffer, uint8 *origin_buffer, char start_char, char end_char)
{
    uint8 return_state = 0;
    char *location1;
    char *location2;
    location1 = strchr((char *)origin_buffer, start_char);
    if(location1)
    {
        location1 ++;
        location2 = strchr(location1, end_char);
        if(location2)
        {
            memcpy(target_buffer, location1, location2-location1);
        }
        else
        {
            return_state = 1;
        }
    }
    else
    {
        return_state = 1;
    }
    return return_state;
}

//--------------------------------------------------------------------------------------------------
// 函数简介     查看模块版本信息
// 参数说明     void
// 返回参数     uint8           0：成功   1：失败
// 使用示例     wifi_uart_get_version();
// 备注信息     内部调用
//--------------------------------------------------------------------------------------------------
static uint8 wifi_uart_get_version (void)
{
    char *location1;
    uint8 return_state = 0;
    uint8 receiver_buffer[256];
    uint32 receiver_len = 256;

    wifi_uart_clear_receive_buffer(); // 清空WiFi接收缓冲区
    uart_write_string(WIFI_UART_INDEX, "AT+GMR\r\n");
    do
    {

        if(wifi_uart_wait_ack("OK", WAIT_TIME_OUT))
        {
            return_state = 1;
            break;
        }

        fifo_read_buffer(&wifi_uart_fifo, receiver_buffer, &receiver_len, FIFO_READ_ONLY);
        location1 = strrchr((char *)receiver_buffer, ':');
        if(wifi_data_parse(wifi_uart_information.wifi_uart_version, (uint8 *)location1, ':', '('))
        {
            return_state = 1;
            break;
        }
    }while(0);
    wifi_uart_clear_receive_buffer();                                           // 清空WiFi接收缓冲区

    return return_state;
}

//--------------------------------------------------------------------------------------------------
// 函数简介     模块回显设置
// 参数说明     model           0:关闭模块的回写功能  其他：开启模块回写
// 返回参数     uint8           0：成功   1：失败
// 使用示例     wifi_uart_echo_set("1");//开启模块回写功能
// 备注信息     内部调用
//--------------------------------------------------------------------------------------------------
static uint8 wifi_uart_echo_set (char *model)
{
    uint8 return_state = 0;

    wifi_uart_clear_receive_buffer(); // 清空WiFi接收缓冲区
    uart_write_string(WIFI_UART_INDEX, "ATE");
    uart_write_string(WIFI_UART_INDEX, model);
    uart_write_string(WIFI_UART_INDEX, "\r\n");
    return_state = wifi_uart_wait_ack("OK", WAIT_TIME_OUT);
    wifi_uart_clear_receive_buffer(); // 清空WiFi接收缓冲区

    return return_state;
}

//--------------------------------------------------------------------------------------------------
// 函数简介     设置模块的串口配置
// 参数说明     baudrate        波特率  支持范围为 80 ~ 5000000
// 参数说明     databits        数据位  5：5 bit 数据位----6：6 bit 数据位----7：7 bit 数据位----8：8 bit 数据位
// 参数说明     stopbits        停止位  1：1 bit 停止位----2：1.5 bit 停止位----3：2 bit 停止位
// 参数说明     parity          校验位  0：None----1：Odd----2：Even
// 参数说明     flow_control    流控   0：不使能流控----1：使能 RTS----2：使能 CTS----3：同时使能 RTS 和 CTS
// 返回参数     uint8           0：成功   1：失败
// 使用示例     wifi_uart_uart_config_set("115200", "8", "1", "0", "1");
// 备注信息     内部调用 临时设置 掉电不保存
//--------------------------------------------------------------------------------------------------
static uint8 wifi_uart_uart_config_set (char *baudrate, char *databits, char *stopbits, char *parity, char *flow_control)
{
    uint8 return_state = 0;

    wifi_uart_clear_receive_buffer(); // 清空WiFi接收缓冲区
    uart_write_string(WIFI_UART_INDEX, "AT+UART_CUR=");
    uart_write_string(WIFI_UART_INDEX, baudrate);
    uart_write_string(WIFI_UART_INDEX, ",");
    uart_write_string(WIFI_UART_INDEX, databits);
    uart_write_string(WIFI_UART_INDEX, ",");
    uart_write_string(WIFI_UART_INDEX, stopbits);
    uart_write_string(WIFI_UART_INDEX, ",");
    uart_write_string(WIFI_UART_INDEX, parity);
    uart_write_string(WIFI_UART_INDEX, ",");
    uart_write_string(WIFI_UART_INDEX, flow_control);
    uart_write_string(WIFI_UART_INDEX, "\r\n");
    return_state = wifi_uart_wait_ack("OK", WAIT_TIME_OUT);
    wifi_uart_clear_receive_buffer(); // 清空WiFi接收缓冲区

    return return_state;
}

//--------------------------------------------------------------------------------------------------
// 函数简介     查询模块自身 的 MAC 地址
// 参数说明     void
// 返回参数     uint8           0：成功   1：失败
// 使用示例     if(wifi_uart_get_mac()){}
// 备注信息     内部调用
//--------------------------------------------------------------------------------------------------
static uint8 wifi_uart_get_mac (void)
{
    uint8 return_state = 0;
    uint8 receiver_buffer[64];
    uint32 receiver_len = 64;

    wifi_uart_clear_receive_buffer(); // 清空WiFi接收缓冲区
    uart_write_string(WIFI_UART_INDEX, "AT+CIPAPMAC?\r\n");
    do
    {
        if(wifi_uart_wait_ack("OK", WAIT_TIME_OUT))
        {
            return_state = 1;
            break;
        }

        fifo_read_buffer(&wifi_uart_fifo, receiver_buffer, &receiver_len, FIFO_READ_ONLY);
        if(wifi_data_parse(wifi_uart_information.wifi_uart_mac, receiver_buffer, '"', '"'))
        {
            return_state = 1;
            break;
        }
    }while(0);
    wifi_uart_clear_receive_buffer(); // 清空WiFi接收缓冲区


    return return_state;
}

//--------------------------------------------------------------------------------------------------
// 函数简介     查询模块或者目标WIFI 的 IP 地址(取决于模块当前的工作模式)
// 参数说明     void
// 返回参数     uint8           0：成功   1：失败
// 使用示例     if(wifi_uart_get_ip()){}
// 备注信息     内部调用
//--------------------------------------------------------------------------------------------------
static uint8 wifi_uart_get_ip (void)
{
    uint8 return_state = 0;

    wifi_uart_clear_receive_buffer(); // 清空WiFi接收缓冲区
    if(wifi_uart_information.wifi_uart_mode == WIFI_UART_STATION)
    {
        uart_write_string(WIFI_UART_INDEX, "AT+CIPSTA?\r\n");
    }
    else if(wifi_uart_information.wifi_uart_mode == WIFI_UART_SOFTAP)
    {
        uart_write_string(WIFI_UART_INDEX, "AT+CIPAP?\r\n");
    }

    do
    {
        if(wifi_uart_wait_ack("OK", WAIT_TIME_OUT))
        {
            return_state = 1;
            break;
        }
        uint8 receiver_buffer[128];
        uint32 receiver_len = 128;
        fifo_read_buffer(&wifi_uart_fifo, receiver_buffer, &receiver_len, FIFO_READ_ONLY);
        if(wifi_data_parse(wifi_uart_information.wifi_uart_local_ip, receiver_buffer, '"', '"'))
        {
            return_state = 1;
            break;
        }
    }while(0);
    wifi_uart_clear_receive_buffer(); // 清空WiFi接收缓冲区

    return return_state;
}

//--------------------------------------------------------------------------------------------------
// 函数简介     查询模块的相关信息
// 参数说明     void
// 返回参数     uint8           0：成功   1：失败
// 使用示例     if(wifi_uart_get_information()){}
// 备注信息     内部调用
//--------------------------------------------------------------------------------------------------
static uint8 wifi_uart_get_information (void)
{
    uint8 return_state = 0;
    do
    {
        // 获取模块版本号
        if(wifi_uart_get_version())
        {
            return_state = 1;
            break;
        }
        // 获取模块IP地址
        if(wifi_uart_get_ip())
        {
            return_state = 1;
            break;
        }
        // 获取模块MAC信息
        if(wifi_uart_get_mac())
        {
            return_state = 1;
            break;
        }
        memcpy(wifi_uart_information.wifi_uart_local_port, "no port", 7);
    }while(0);
    return return_state;
}

//--------------------------------------------------------------------------------------------------
// 函数简介     连接 WiFi
// 参数说明     wifi_ssid       WiFi名称
// 参数说明     pass_word       WiFi密码
// 参数说明     model           0:查询WiFi连接情况   其他：连接WiFi
// 返回参数     uint8           0：成功   1：失败
// 使用示例     wifi_uart_get_or_connect_wifi("WiFi_name", "Pass_word", 1);
// 备注信息     内部调用
//--------------------------------------------------------------------------------------------------
static uint8 wifi_uart_set_wifi (char *wifi_ssid, char *pass_word)
{
    uint8 return_state = 0;

    wifi_uart_clear_receive_buffer(); // 清空WiFi接收缓冲区
    if(wifi_uart_information.wifi_uart_mode == WIFI_UART_SOFTAP)
    {
        uart_write_string(WIFI_UART_INDEX, "AT+CWSAP=\"");
        uart_write_string(WIFI_UART_INDEX, wifi_ssid);
        uart_write_string(WIFI_UART_INDEX, "\",\"");
        uart_write_string(WIFI_UART_INDEX, pass_word);
        uart_write_string(WIFI_UART_INDEX, "\",5,3\r\n");
    }
    else
    {
        uart_write_string(WIFI_UART_INDEX, "AT+CWJAP=\"");
        uart_write_string(WIFI_UART_INDEX, wifi_ssid);
        uart_write_string(WIFI_UART_INDEX, "\",\"");
        uart_write_string(WIFI_UART_INDEX, pass_word);
        uart_write_string(WIFI_UART_INDEX, "\"\r\n");
    }
    return_state = wifi_uart_wait_ack("OK", WAIT_TIME_OUT);
    wifi_uart_clear_receive_buffer(); // 清空WiFi接收缓冲区

    return return_state;
}

//--------------------------------------------------------------------------------------------------
// 函数简介     上电是否自动连接WiFi
// 参数说明     model           0:上电不自动连接wifi   其他：上电自动连接wifi
// 返回参数     uint8           0：成功   1：失败
// 使用示例     wifi_uart_auto_connect_wifi(0); //上电不自动连接wifi
// 备注信息     内部调用
//--------------------------------------------------------------------------------------------------
static uint8 wifi_uart_auto_connect_wifi (char *model)
{
    uint8 return_state = 0;

    wifi_uart_clear_receive_buffer(); // 清空WiFi接收缓冲区
    uart_write_string(WIFI_UART_INDEX, "AT+CWAUTOCONN=");
    uart_write_string(WIFI_UART_INDEX, model);
    uart_write_string(WIFI_UART_INDEX, "\r\n");
    return_state = wifi_uart_wait_ack("OK", WAIT_TIME_OUT);
    wifi_uart_clear_receive_buffer(); // 清空WiFi接收缓冲区

    return return_state;
}

//--------------------------------------------------------------------------------------------------
// 函数简介     设置连接模式
// 参数说明     model           0: 单连接模式     1：多连接模式
// 返回参数     uint8           0：成功   1：失败
// 使用示例     wifi_uart_set_connect_model("1");
// 备注信息     内部调用
//--------------------------------------------------------------------------------------------------
static uint8 wifi_uart_set_connect_model (char *model)
{
    uint8 return_state = 0;

    wifi_uart_clear_receive_buffer(); // 清空WiFi接收缓冲区
    uart_write_string(WIFI_UART_INDEX, "AT+CIPMUX=");
    uart_write_string(WIFI_UART_INDEX, model);
    uart_write_string(WIFI_UART_INDEX, "\r\n");
    return_state = wifi_uart_wait_ack("OK", WAIT_TIME_OUT);
    wifi_uart_clear_receive_buffer(); // 清空WiFi接收缓冲区

    return return_state;
}

//--------------------------------------------------------------------------------------------------
// 函数简介     设置传输模式
// 参数说明     model           – 0: 普通传输模式     IP断开后不重新连接
//                              – 1: Wi-Fi 透传接收模式，仅支持 TCP 单连接、UDP 固定通信对端、SSL 单连接的情况     IP断开后会不断尝试重新连接
// 返回参数     uint8           0：成功   1：失败
// 使用示例     wifi_uart_set_transfer_model("1");
// 备注信息     内部调用
//--------------------------------------------------------------------------------------------------
static uint8 wifi_uart_set_transfer_model (char *model)
{
    uint8 return_state = 0;

    wifi_uart_clear_receive_buffer(); // 清空WiFi接收缓冲区
    uart_write_string(WIFI_UART_INDEX, "AT+CIPMODE=");
    uart_write_string(WIFI_UART_INDEX, model);
    uart_write_string(WIFI_UART_INDEX, "\r\n");
    return_state = wifi_uart_wait_ack("OK", WAIT_TIME_OUT);
    wifi_uart_clear_receive_buffer(); // 清空WiFi接收缓冲区

    return return_state;
}

//--------------------------------------------------------------------------------------------------
// 函数简介     模块软件复位
// 参数说明     void
// 返回参数     uint8           0：成功   1：失败
// 使用示例     wifi_uart_soft_reset();
// 备注信息
//--------------------------------------------------------------------------------------------------
uint8 wifi_uart_soft_reset (void)
{
    uint8 return_state = 0;

    wifi_uart_clear_receive_buffer(); // 清空WiFi接收缓冲区
    uart_write_string(WIFI_UART_INDEX, "+++");
    system_delay_ms(100);
    uart_write_string(WIFI_UART_INDEX, "\r\n");
    system_delay_ms(100);
    wifi_uart_clear_receive_buffer(); // 清空WiFi接收缓冲区
    uart_write_string(WIFI_UART_INDEX, "AT+RST\r\n");
    return_state = wifi_uart_wait_ack("ready", WAIT_TIME_OUT);
    wifi_uart_clear_receive_buffer(); // 清空WiFi接收缓冲区

    return return_state;
}

//--------------------------------------------------------------------------------------------------
// 函数简介     模块硬件复位
// 参数说明     void
// 返回参数     uint8           0：成功   1：失败
// 使用示例     wifi_uart_reset();
// 备注信息
//--------------------------------------------------------------------------------------------------
uint8 wifi_uart_reset (void)
{
#if WIFI_UART_HARDWARE_RST
    uint8 return_state = 0;

    gpio_set_level(WIFI_UART_RST_PIN, 0);
    system_delay_ms(50);
    gpio_set_level(WIFI_UART_RST_PIN, 1);
    system_delay_ms(200);
    wifi_uart_clear_receive_buffer(); // 清空WiFi接收缓冲区
    return_state = wifi_uart_wait_ack("ready", WAIT_TIME_OUT);
    wifi_uart_clear_receive_buffer(); // 清空WiFi接收缓冲区

    return return_state;
#else
    return wifi_uart_soft_reset();
#endif
}

//--------------------------------------------------------------------------------------------------
// 函数简介     设置模块模式 (Station/SoftAP/Station+SoftAP)
// 参数说明     state           0:无 Wi-Fi 模式，并且关闭 Wi-Fi RF----1: Station 模式----2: SoftAP 模式----3: SoftAP+Station 模式
// 返回参数     uint8           0：成功   1：失败
// 使用示例     wifi_uart_set_model("1");
// 备注信息
//--------------------------------------------------------------------------------------------------
uint8 wifi_uart_set_model (wifi_uart_mode_enum  mode)
{
    uint8 return_state = 0;

    wifi_uart_clear_receive_buffer(); // 清空WiFi接收缓冲区
    if(mode == WIFI_UART_SOFTAP)
    {
        uart_write_string(WIFI_UART_INDEX, "AT+CWMODE=2\r\n");
    }
    else
    {
        uart_write_string(WIFI_UART_INDEX, "AT+CWMODE=1\r\n");
    }
    // 设置模块工作模式
    wifi_uart_information.wifi_uart_mode = mode;
    return_state = wifi_uart_wait_ack("OK", WAIT_TIME_OUT);
    wifi_uart_clear_receive_buffer(); // 清空WiFi接收缓冲区

    return return_state;
}

//--------------------------------------------------------------------------------------------------
// 函数简介     断开与wifi的连接
// 参数说明     void
// 返回参数     uint8           0：成功   1：失败
// 使用示例     wifi_uart_disconnected_wifi();
// 备注信息
//--------------------------------------------------------------------------------------------------
uint8 wifi_uart_disconnected_wifi (void)
{
    uint8 return_state = 0;

    wifi_uart_clear_receive_buffer(); // 清空WiFi接收缓冲区
    uart_write_string(WIFI_UART_INDEX, "AT+CWQAP\r\n");
    return_state = wifi_uart_wait_ack("OK", WAIT_TIME_OUT);
    wifi_uart_clear_receive_buffer(); // 清空WiFi接收缓冲区

    return return_state;
}

//--------------------------------------------------------------------------------------------------
// 函数简介     进入打开透传模式
// 参数说明     void
// 返回参数     uint8           0：成功   1：失败
// 使用示例     wifi_uart_entry_serianet();
// 备注信息
//--------------------------------------------------------------------------------------------------
uint8 wifi_uart_entry_serianet (void)
{
    uint8 return_state = 0;

    wifi_uart_clear_receive_buffer(); // 清空WiFi接收缓冲区
    uart_write_string(WIFI_UART_INDEX, "AT+CIPSEND\r\n");
    return_state = wifi_uart_wait_ack("OK", WAIT_TIME_OUT);
    wifi_uart_clear_receive_buffer(); // 清空WiFi接收缓冲区

    return return_state;
}

//--------------------------------------------------------------------------------------------------
// 函数简介     退出透传模式
// 参数说明     model           0:关闭透传模式   其他：开启透传模式
// 返回参数     uint8           0：成功   1：失败
// 使用示例    wifi_uart_exit_serianet();
// 备注信息
//--------------------------------------------------------------------------------------------------
uint8 wifi_uart_exit_serianet (void)
{
    wifi_uart_clear_receive_buffer(); // 清空WiFi接收缓冲区

    system_delay_ms(20);
    uart_write_string(WIFI_UART_INDEX, "+++");
    system_delay_ms(1000);

    return 0;
}

//--------------------------------------------------------------------------------------------------
// 函数简介     建立TCP连接
// 参数说明     ip              远端 IPv4 地址、IPv6 地址，或域名
// 参数说明     port            远端端口值
// 返回参数     uint8           0：成功   1：失败
// 使用示例     wifi_uart_connect_tcp_servers("192.168.101.110", "8080");
// 备注信息     如果总是连接不上电脑的TCP服务器 可以尝试使用网线连接电脑
//              如果是使用WiFi连接 可能会导致模块连接TCP服务器等待较长时间
//--------------------------------------------------------------------------------------------------
uint8 wifi_uart_connect_tcp_servers (char *ip, char *port, wifi_uart_transfer_mode_enum mode)
{
    zf_assert(ip != NULL);
    zf_assert(port != NULL);

    uint8 return_state = 0;

    wifi_uart_clear_receive_buffer(); // 清空WiFi接收缓冲区
    do
    {
        if(wifi_uart_set_connect_model("0"))
        {
            return_state = 1;
            break;
        }

        wifi_uart_clear_receive_buffer(); // 清空WiFi接收缓冲区

        uart_write_string(WIFI_UART_INDEX, "AT+CIPSTARTEX=\"TCP\",\"");
        uart_write_string(WIFI_UART_INDEX, ip);
        uart_write_string(WIFI_UART_INDEX, "\",");
        uart_write_string(WIFI_UART_INDEX, port);
        uart_write_string(WIFI_UART_INDEX, "\r\n");
        if(wifi_uart_wait_ack("OK", WAIT_TIME_OUT))
        {
            return_state = 1;
            wifi_uart_information.wifi_uart_connect_state = WIFI_UART_SERVER_OFF;
            break;
        }

        wifi_uart_clear_receive_buffer(); // 清空WiFi接收缓冲区

        // 设置传输模式
        if(wifi_uart_set_transfer_model(mode == WIFI_UART_COMMAND ? "0" : "1"))
        {
            return_state = 1;
            break;
        }

        wifi_uart_clear_receive_buffer(); // 清空WiFi接收缓冲区
        uart_write_string(WIFI_UART_INDEX, "AT+CIPSTATE?\r\n");
        if(wifi_uart_wait_ack("OK", WAIT_TIME_OUT))
        {
            return_state = 1;
            break;
        }
        else
        {
            uint8 receiver_buffer[128];
            uint32 receiver_len = 128;
            fifo_read_buffer(&wifi_uart_fifo, receiver_buffer, &receiver_len, FIFO_READ_ONLY);
            char* buffer_index = (char *)receiver_buffer;
            char* end_index;

            buffer_index += 22;
            buffer_index += strlen(ip);
            buffer_index += strlen(port);
            end_index = strchr(buffer_index, ',');

            memcpy(wifi_uart_information.wifi_uart_local_port, "       ", 9);
            memcpy(wifi_uart_information.wifi_uart_local_port, buffer_index, (end_index - buffer_index));
        }

        wifi_uart_information.wifi_uart_connect_state = WIFI_UART_SERVER_ON;
        wifi_uart_information.wifi_uart_connect_mode = WIFI_UART_TCP_CLIENT;
        wifi_uart_information.wifi_uart_transfer_mode = mode;

        wifi_uart_clear_receive_buffer();       // 清空WiFi接收缓冲区
        if(mode == WIFI_UART_SERIANET)          // 透传模式下直接开启透传
        {
            if(wifi_uart_entry_serianet())
            {
                return_state = 1;
                break;
            }
        }
        wifi_uart_send_buffer((uint8 *)"TCP connect!", 12);
    }while(0);
    wifi_uart_clear_receive_buffer();           // 清空WiFi接收缓冲区

    return return_state;
}

//--------------------------------------------------------------------------------------------------
// 函数简介     建立UDP连接
// 参数说明     *ip             远端 IPv4 地址、IPv6 地址 或域名 字符串形式
// 参数说明     *port           远端端口值 字符串形式
// 参数说明     *local_port     远端 IPv4 地址、IPv6 地址 或域名 字符串形式
// 参数说明     mode            模块数据通信模式
// 返回参数     uint8           0：成功   1：失败
// 使用示例     wifi_uart_connect_udp_client("192.168.101.110", "8080", "8080", WIFI_UART_COMMAND);
// 备注信息     自动分配ID
//--------------------------------------------------------------------------------------------------
uint8 wifi_uart_connect_udp_client (char *ip, char *port, char *local_port, wifi_uart_transfer_mode_enum mode)
{
    zf_assert(ip != NULL);
    zf_assert(port != NULL);
    zf_assert(local_port != NULL);
    uint8 return_state = 0;

    wifi_uart_clear_receive_buffer();                                           // 清空WiFi接收缓冲区
    do
    {
        if(wifi_uart_set_connect_model("0"))
        {
            return_state = 1;
            break;
        }

        wifi_uart_clear_receive_buffer();                                       // 清空WiFi接收缓冲区

        uart_write_string(WIFI_UART_INDEX, "AT+CIPSTARTEX=\"UDP\",\"");
        uart_write_string(WIFI_UART_INDEX, ip);
        uart_write_string(WIFI_UART_INDEX, "\",");
        uart_write_string(WIFI_UART_INDEX, port);
        uart_write_string(WIFI_UART_INDEX, ",");
        uart_write_string(WIFI_UART_INDEX, local_port);
        uart_write_string(WIFI_UART_INDEX, "\r\n");

        if(wifi_uart_wait_ack("OK", WAIT_TIME_OUT))
        {
            return_state = 1;
            wifi_uart_information.wifi_uart_connect_state = WIFI_UART_SERVER_OFF;
            break;
        }

        wifi_uart_clear_receive_buffer();                                       // 清空WiFi接收缓冲区
        if(wifi_uart_set_transfer_model(mode == WIFI_UART_COMMAND ? "0" : "1")) // 设置传输模式
        {
            return_state = 1;
            break;
        }

        wifi_uart_clear_receive_buffer();                                       // 清空WiFi接收缓冲区
        if(mode == WIFI_UART_SERIANET)                                          // 透传模式下直接开启透传
        {
            if(wifi_uart_entry_serianet())
            {
                return_state = 1;
                break;
            }
        }
        memcpy(wifi_uart_information.wifi_uart_local_port, "       ", 7);
        memcpy(wifi_uart_information.wifi_uart_local_port, local_port, strlen(local_port));
        wifi_uart_information.wifi_uart_connect_state = WIFI_UART_SERVER_ON;
        wifi_uart_information.wifi_uart_connect_mode  = WIFI_UART_UDP_CLIENT;
        wifi_uart_information.wifi_uart_transfer_mode = mode;
        wifi_uart_send_buffer((uint8 *)"UDP connect!", 12);
    }while(0);
    wifi_uart_clear_receive_buffer(); // 清空WiFi接收缓冲区

    return return_state;
}

//--------------------------------------------------------------------------------------------------
// 函数简介     断开连接 TCP Server 使用本接口将会断开所有连接
// 参数说明     void
// 返回参数     uint8           0：成功   1：失败
// 使用示例     wifi_uart_disconnect_link();
// 备注信息
//--------------------------------------------------------------------------------------------------
uint8 wifi_uart_disconnect_link (void)
{
    uint8 return_state = 0;

    wifi_uart_clear_receive_buffer();                                           // 清空WiFi接收缓冲区
    do
    {
        if(WIFI_UART_TCP_SERVER == wifi_uart_information.wifi_uart_connect_mode)
        {
            uart_write_string(WIFI_UART_INDEX, "AT+CIPCLOSE=5\r\n");
        }
        else
        {
            uart_write_string(WIFI_UART_INDEX, "AT+CIPCLOSE\r\n");
        }

        if(wifi_uart_wait_ack("OK", WAIT_TIME_OUT))
        {
            return_state = 1;
            wifi_uart_information.wifi_uart_connect_state = WIFI_UART_SERVER_OFF;
            break;
        }
    }while(0);
    wifi_uart_clear_receive_buffer(); // 清空WiFi接收缓冲区

    return return_state;
}

//--------------------------------------------------------------------------------------------------
// 函数简介     TCP Server 断开指定连接 TCP/UDP Client 将不会有反应
// 参数说明     link_id         将要断开的目标连接
// 返回参数     uint8           0：成功   1：失败
// 使用示例     wifi_uart_disconnect_link_with_id(WIFI_UART_LINK_0);
// 备注信息
//--------------------------------------------------------------------------------------------------
uint8 wifi_uart_disconnect_link_with_id (wifi_uart_link_id_enum link_id)
{
    uint8 return_state = 0;

    wifi_uart_clear_receive_buffer();                                           // 清空WiFi接收缓冲区
    do
    {
        if(WIFI_UART_TCP_SERVER == wifi_uart_information.wifi_uart_connect_mode)
        {
            uart_write_string(WIFI_UART_INDEX, "AT+CIPCLOSE=");
            uart_write_byte(WIFI_UART_INDEX, link_id + 0x30);
            uart_write_string(WIFI_UART_INDEX, "\r\n");
        }
        else
        {
            return_state = 1;
            break;
        }

        if(wifi_uart_wait_ack("OK", WAIT_TIME_OUT))
        {
            return_state = 1;
            wifi_uart_information.wifi_uart_connect_state = WIFI_UART_SERVER_OFF;
            break;
        }
    }while(0);
    wifi_uart_clear_receive_buffer(); // 清空WiFi接收缓冲区

    return return_state;
}

//--------------------------------------------------------------------------------------------------
// 函数简介     建立 TCP 服务器
// 参数说明     *port           端口值 字符串形式
// 返回参数     uint8           0：成功   1：失败
// 使用示例     wifi_uart_entry_tcp_servers("80");
// 备注信息     自动分配ID
//--------------------------------------------------------------------------------------------------
uint8 wifi_uart_entry_tcp_servers (char *port)
{
    zf_assert(port != NULL);
    uint8 return_state = 0;

    wifi_uart_clear_receive_buffer();                                           // 清空WiFi接收缓冲区
    do
    {
        if(wifi_uart_set_transfer_model("0"))                                   // 设置传输模式为普通传输模式
        {
            return_state = 1;
            break;
        }
        wifi_uart_clear_receive_buffer();                                       // 清空WiFi接收缓冲区

        if(wifi_uart_set_connect_model("1"))                                    // 设置连接模式为多连接模式
        {
            return_state = 1;
            break;
        }
        wifi_uart_clear_receive_buffer();                                       // 清空WiFi接收缓冲区

        uart_write_string(WIFI_UART_INDEX, "AT+CIPSERVER=1,");
        uart_write_string(WIFI_UART_INDEX, port);
        uart_write_string(WIFI_UART_INDEX, "\r\n");

        if(wifi_uart_wait_ack("OK", WAIT_TIME_OUT))
        {
            return_state = 1;
            wifi_uart_information.wifi_uart_connect_state = WIFI_UART_SERVER_OFF;
            break;
        }
        memcpy(wifi_uart_information.wifi_uart_local_port, "       ", 7);
        memcpy(wifi_uart_information.wifi_uart_local_port, port, strlen(port));
        wifi_uart_information.wifi_uart_connect_state = WIFI_UART_SERVER_ON;
        wifi_uart_information.wifi_uart_transfer_mode = WIFI_UART_COMMAND;
        wifi_uart_information.wifi_uart_connect_mode = WIFI_UART_TCP_SERVER;
    }while(0);
    wifi_uart_clear_receive_buffer(); // 清空WiFi接收缓冲区

    return return_state;
}

//--------------------------------------------------------------------------------------------------
// 函数简介     关闭 TCP 服务器
// 参数说明     void
// 返回参数     uint8           0：成功   1：失败
// 使用示例     wifi_uart_exit_tcp_servers();
// 备注信息
//--------------------------------------------------------------------------------------------------
uint8 wifi_uart_exit_tcp_servers (void)
{
    uint8 return_state = 0;

    wifi_uart_clear_receive_buffer();                                           // 清空WiFi接收缓冲区
    uart_write_string(WIFI_UART_INDEX, "AT+CIPSERVER=0,1\r\n");
    return_state = wifi_uart_wait_ack("OK", WAIT_TIME_OUT);
    wifi_uart_clear_receive_buffer();                                           // 清空WiFi接收缓冲区

    return return_state;
}

//--------------------------------------------------------------------------------------------------
// 函数简介     TCP Server 模式下检查当前链接数量 并获取 IP
// 参数说明     void
// 返回参数     uint8           当前建立的连接数量
// 使用示例     wifi_uart_tcp_servers_check_link();
// 备注信息
//--------------------------------------------------------------------------------------------------
uint8 wifi_uart_tcp_servers_check_link (void)
{
    uint8 return_value = 0;
    uint8 loop_temp = 0;
    uint8 linke_index = 0;

    uint8 receiver_buffer[256];
    uint32 receiver_len = 256;

    char* buffer_index;
    char* start_index;
    char* end_index;

    for(loop_temp = 0; loop_temp < 5; loop_temp ++)
    {
        memset(wifi_uart_information.wifi_uart_remote_ip[loop_temp], 0, 15);
    }

    wifi_uart_clear_receive_buffer();                                           // 清空WiFi接收缓冲区
    uart_write_string(WIFI_UART_INDEX, "AT+CIPSTATE?\r\n");

    if(wifi_uart_wait_ack("OK", WAIT_TIME_OUT) == 0)
    {
        fifo_read_buffer(&wifi_uart_fifo, receiver_buffer, &receiver_len, FIFO_READ_ONLY);
        buffer_index = (char *)receiver_buffer;
        for(loop_temp = 0; loop_temp < 5; loop_temp ++)
        {
            start_index = strchr(buffer_index, ':');
            if(NULL == start_index)
            {
                break;
            }
            start_index ++;
            linke_index = *(start_index) - 0x30;
            start_index += 9;
            end_index = strchr((const char *)(start_index), '"');
            memset(wifi_uart_information.wifi_uart_remote_ip[linke_index], 0, 15);
            memcpy(wifi_uart_information.wifi_uart_remote_ip[linke_index], start_index, (end_index - start_index));
            buffer_index = end_index;
        }
    }
    wifi_uart_clear_receive_buffer();                                           // 清空WiFi接收缓冲区
    return return_value;
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     WiFi 模块 发送函数
// 参数说明     buff            需要发送的数据地址
// 参数说明     len             发送长度
// 返回参数     uint16          剩余未发送数据长度
// 使用示例     wifi_uart_send_buffer("123", 3);
// 备注信息     当模块作为TCP服务器时，发送数据函数默认将数据发送至第一个连接模块的客户端
//-------------------------------------------------------------------------------------------------------------------
uint32 wifi_uart_send_buffer (uint8 *buff, uint32 len)
{
    zf_assert(buff != NULL);
    int32 timeout = WAIT_TIME_OUT;

    char lenth[32] = {0};

    if(wifi_uart_information.wifi_uart_connect_state == WIFI_UART_SERVER_ON)
    {
        if(wifi_uart_information.wifi_uart_transfer_mode == WIFI_UART_COMMAND)
        {
            wifi_uart_clear_receive_buffer();                                   // 清空WiFi接收缓冲区

            func_int_to_str(lenth,len);
            if(len > 8192)
            {
                uart_write_string(WIFI_UART_INDEX, "AT+CIPSENDL=");
            }
            else
            {
                uart_write_string(WIFI_UART_INDEX, "AT+CIPSEND=");
            }
            if(wifi_uart_information.wifi_uart_connect_mode == WIFI_UART_TCP_SERVER)
            {
                uart_write_string(WIFI_UART_INDEX, "0,");
            }

            uart_write_string(WIFI_UART_INDEX, lenth);
            uart_write_string(WIFI_UART_INDEX, "\r\n");

            if(wifi_uart_wait_ack("OK", WAIT_TIME_OUT) == 0)                    // 等待模块响应
            {
                wifi_uart_clear_receive_buffer();                               // 清空WiFi接收缓冲区
                uart_write_buffer(WIFI_UART_INDEX, buff, len);
                wifi_uart_clear_receive_buffer();                               // 清空WiFi接收缓冲区
                if(wifi_uart_wait_ack("OK", WAIT_TIME_OUT) == 0)                // 等待模块响应
                {
                    len = 0;
                }
                wifi_uart_clear_receive_buffer();                               // 清空WiFi接收缓冲区
            }

        }
        else
        {
            while(len--)
            {
                while(gpio_get_level(WIFI_UART_RTS_PIN) && 0 < timeout -- );    // 如果RTS为低电平，则发送数据
                if(0 >= timeout)
                {
                    break;
                }
                uart_write_byte(WIFI_UART_INDEX, *buff);                        // 发送最后的数据
                buff ++;
            }
        }
    }
    wifi_uart_clear_receive_buffer();                                           // 清空WiFi接收缓冲区
    return len;
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     WiFi 模块作为 TCP 服务器 向指定目标设备发送函数
// 参数说明     buff            需要发送的数据地址
// 参数说明     len             发送长度
// 参数说明     id              目标 client id
// 返回参数     uint16          剩余未发送数据长度
// 使用示例     wifi_uart_tcp_servers_send_buffer("123", 3, WIFI_UART_LINK_0);
// 备注信息     当模块作为TCP服务器时，发送数据函数默认将数据发送至第一个连接模块的客户端
//-------------------------------------------------------------------------------------------------------------------
uint32 wifi_uart_tcp_servers_send_buffer (uint8 *buff, uint32 len, wifi_uart_link_id_enum id)
{
    zf_assert(buff != NULL);
    char lenth[32] = {0};

    if( wifi_uart_information.wifi_uart_transfer_mode == WIFI_UART_COMMAND && \
        wifi_uart_information.wifi_uart_connect_mode == WIFI_UART_TCP_SERVER)
    {
        wifi_uart_clear_receive_buffer();                                       // 清空WiFi接收缓冲区

        func_int_to_str(lenth,len);
        if(len > 8192)
        {
            uart_write_string(WIFI_UART_INDEX, "AT+CIPSENDL=");
        }
        else
        {
            uart_write_string(WIFI_UART_INDEX, "AT+CIPSEND=");
        }

        uart_write_byte(WIFI_UART_INDEX, (id + '0'));
        uart_write_string(WIFI_UART_INDEX, ",");

        uart_write_string(WIFI_UART_INDEX, lenth);
        uart_write_string(WIFI_UART_INDEX, "\r\n");

        if(wifi_uart_wait_ack("OK", WAIT_TIME_OUT) == 0)                        // 等待模块响应
        {
            // 模块允许发送数据
            wifi_uart_clear_receive_buffer();                                   // 清空WiFi接收缓冲区
            uart_write_buffer(WIFI_UART_INDEX, buff, len);
            if(wifi_uart_wait_ack("OK", WAIT_TIME_OUT) == 0)                    // 等待模块响应
            {
                len = 0;
            }
        }
    }
    wifi_uart_clear_receive_buffer();                                           // 清空WiFi接收缓冲区
    return len;
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     WiFi 模块数据接收函数
// 参数说明     buffer          接收数据的存放地址
// 参数说明     len             数组长度，可直接填写或者使用sizeof求得
// 返回参数     uint16          返回实际接收到的数据长度
// 使用示例     uint8 test_buffer[256]; wifi_uart_read_buffer(&test_buffer[0], sizeof(test_buffer));
// 备注信息
//-------------------------------------------------------------------------------------------------------------------
uint32 wifi_uart_read_buffer (uint8 *buffer, uint32 len)
{
    zf_assert(buffer != NULL);
    uint32 read_len = len;
    fifo_read_buffer(&wifi_uart_fifo, buffer, &read_len, FIFO_READ_AND_CLEAN);
    return read_len;
}

//--------------------------------------------------------------------------------------------------
// 函数简介     WiFi 串口回调函数
// 参数说明     void
// 返回参数     void
// 使用示例     wireless_uart_callback();
// 备注信息     该函数在 ISR 文件 串口中断程序被调用
//              由串口中断服务函数调用 wireless_module_uart_handler() 函数
//              再由 wireless_module_uart_handler() 函数调用本函数
//--------------------------------------------------------------------------------------------------
void wifi_uart_callback (void)
{
    uart_query_byte(WIFI_UART_INDEX, &wifi_uart_data);                          // 读取串口数据
    fifo_write_buffer(&wifi_uart_fifo, &wifi_uart_data, 1);                     // 存入 FIFO
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     WiFi 模块初始化
// 参数说明     *wifi_ssid      目标连接的 WiFi 的名称 字符串形式
// 参数说明     *pass_word      目标连接的 WiFi 的密码 字符串形式
// 参数说明     wifi_mode       模块的工作模式 参照 zf_device_wireless_uart.h 中 wifi_uart_mode_enum 枚举
// 返回参数     uint8           模块初始化状态 0-成功 1-错误
// 使用示例     wifi_uart_init("SEEKFREE_2.4G", "SEEKFREEV2", WIFI_UART_STATION);
// 备注信息     初始化会首先设置串口配置，之后会对模块进行基本参数配置
//              具体的配置信息可以在 zf_device_wireless_uart.h 文件中修改
//-------------------------------------------------------------------------------------------------------------------
uint8 wifi_uart_init (char *wifi_ssid, char *pass_word, wifi_uart_mode_enum wifi_mode)
{
    zf_assert(wifi_ssid != NULL);
    zf_assert(pass_word != NULL);
    char uart_baud[32] = {0};
    uint8 return_state = 0;

    // 设置模块类型
    set_wireless_type(WIFI_UART, wifi_uart_callback);
    fifo_init(&wifi_uart_fifo, FIFO_DATA_8BIT, wifi_uart_buffer, WIFI_UART_BUFFER_SIZE);
    gpio_init(WIFI_UART_RTS_PIN, GPI, 0, GPI_PULL_UP);                          // 初始化流控引脚
#if WIFI_UART_HARDWARE_RST
    gpio_init(WIFI_UART_RST_PIN, GPO, 1, GPO_PUSH_PULL);                        // 初始化复位引脚
#endif
    uart_init(WIFI_UART_INDEX, 115200, WIFI_UART_RX_PIN, WIFI_UART_TX_PIN);     // 初始化WiFi模块所使用的串口
    uart_rx_interrupt(WIFI_UART_INDEX, 1);
    do
    {
        if(wifi_uart_reset())                                                   // 重启模块
        {
            // 检查一下 RST 引脚的连接
            // 如果没有接 RST 引脚又启用了硬件复位
            // 就会一直报错
            // 如果禁用了硬件复位 使用软件复位
            // 反复报错无法复位的话就断电重启一下
            zf_log(0, "reset failed");
            return_state = 1;
            break;
        }
        func_int_to_str(uart_baud, WIFI_UART_BAUD);                             // 更改WiFi模块所使用的波尔率参数
        if(wifi_uart_uart_config_set(uart_baud, "8", "1", "0", "1"))            // 调用接口重设模块的工作串口参数
        {
            zf_log(0, "set config failed");
            return_state = 1;
            break;
        }
        // 重新初始化WiFi模块所使用的串口
        uart_init(WIFI_UART_INDEX, WIFI_UART_BAUD, WIFI_UART_RX_PIN, WIFI_UART_TX_PIN);
        uart_rx_interrupt(WIFI_UART_INDEX, 1);
        system_delay_ms(100);

        if(wifi_uart_echo_set("0"))                                             // 关闭模块回写
        {
            zf_log(0, "exit echo failed");
            return_state = 1;
            break;
        }

        if(wifi_uart_auto_connect_wifi("0"))                                    // 关闭自动连接
        {
            zf_log(0, "close auto connect failed");
            return_state = 1;
            break;
        }

        if(wifi_uart_set_model(wifi_mode))                                      // 设置运行模式
        {
            zf_log(0, "set run mode failed");
            return_state = 1;
            break;
        }

        if(wifi_uart_set_wifi((char *)wifi_ssid, (char *)pass_word))            // 连接 wifi 或者开启热点
        {
            zf_log(0, "wifi set failed");
            return_state = 1;
            break;
        }

        if(wifi_uart_get_information())                                         // 模块基本参数获取
        {
            zf_log(0, "get module information failed");
            return_state = 1;
            break;
        }
#if WIFI_UART_AUTO_CONNECT == 1
        if(wifi_uart_connect_tcp_servers(WIFI_UART_TARGET_IP, WIFI_UART_TARGET_PORT,WIFI_UART_COMMAND))                         // 连接TCP服务器
        {
            zf_log(0, "connect TCP server failed");
            return_state = 1;
            break;
        }
#endif
#if WIFI_UART_AUTO_CONNECT == 2
        if(wifi_uart_connect_udp_transfer(WIFI_UART_TARGET_IP, WIFI_UART_TARGET_PORT, WIFI_UART_MINE_PORT, WIFI_UART_SERIANET)) // 建立UDP连接
        {
            zf_log(0, "connect UDP server failed");
            return_state = 1;
            break;
        }
#endif
#if WIFI_UART_AUTO_CONNECT == 3
        if(wifi_uart_creat_tcp_servers(WIFI_UART_MINE_PORT))                                                                    // 建立TCP服务器
        {
            zf_log(0, "build TCP server failed");
            return_state = 1;
            break;
        }
#endif
    }while(0);

    wifi_uart_clear_receive_buffer();                                           // 清空WiFi接收缓冲区
    return return_state;
}
