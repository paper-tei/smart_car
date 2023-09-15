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
* 文件名称          zf_device_bluetooth_ch9141
* 公司名称          成都逐飞科技有限公司
* 版本信息          查看 libraries/doc 文件夹内 version 文件 版本说明
* 开发环境          ADS v1.8.0
* 适用平台          TC264D
* 店铺链接          https://seekfree.taobao.com/
*
* 修改记录
* 日期             作者                备注
* 2022-09-15      pudding            first version
********************************************************************************************************************/
/*********************************************************************************************************************
* 接线定义：
*                 ------------------------------------
*                 蓝牙转串口           单片机
*                 RX                 查看zf_device_bluetooth_ch9141.h文件中的BLUETOOTH_CH9141_TX_PIN宏定义
*                 TX                 查看zf_device_bluetooth_ch9141.h文件中的BLUETOOTH_CH9141_RX_PIN宏定义
*                 RTS                查看zf_device_bluetooth_ch9141.h文件中的BLUETOOTH_CH9141_RTS_PIN宏定义
*                 CTS                悬空
*                 CMD                悬空或者上拉
*                 ------------------------------------
*********************************************************************************************************************/

#include "zf_common_debug.h"
#include "zf_common_fifo.h"
#include "zf_driver_gpio.h"
#include "zf_driver_uart.h"
#include "zf_driver_delay.h"
#include "zf_device_bluetooth_ch9141.h"

static  fifo_struct     bluetooth_ch9141_fifo;
static  uint8           bluetooth_ch9141_buffer[BLUETOOTH_CH9141_BUFFER_SIZE];  // 数据存放数组

static  uint8           bluetooth_ch9141_data;

//-------------------------------------------------------------------------------------------------------------------
//  @brief      蓝牙转串口模块 串口中断回调函数
//  @param      void
//  @return     void
//  Sample usage:
//  @note       该函数在ISR文件 串口中断程序被调用
//-------------------------------------------------------------------------------------------------------------------
void bluetooth_ch9141_uart_callback (void)
{
    uart_query_byte(BLUETOOTH_CH9141_INDEX, &bluetooth_ch9141_data);            // 读取串口数据
    fifo_write_buffer(&bluetooth_ch9141_fifo, &bluetooth_ch9141_data, 1);       // 存入 FIFO
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       蓝牙转串口模块 发送数据
// @param       data            8bit 数据
// @return      uint32          剩余发送长度
// Sample usage:
//-------------------------------------------------------------------------------------------------------------------
uint32 bluetooth_ch9141_send_byte (const uint8 data)
{
    uint16 time_count = 0;
    while(gpio_get_level(BLUETOOTH_CH9141_RTS_PIN))                             // 如果RTS为低电平，则继续发送数据
    {
        if(BLUETOOTH_CH9141_TIMEOUT_COUNT < time_count ++)
            return 1;                                                           // 模块忙,如果允许当前程序使用while等待 则可以使用后面注释的while等待语句替换本if语句
        system_delay_ms(1);
    }
    uart_write_byte(BLUETOOTH_CH9141_INDEX, data);                              // 发送最后的数据

    return 0;
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      蓝牙转串口模块 发送函数
//  @param      buff            需要发送的数据地址
//  @param      len             发送长度
//  @return     uint32          剩余未发送的字节数
//  Sample usage:
//-------------------------------------------------------------------------------------------------------------------
uint32 bluetooth_ch9141_send_buff (const uint8 *buff, uint32 len)
{
    uint16 time_count = 0;
    while(30 < len)
    {
        time_count = 0;
        while(gpio_get_level(BLUETOOTH_CH9141_RTS_PIN) && BLUETOOTH_CH9141_TIMEOUT_COUNT > time_count ++) // 如果RTS为低电平，则继续发送数据
            system_delay_ms(1);
        if(BLUETOOTH_CH9141_TIMEOUT_COUNT <= time_count)
            return len;                                                         // 模块忙,如果允许当前程序使用while等待 则可以使用后面注释的while等待语句替换本if语句
        uart_write_buffer(BLUETOOTH_CH9141_INDEX, buff, 30);

        buff += 30;                                                             // 地址偏移
        len -= 30;                                                              // 数量
    }

    time_count = 0;
    while(gpio_get_level(BLUETOOTH_CH9141_RTS_PIN) && BLUETOOTH_CH9141_TIMEOUT_COUNT > time_count ++)     // 如果RTS为低电平，则继续发送数据
        system_delay_ms(1);
    if(time_count >= BLUETOOTH_CH9141_TIMEOUT_COUNT)
        return len;                                                             // 模块忙,如果允许当前程序使用while等待 则可以使用后面注释的while等待语句替换本if语句
    uart_write_buffer(BLUETOOTH_CH9141_INDEX, buff, len);                       // 发送最后的数据

    return 0;
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       蓝牙转串口模块 发送字符串
// @param       *str            要发送的字符串地址
// @return      uint32          剩余发送长度
// Sample usage:                bluetooth_ch9141_send_string("Believe in yourself.");
//-------------------------------------------------------------------------------------------------------------------
uint32 bluetooth_ch9141_send_string (const char *str)
{
    uint16 time_count = 0;
    uint32 len = strlen(str);
    while(30 < len)
    {
        time_count = 0;
        while(gpio_get_level(BLUETOOTH_CH9141_RTS_PIN))                         // 如果RTS为低电平，则继续发送数据
        {
            if(BLUETOOTH_CH9141_TIMEOUT_COUNT > time_count ++)
                return len;                                                     // 模块忙,如果允许当前程序使用while等待 则可以使用后面注释的while等待语句替换本if语句
            system_delay_ms(1);
        }
        uart_write_buffer(BLUETOOTH_CH9141_INDEX, (const uint8 *)str, 30);

        str += 30;                                                              // 地址偏移
        len -= 30;                                                              // 数量
    }

    time_count = 0;
    while(gpio_get_level(BLUETOOTH_CH9141_RTS_PIN))                             // 如果RTS为低电平，则继续发送数据
    {
        if(BLUETOOTH_CH9141_TIMEOUT_COUNT > time_count ++)
            return len;                                                         // 模块忙,如果允许当前程序使用while等待 则可以使用后面注释的while等待语句替换本if语句
        system_delay_ms(1);
    }
    uart_write_buffer(BLUETOOTH_CH9141_INDEX, (const uint8 *)str, len);         // 发送最后的数据

    return 0;
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       蓝牙转串口模块 发送摄像头图像至上位机查看图像
// @param       *image_addr     需要发送的图像地址
// @param       image_size      图像的大小
// @return      void
// Sample usage:                bluetooth_ch9141_send_image(&mt9v03x_image[0][0], MT9V03X_IMAGE_SIZE);
//-------------------------------------------------------------------------------------------------------------------
void bluetooth_ch9141_send_image (const uint8 *image_addr, uint32 image_size)
{
    extern uint8 camera_send_image_frame_header[4];
    bluetooth_ch9141_send_buff(camera_send_image_frame_header, 4);
    bluetooth_ch9141_send_buff((uint8 *)image_addr, image_size);
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       蓝牙转串口模块 读取函数
// @param       buff            存储的数据地址
// @param       len             长度
// @return      uint32          实际读取字节数
// Sample usage:
//-------------------------------------------------------------------------------------------------------------------
uint32 bluetooth_ch9141_read_buff (uint8 *buff, uint32 len)
{
    uint32 data_len = len;
    fifo_read_buffer(&bluetooth_ch9141_fifo, buff, &data_len, FIFO_READ_AND_CLEAN);
    return data_len;
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       蓝牙转串口模块 初始化
// @param       mode            蓝牙模式 MASTER_MODE(主机)或者SLAVE_MODE(从机)
// @return      uint8           初始化状态 0-成功 1-失败
// Sample usage:
//-------------------------------------------------------------------------------------------------------------------
uint8 bluetooth_ch9141_init (void)
{
    set_wireless_type(BLUETOOTH_CH9141, bluetooth_ch9141_uart_callback);

    fifo_init(&bluetooth_ch9141_fifo, FIFO_DATA_8BIT, bluetooth_ch9141_buffer, BLUETOOTH_CH9141_BUFFER_SIZE);
    // 本函数使用的波特率为115200 为蓝牙转串口模块的默认波特率 如需其他波特率请使用上位机修改模块参数
    gpio_init(BLUETOOTH_CH9141_RTS_PIN, GPI, 1, GPI_PULL_UP);                   // 初始化流控引脚
    uart_init(BLUETOOTH_CH9141_INDEX, BLUETOOTH_CH9141_BUAD_RATE, BLUETOOTH_CH9141_RX_PIN, BLUETOOTH_CH9141_TX_PIN);
    uart_rx_interrupt(BLUETOOTH_CH9141_INDEX, 1);

    return 0;
}
