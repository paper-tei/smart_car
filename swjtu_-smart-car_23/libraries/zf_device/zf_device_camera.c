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
* 文件名称          zf_device_camera
* 公司名称          成都逐飞科技有限公司
* 版本信息          查看 libraries/doc 文件夹内 version 文件 版本说明
* 开发环境          ADS v1.8.0
* 适用平台          TC264D
* 店铺链接          https://seekfree.taobao.com/
*
* 修改记录
* 日期              作者                备注
* 2022-09-15       pudding             first version
********************************************************************************************************************/

#include "zf_common_debug.h"
#include "zf_common_interrupt.h"
#include "zf_driver_gpio.h"
#include "zf_driver_dma.h"
#include "zf_driver_exti.h"
#include "zf_device_mt9v03x.h"
#include "zf_device_ov7725.h"
#include "zf_device_scc8660.h"
#include "zf_device_camera.h"


fifo_struct camera_receiver_fifo;
uint8 camera_receiver_buffer[CAMERA_RECEIVER_BUFFER_SIZE];
uint8 camera_send_image_frame_header[4] = {0x00, 0xFF, 0x01, 0x01};

//-------------------------------------------------------------------------------------------------------------------
// @brief       摄像头二进制图像数据解压为十六进制八位数据 小钻风用
// @param       *data1          摄像头图像数组
// @param       *data2          存放解压数据的地址
// @param       image_size      图像的大小
// @return      void
// Sample usage:                camera_binary_image_decompression(&ov7725_image_binary[0][0], &data_buffer[0][0], OV7725_SIZE);
//-------------------------------------------------------------------------------------------------------------------
void camera_binary_image_decompression (const uint8 *data1, uint8 *data2, uint32 image_size)
{
    uint8  i = 8;
    zf_assert(data1 != NULL);
    zf_assert(data2 != NULL);
    while(image_size --)
    {
        i = 8;
        while(i --)
        {
            *data2 ++ = (((*data1 >> i) & 0x01) ? 255 : 0);
        }
        data1 ++;
    }
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       摄像头图像发送至上位机查看图像
// @param       uartn           使用的串口号
// @param       *image_addr     需要发送的图像地址
// @param       image_size      图像的大小
// @return      void
// Sample usage:                camera_send_image(DEBUG_UART_INDEX, &mt9v03x_image[0][0], MT9V03X_IMAGE_SIZE);
//-------------------------------------------------------------------------------------------------------------------
void camera_send_image (uart_index_enum uartn, const uint8 *image_addr, uint32 image_size)
{
    zf_assert(image_addr != NULL);
    // 发送命令
    uart_write_buffer(uartn, camera_send_image_frame_header, 4);

    // 发送图像
    uart_write_buffer(uartn, (uint8 *)image_addr, image_size);
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     摄像头串口 FIFO 初始化
// 参数说明     void
// 返回参数     void
// 使用示例     camera_fifo_init();
// 备注信息
//-------------------------------------------------------------------------------------------------------------------
void camera_fifo_init (void)
{
    fifo_init(&camera_receiver_fifo, FIFO_DATA_8BIT, camera_receiver_buffer, CAMERA_RECEIVER_BUFFER_SIZE);
}


//-------------------------------------------------------------------------------------------------------------------
// @brief       摄像头初始化
// @param       image_size      图像的大小
// @return      void
// @param       image_size      图像的大小
// @param       data_addr       数据来源外设地址
// @param       buffer_addr     图像缓冲区地址
// @return      void
// Sample usage:                camera_init();
//-------------------------------------------------------------------------------------------------------------------
uint8 camera_init (uint8 *source_addr, uint8 *destination_addr, uint16 image_size)
{
    uint8 num;
    uint8 link_list_num;
    switch(camera_type)
    {
        case CAMERA_BIN_IIC:                                                    // IIC 小钻风
        case CAMERA_BIN_UART:                                                   // UART 小钻风
            for(num = 0; num < 8; num ++)
            {
                gpio_init((gpio_pin_enum)(OV7725_DATA_PIN + num), GPI, GPIO_LOW, GPI_FLOATING_IN);
            }
            link_list_num = dma_init(OV7725_DMA_CH,
                                     source_addr,
                                     destination_addr,
                                     OV7725_PCLK_PIN,
                                     EXTI_TRIGGER_FALLING,
                                     image_size);
            exti_init(OV7725_VSYNC_PIN, EXTI_TRIGGER_FALLING);                  //初始化场中断，并设置为下降沿触发中断
            break;
        case CAMERA_GRAYSCALE:                                                  // 总钻风
            for(num = 0; num < 8; num ++)
            {
                gpio_init((gpio_pin_enum)(MT9V03X_DATA_PIN + num), GPI, GPIO_LOW, GPI_FLOATING_IN);
            }
            link_list_num = dma_init(MT9V03X_DMA_CH,
                                     source_addr,
                                     destination_addr,
                                     MT9V03X_PCLK_PIN,
                                     EXTI_TRIGGER_RISING,
                                     image_size);                               // 如果超频到300M 倒数第二个参数请设置为FALLING

            exti_init(MT9V03X_VSYNC_PIN, EXTI_TRIGGER_FALLING);                 // 初始化场中断，并设置为下降沿触发中断
            break;
        case CAMERA_COLOR:                                                      // 凌瞳
            for(num=0; num<8; num++)
            {
                gpio_init((gpio_pin_enum)(SCC8660_DATA_PIN + num), GPI, GPIO_LOW, GPI_FLOATING_IN);
            }

            link_list_num = dma_init(SCC8660_DMA_CH,
                                     source_addr,
                                     destination_addr,
                                     SCC8660_PCLK_PIN,
                                     EXTI_TRIGGER_RISING,
                                     image_size);                               // 如果超频到300M 倒数第二个参数请设置为FALLING

            exti_init(SCC8660_VSYNC_PIN, EXTI_TRIGGER_FALLING);                  // 初始化场中断，并设置为下降沿触发中断
            break;
        default:
            break;
    }
    return link_list_num;
}
