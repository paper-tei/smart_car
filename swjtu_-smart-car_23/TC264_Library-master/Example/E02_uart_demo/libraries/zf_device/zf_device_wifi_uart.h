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

#ifndef _zf_device_wifi_uart_h_
#define _zf_device_wifi_uart_h_

#include "zf_common_typedef.h"

//=================================================WIFI_UART 驱动配置====================================================
#define WIFI_UART_INDEX         (UART_2)                                        // WIFI 模块 所使用到的串口
#define WIFI_UART_TX_PIN        (UART2_RX_P10_6)                                // 连接 WIFI 模块 TX
#define WIFI_UART_RX_PIN        (UART2_TX_P10_5)                                // 连接 WIFI 模块 RX
#define WIFI_UART_BAUD          (115200)                                        // 模块工作波特率
#define WIFI_UART_RTS_PIN       (P10_2)                                         // 定义流控位引脚  指示当前模块是否可以接受数据  0-可以继续接收  1-不可以继续接收
#define WIFI_UART_HARDWARE_RST  (1)                                             // 定义是否使用硬件复位引脚 0-使用软件复位 1-使用硬件 RST
#if WIFI_UART_HARDWARE_RST                                                      // 建议使用硬件复位引脚 否则容易出现单片机复位后无法正常初始化模块
#define WIFI_UART_RST_PIN       (P02_8)                                         // 定义硬件复位引脚
#endif
//=================================================WIFI_UART 驱动配置====================================================

//=================================================WIFI_UART 参数配置====================================================
#define WIFI_UART_BUFFER_SIZE   (256)                                           // 定义接收缓存区大小

#define WIFI_UART_AUTO_CONNECT  (0)                                             // 定义是否初始化时建立TCP或者UDP连接    0-不连接  1-自动连接TCP服务器  2-自动连接UDP服务器  3：自动建立TCP服务器

#if     (WIFI_UART_AUTO_CONNECT > 3)
#error "WIFI_UART_AUTO_CONNECT 的值只能为 [0,1,2,3]"
#else
#define WIFI_UART_TARGET_IP     "192.168.2.152"                                 // 连接目标的 IP
#define WIFI_UART_TARGET_PORT   "8080"                                          // 连接目标的端口
#define WIFI_UART_LOCAL_PORT    "8080"                                          // 本机端口
#endif
//=================================================WIFI_UART 参数配置====================================================

//=================================================WIFI_UART 参数枚举====================================================
typedef enum
{
    WIFI_UART_STATION,                                                          // 设备模式
    WIFI_UART_SOFTAP,                                                           // AP模式
}wifi_uart_mode_enum;

typedef enum
{
    WIFI_UART_COMMAND,                                                          // 使用命令的方式发送数据
    WIFI_UART_SERIANET,                                                         // 使用透传的方式发送数据
}wifi_uart_transfer_mode_enum;

typedef enum
{
    WIFI_UART_TCP_CLIENT,                                                       // 模块连接TCP服务器
    WIFI_UART_TCP_SERVER,                                                       // 模块作为TCP服务器
    WIFI_UART_UDP_CLIENT,                                                       // 模块启用UDP连接
}wifi_uart_connect_mode_enum;

typedef enum
{
    WIFI_UART_SERVER_OFF,                                                       // 模块未连接服务器
    WIFI_UART_SERVER_ON,                                                        // 模块已经连接服务器
}wifi_uart_connect_state_enum;

typedef enum
{
    WIFI_UART_LINK_0,                                                           // 模块当前链接 0
    WIFI_UART_LINK_1,                                                           // 模块当前链接 1
    WIFI_UART_LINK_2,                                                           // 模块当前链接 2
    WIFI_UART_LINK_3,                                                           // 模块当前链接 3
    WIFI_UART_LINK_4,                                                           // 模块当前链接 4
}wifi_uart_link_id_enum;

typedef struct
{
    uint8                           wifi_uart_version[12];                      // 固件版本         字符串形式
    uint8                           wifi_uart_mac[20];                          // 本机 MAC 地址    字符串形式
    uint8                           wifi_uart_local_ip[17];                     // 本机 IP 地址     字符串形式
    uint8                           wifi_uart_local_port[10];                   // 本机端口号       字符串形式
    uint8                           wifi_uart_remote_ip[5][15];                 // 远端 IP 地址     字符串形式
    wifi_uart_mode_enum             wifi_uart_mode;                             // WIFI 模式
    wifi_uart_transfer_mode_enum    wifi_uart_transfer_mode;                    // 当前传输模式
    wifi_uart_connect_mode_enum     wifi_uart_connect_mode;                     // 网络连接模式
    wifi_uart_connect_state_enum    wifi_uart_connect_state;                    // 服务器连接情况
}wifi_uart_information_struct;
//=================================================WIFI_UART 参数枚举====================================================

extern wifi_uart_information_struct wifi_uart_information;

//=================================================WIFI_UART 基础函数====================================================
uint8   wifi_uart_disconnected_wifi         (void);                                                                         // 断开 WIFI 连接
uint8   wifi_uart_entry_serianet            (void);                                                                         // 打开透传模式
uint8   wifi_uart_exit_serianet             (void);                                                                         // 关闭透传模式

uint8   wifi_uart_connect_tcp_servers       (char *ip, char *port, wifi_uart_transfer_mode_enum mode);                      // 建立 TCP 连接
uint8   wifi_uart_connect_udp_client        (char *ip, char *port, char *local_port, wifi_uart_transfer_mode_enum mode);    // 建立 UDP 传输
uint8   wifi_uart_disconnect_link           (void);                                                                         // 断开连接 TCP Server 使用本接口将会断开所有连接
uint8   wifi_uart_disconnect_link_with_id   (wifi_uart_link_id_enum link_id);                                               // TCP Server 断开指定连接 TCP/UDP Client 将不会有反应

uint8   wifi_uart_entry_tcp_servers         (char *port);                                                                   // 建立 TCP 服务器
uint8   wifi_uart_exit_tcp_servers          (void);                                                                         // 关闭 TCP 服务器
uint8   wifi_uart_tcp_servers_check_link    (void);                                                                         // TCP Server 模式下检查当前链接数量 并获取 IP

uint32  wifi_uart_send_buffer               (uint8 *buff, uint32 len);                                                      // WIFI 模块数据发送函数
uint32  wifi_uart_tcp_servers_send_buffer   (uint8 *buff, uint32 len, wifi_uart_link_id_enum id);                           // WIFI 模块作为 TCP Server 指定目标设备发送函数
uint32  wifi_uart_read_buffer               (uint8 *buff, uint32 len);                                                      // WIFI 模块数据接收函数

void    wifi_uart_callback                  (void);                                                                         // WIFI 模块串口回调函数
uint8   wifi_uart_init                      (char *wifi_ssid, char *pass_word, wifi_uart_mode_enum wifi_mode);              // WIFI 模块初始化函数
//=================================================WIFI_UART 基础函数====================================================

#endif
