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
* 文件名称          zf_device_scc8660
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
*                  ------------------------------------
*                  模块管脚             单片机管脚
*                  TXD                查看 zf_device_scc8660.h 中 SCC8660_COF_UART_TX 宏定义
*                  RXD                查看 zf_device_scc8660.h 中 SCC8660_COF_UART_RX 宏定义
*                  PCLK               查看 zf_device_scc8660.h 中 SCC8660_PCLK_PIN 宏定义
*                  VSY                查看 zf_device_scc8660.h 中 SCC8660_VSYNC_PIN 宏定义
*                  D0-D7              查看 zf_device_scc8660.h 中 SCC8660_DATA_PIN 宏定义 从该定义开始的连续八个引脚
*                  VCC                3.3V电源
*                  GND                电源地
*                  其余引脚悬空
*                  ------------------------------------
********************************************************************************************************************/

#include "zf_common_interrupt.h"
#include "zf_common_debug.h"
#include "zf_common_fifo.h"
#include "zf_driver_delay.h"
#include "zf_driver_dma.h"
#include "zf_driver_exti.h"
#include "zf_driver_gpio.h"
#include "zf_driver_uart.h"
#include "zf_device_camera.h"
#include "zf_device_scc8660.h"
#include "zf_device_type.h"

vuint8 scc8660_finish_flag = 0;                                                 // 一场图像采集完成标志位
IFX_ALIGN(4) uint16 scc8660_image[SCC8660_H][SCC8660_W];

uint8   scc8660_link_list_num;

uint8   scc8660_lost_flag = 1;                                                  // 图像丢失标志位
uint8   scc8660_dma_int_num;                                                    // 当前DMA中断次数
uint8   scc8660_dma_init_flag;                                                  // 是否需要重新初始化


// 需要配置到摄像头的数据 不允许在这修改参数
static int16 scc8660_set_confing_buffer[SCC8660_CONFIG_FINISH][2]=
{
    {SCC8660_INIT,              0},                                             // 摄像头开始初始化

    {SCC8660_AUTO_EXP,          SCC8660_AUTO_EXP_DEF},                          // 自动曝光
    {SCC8660_BRIGHT,            SCC8660_BRIGHT_DEF},                            // 亮度设置
    {SCC8660_FPS,               SCC8660_FPS_DEF},                               // 图像帧率
    {SCC8660_SET_COL,           SCC8660_W},                                     // 图像列数
    {SCC8660_SET_ROW,           SCC8660_H},                                     // 图像行数
    {SCC8660_PCLK_DIV,          SCC8660_PCLK_DIV_DEF},                          // PCLK分频系数
    {SCC8660_PCLK_MODE,         SCC8660_PCLK_MODE_DEF},                         // PCLK模式
    {SCC8660_COLOR_MODE,        SCC8660_COLOR_MODE_DEF},                        // 图像色彩模式
    {SCC8660_DATA_FORMAT,       SCC8660_DATA_FORMAT_DEF},                       // 输出数据格式
    {SCC8660_MANUAL_WB,         SCC8660_MANUAL_WB_DEF}                          // 手动白平衡
};

// 从摄像头内部获取到的配置数据 不允许在这修改参数
static int16 scc8660_get_confing_buffer[SCC8660_CONFIG_FINISH - 1][2]=
{
    {SCC8660_AUTO_EXP,          0},
    {SCC8660_BRIGHT,            0},                                             // 亮度设置
    {SCC8660_FPS,               0},                                             // 图像帧率
    {SCC8660_SET_COL,           0},                                             // 图像列数
    {SCC8660_SET_ROW,           0},                                             // 图像行数
    {SCC8660_PCLK_DIV,          0},                                             // PCLK分频系数
    {SCC8660_PCLK_MODE,         0},                                             // PCLK模式
    {SCC8660_COLOR_MODE,        0},                                             // 图像色彩模式
    {SCC8660_DATA_FORMAT,       0},                                             // 输出数据格式
    {SCC8660_MANUAL_WB,         0},                                             // 白平衡设置
};

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     配置摄像头内部配置信息 内部调用
// 参数说明     buff            发送配置信息的地址
// 返回参数     uint8           1-失败 0-成功
// 使用示例     if(scc8660_set_config(scc8660_set_confing_buffer)){}
// 备注信息     调用该函数前请先初始化串口
//-------------------------------------------------------------------------------------------------------------------
static uint8 scc8660_set_config (int16 buff[SCC8660_CONFIG_FINISH][2])
{
    uint8 return_state = 1;
    uint8  uart_buffer[4];
    uint16 temp;
    uint16 timeout_count = 0;
    uint32 loop_count = 0;
    uint32 uart_buffer_index = 0;

    // 设置参数  具体请参看问题锦集手册
    // 开始配置摄像头并重新初始化
    for(loop_count = SCC8660_MANUAL_WB; loop_count < SCC8660_SET_REG_DATA; loop_count --)
    {
        uart_buffer[0] = 0xA5;
        uart_buffer[1] = (uint8)buff[loop_count][0];
        temp = buff[loop_count][1];
        uart_buffer[2] = temp >> 8;
        uart_buffer[3] = (uint8)temp;
        uart_write_buffer(SCC8660_COF_UART, uart_buffer, 4);

        system_delay_ms(2);
    }

    do
    {
        if(3 <= fifo_used(&camera_receiver_fifo))
        {
            uart_buffer_index = 3;
            fifo_read_buffer(&camera_receiver_fifo, uart_buffer, &uart_buffer_index, FIFO_READ_AND_CLEAN);
            if((0xff == uart_buffer[1]) || (0xff == uart_buffer[2]))
            {
                return_state = 0;
                break;
            }
        }
        system_delay_ms(1);
    }while(SCC8660_INIT_TIMEOUT > timeout_count ++);

    // 以上部分对摄像头配置的数据全部都会保存在摄像头上51单片机的eeprom中
    // 利用set_exposure_time函数单独配置的曝光数据不存储在eeprom中
    return return_state;
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     获取摄像头内部配置信息 内部调用
// 参数说明     buff            接收配置信息的地址
// 返回参数     uint8           1-失败 0-成功
// 使用示例     if(scc8660_get_config(scc8660_get_confing_buffer)){}
// 备注信息     调用该函数前请先初始化串口
//-------------------------------------------------------------------------------------------------------------------
static uint8 scc8660_get_config (int16 buff[SCC8660_CONFIG_FINISH-1][2])
{
    uint8 return_state = 0;
    uint8  uart_buffer[4];
    uint16 temp;
    uint16 timeout_count = 0;
    uint32 loop_count = 0;
    uint32 uart_buffer_index = 0;

    for(loop_count = SCC8660_MANUAL_WB - 1; loop_count >= 1; loop_count --)
    {
        uart_buffer[0] = 0xA5;
        uart_buffer[1] = SCC8660_GET_STATUS;
        temp = buff[loop_count][0];
        uart_buffer[2] = temp >> 8;
        uart_buffer[3] = (uint8)temp;
        uart_write_buffer(SCC8660_COF_UART, uart_buffer, 4);

        timeout_count = 0;
        do
        {
            if(3 <= fifo_used(&camera_receiver_fifo))
            {
                uart_buffer_index = 3;
                fifo_read_buffer(&camera_receiver_fifo, uart_buffer, &uart_buffer_index, FIFO_READ_AND_CLEAN);
                buff[loop_count][1] = uart_buffer[1] << 8 | uart_buffer[2];
                break;
            }
            system_delay_ms(1);
        }while(SCC8660_INIT_TIMEOUT > timeout_count ++);
        if(timeout_count > SCC8660_INIT_TIMEOUT)                                // 超时
        {
            return_state = 1;
            break;
        }
    }
    return return_state;
}

//-------------------------------------------------------------------------------------------------------------------
//  函数简介      SCC8660摄像头串口通信回调
//  参数说明      void
//  返回参数      void
//  使用示例      scc8660_uart_callback();
//-------------------------------------------------------------------------------------------------------------------
static void scc8660_uart_callback (void)
{
    uint8 data = 0;
    uart_query_byte(SCC8660_COF_UART, &data);
    if(0xA5 == data)
    {
        fifo_clear(&camera_receiver_fifo);
    }
    fifo_write_element(&camera_receiver_fifo, data);
}

//-------------------------------------------------------------------------------------------------------------------
//  函数简介      SCC8660摄像头场中断
//  参数说明      void
//  返回参数      void
//  使用示例      scc8660_vsync_handler();
//-------------------------------------------------------------------------------------------------------------------
static void scc8660_vsync_handler(void)
{
    exti_flag_clear(SCC8660_VSYNC_PIN);
    scc8660_dma_int_num = 0;
    if(scc8660_dma_init_flag || scc8660_lost_flag)
    {
        scc8660_dma_init_flag = 0;
        IfxDma_resetChannel(&MODULE_DMA, SCC8660_DMA_CH);
        scc8660_link_list_num = dma_init(SCC8660_DMA_CH,
                                         SCC8660_DATA_ADD,
                                         (uint8 *)scc8660_image[0],
                                         SCC8660_PCLK_PIN,
                                         EXTI_TRIGGER_RISING,
                                         SCC8660_IMAGE_SIZE);               // 如果超频到300M 倒数第二个参数请设置为FALLING
        dma_enable(SCC8660_DMA_CH);
    }
    else
    {
        if(1 == scc8660_link_list_num)
        {
            dma_set_destination(SCC8660_DMA_CH, (uint8 *)scc8660_image[0]); // 没有采用链接传输模式 重新设置目的地址
        }
        dma_enable(SCC8660_DMA_CH);
    }
    scc8660_lost_flag = 1;
}

//-------------------------------------------------------------------------------------------------------------------
//  函数简介      SCC8660摄像头DMA完成中断
//  参数说明      void
//  返回参数      void
//  使用示例      scc8660_dma_handler();
//-------------------------------------------------------------------------------------------------------------------
static void scc8660_dma_handler(void)
{
    clear_dma_flag(SCC8660_DMA_CH);

    if(IfxDma_getChannelTransactionRequestLost(&MODULE_DMA, SCC8660_DMA_CH)) // 图像错位判断
    {
        scc8660_finish_flag = 0;
        dma_disable(SCC8660_DMA_CH);
        IfxDma_clearChannelTransactionRequestLost(&MODULE_DMA, SCC8660_DMA_CH);
        scc8660_dma_init_flag = 1;
    }
    else
    {
        scc8660_dma_int_num++;
        if(scc8660_dma_int_num >= scc8660_link_list_num)
        {
            // 采集完成
            // 一副图像从采集开始到采集结束耗时3.8MS左右(50FPS、188*120分辨率)
            scc8660_dma_int_num = 0;
            scc8660_lost_flag   = 0;
            scc8660_finish_flag = 1;
            dma_disable(SCC8660_DMA_CH);
        }
    }
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     获取摄像头 ID
// 参数说明     void
// 返回参数     uint16          0-获取错误 N-版本号
// 使用示例     scc8660_get_id();
// 备注信息     调用该函数前请先初始化串口
//-------------------------------------------------------------------------------------------------------------------
uint16 scc8660_get_id (void)
{
    uint16 temp;
    uint8  uart_buffer[4];
    uint16 timeout_count = 0;
    uint16 return_value = 0;
    uint32 uart_buffer_index = 0;

    uart_buffer[0] = 0xA5;
    uart_buffer[1] = SCC8660_GET_WHO_AM_I;
    temp = 0;
    uart_buffer[2] = temp >> 8;
    uart_buffer[3] = (uint8)temp;
    uart_write_buffer(SCC8660_COF_UART, uart_buffer, 4);

    do
    {
        if(3 <= fifo_used(&camera_receiver_fifo))
        {
            uart_buffer_index = 3;
            fifo_read_buffer(&camera_receiver_fifo, uart_buffer, &uart_buffer_index, FIFO_READ_AND_CLEAN);
            return_value = uart_buffer[1] << 8 | uart_buffer[2];
            break;
        }
        system_delay_ms(1);
    }while(SCC8660_INIT_TIMEOUT > timeout_count ++);
    return return_value;
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     单独设置摄像头曝光时间
// 参数说明     light           设置曝光时间越大图像越亮，摄像头收到后会根据分辨率及FPS计算最大曝光时间如果设置的数据过大，那么摄像头将会设置这个最大值
// 返回参数     uint16          数据
// 使用示例     scc8660_get_parameter();
// 备注信息     调用该函数前请先初始化串口
//-------------------------------------------------------------------------------------------------------------------
uint16 scc8660_get_parameter (uint16 config)
{
    uint8  uart_buffer[4];
    uint16 timeout_count = 0;
    uint16 return_value = 0;
    uint32 uart_buffer_index = 0;

    uart_buffer[0] = 0xA5;
    uart_buffer[1] = SCC8660_GET_WHO_AM_I;
    uart_buffer[2] = 0x00;
    uart_buffer[3] = (uint8)config;
    uart_write_buffer(SCC8660_COF_UART, uart_buffer, 4);

    do
    {
        if(3 <= fifo_used(&camera_receiver_fifo))
        {
            uart_buffer_index = 3;
            fifo_read_buffer(&camera_receiver_fifo, uart_buffer, &uart_buffer_index, FIFO_READ_AND_CLEAN);
            return_value = uart_buffer[1] << 8 | uart_buffer[2];
            break;
        }
        system_delay_ms(1);
    }while(SCC8660_INIT_TIMEOUT > timeout_count ++);
    return return_value;
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     获取彩色摄像头固件版本
// 参数说明     void
// 返回参数     uint16          版本号
// 使用示例     scc8660_get_version();
// 备注信息     调用该函数前请先初始化摄像头配置串口
//-------------------------------------------------------------------------------------------------------------------
uint16 scc8660_get_version (void)
{
    uint16 temp;
    uint8  uart_buffer[4];
    uint16 timeout_count = 0;
    uint16 return_value = 0;
    uint32 uart_buffer_index = 0;

    uart_buffer[0] = 0xA5;
    uart_buffer[1] = SCC8660_GET_STATUS;
    temp           = SCC8660_GET_VERSION;
    uart_buffer[2] = temp >> 8;
    uart_buffer[3] = (uint8)temp;

    uart_write_buffer(SCC8660_COF_UART, uart_buffer, 4);

    do
    {
        if(3 <= fifo_used(&camera_receiver_fifo))
        {
            uart_buffer_index = 3;
            fifo_read_buffer(&camera_receiver_fifo, uart_buffer, &uart_buffer_index, FIFO_READ_AND_CLEAN);
            return_value = uart_buffer[1] << 8 | uart_buffer[2];
            break;
        }
        system_delay_ms(1);
    }while(SCC8660_INIT_TIMEOUT > timeout_count ++);
    return return_value;
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     单独设置图像亮度
// 参数说明     data            需要设置的亮度值
// 返回参数     uint8           1-失败 0-成功
// 使用示例     scc8660_set_bright(data);
// 备注信息     调用该函数前请先初始化摄像头配置串口   通过该函数设置的参数，不会被51单片机保存
//-------------------------------------------------------------------------------------------------------------------
uint8 scc8660_set_bright (uint16 data)
{
    uint8 return_state = 0;
    uint8  uart_buffer[4];
    uint16 temp;
    uint16 timeout_count = 0;
    uint32 uart_buffer_index = 0;

    uart_buffer[0] = 0xA5;
    uart_buffer[1] = SCC8660_SET_BRIGHT;
    uart_buffer[2] = data >> 8;
    uart_buffer[3] = (uint8)data;

    uart_write_buffer(SCC8660_COF_UART, uart_buffer, 4);

    do
    {
        if(3 <= fifo_used(&camera_receiver_fifo))
        {
            uart_buffer_index = 3;
            fifo_read_buffer(&camera_receiver_fifo, uart_buffer, &uart_buffer_index, FIFO_READ_AND_CLEAN);
            temp = uart_buffer[1] << 8 | uart_buffer[2];
            break;
        }
        system_delay_ms(1);
    }while(SCC8660_INIT_TIMEOUT > timeout_count ++);
    if((temp != data) || (SCC8660_INIT_TIMEOUT <= timeout_count))
    {
        return_state = 1;
    }
    return return_state;
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     单独设置白平衡
// 参数说明     data            需要设置的亮度值
// 返回参数     uint8           1-失败 0-成功
// 使用示例     scc8660_set_white_balance(data);
// 备注信息     通过该函数设置的参数，不会被51单片机保存 调用该函数前请先初始化摄像头配置串口
//-------------------------------------------------------------------------------------------------------------------
uint8 scc8660_set_white_balance (uint16 data)
{
    uint8 return_state = 0;
    uint8  uart_buffer[4];
    uint16 temp;
    uint16 timeout_count = 0;
    uint32 uart_buffer_index = 0;

    uart_buffer[0] = 0xA5;
    uart_buffer[1] = SCC8660_SET_MANUAL_WB;
    uart_buffer[2] = data >> 8;
    uart_buffer[3] = (uint8)data;

    uart_write_buffer(SCC8660_COF_UART, uart_buffer, 4);

    do
    {
        if(3 <= fifo_used(&camera_receiver_fifo))
        {
            uart_buffer_index = 3;
            fifo_read_buffer(&camera_receiver_fifo, uart_buffer, &uart_buffer_index, FIFO_READ_AND_CLEAN);
            temp = uart_buffer[1] << 8 | uart_buffer[2];
            break;
        }
        system_delay_ms(1);
    }while(SCC8660_INIT_TIMEOUT > timeout_count ++);
    if((temp != data) || (SCC8660_INIT_TIMEOUT <= timeout_count))
    {
        return_state = 1;
    }
    return return_state;
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     对摄像头内部寄存器进行写操作
// 参数说明     addr            摄像头内部寄存器地址
// 参数说明     data            需要写入的数据
// 返回参数     uint8           1-失败 0-成功
// 使用示例     scc8660_set_reg(addr, data);
// 备注信息     调用该函数前请先初始化串口
//-------------------------------------------------------------------------------------------------------------------
uint8 scc8660_set_reg (uint8 addr, uint16 data)
{
    uint8 return_state = 0;
    uint8  uart_buffer[4];
    uint16 temp;
    uint16 timeout_count = 0;
    uint32 uart_buffer_index = 0;

    uart_buffer[0] = 0xA5;
    uart_buffer[1] = SCC8660_SET_REG_ADDR;
    uart_buffer[2] = 0x00;
    uart_buffer[3] = (uint8)addr;
    uart_write_buffer(SCC8660_COF_UART, uart_buffer, 4);

    system_delay_ms(10);
    uart_buffer[0] = 0xA5;
    uart_buffer[1] = SCC8660_SET_REG_DATA;
    temp           = data;
    uart_buffer[2] = temp >> 8;
    uart_buffer[3] = (uint8)temp;
    uart_write_buffer(SCC8660_COF_UART, uart_buffer, 4);

    do
    {
        if(3 <= fifo_used(&camera_receiver_fifo))
        {
            uart_buffer_index = 3;
            fifo_read_buffer(&camera_receiver_fifo, uart_buffer, &uart_buffer_index, FIFO_READ_AND_CLEAN);
            temp = uart_buffer[1] << 8 | uart_buffer[2];
            break;
        }
        system_delay_ms(1);
    }while(SCC8660_INIT_TIMEOUT > timeout_count ++);
    if((temp != data) || (SCC8660_INIT_TIMEOUT <= timeout_count))
    {
        return_state = 1;
    }
    return return_state;
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     SCC8660 摄像头初始化
// 参数说明     void
// 返回参数     uint8           1-失败 0-成功
// 使用示例     scc8660_init();
// 备注信息
//-------------------------------------------------------------------------------------------------------------------
uint8 scc8660_init (void)
{
    uint8 return_state = 0;
    uint16 scc8660_version = 0;

    gpio_init(P02_0, GPO, GPIO_HIGH, GPO_PUSH_PULL);
    gpio_init(P02_1, GPO, GPIO_HIGH, GPO_PUSH_PULL);

    // 初始换串口 配置摄像头
    uart_init(SCC8660_COF_UART, SCC8660_COF_BAUR, SCC8660_COF_UART_RX, SCC8660_COF_UART_TX);
    uart_rx_interrupt(SCC8660_COF_UART, 1);

    system_delay_ms(200);

    set_camera_type(CAMERA_COLOR, scc8660_vsync_handler, scc8660_dma_handler, scc8660_uart_callback);                                              // 设置连接摄像头类型
    camera_fifo_init();
    do
    {
        // 等待摄像头上电初始化成功 方式有两种：延时或者通过获取配置的方式 二选一
        // system_delay_ms(1000);                                                   // 延时方式
        scc8660_version = scc8660_get_version();                                    // 获取配置的方式
        if(scc8660_set_config(scc8660_set_confing_buffer))
        {
            set_camera_type(NO_CAMERE, NULL, NULL, NULL);
            return_state = 1;
            // 如果程序在输出了断言信息 并且提示出错位置在这里
            // 那么就是串口通信出错并超时退出了
            // 检查一下接线有没有问题 如果没问题可能就是坏了
            zf_log(0, "SCC8660 set config error.");
            break;
        }

        if(0 == return_state)
        {
            // 获取配置便于查看配置是否正确
            if(scc8660_get_config(scc8660_get_confing_buffer))
            {
                set_camera_type(NO_CAMERE, NULL, NULL, NULL);
                return_state = 1;
                // 如果程序在输出了断言信息 并且提示出错位置在这里
                // 那么就是串口通信出错并超时退出了
                // 检查一下接线有没有问题 如果没问题可能就是坏了
                zf_log(0, "SCC8660 set config error.");
                break;
            }

            scc8660_link_list_num = camera_init(SCC8660_DATA_ADD, (uint8 *)scc8660_image[0], SCC8660_IMAGE_SIZE);
        }
    }while(0);

    return return_state;
}

