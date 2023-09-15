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
* 文件名称          zf_device_ips114
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
/********************************************************************************************************************
* 接线定义：
*                  ------------------------------------
*                  模块管脚              单片机管脚
*                  SCL                 查看 zf_device_ips114.h 中 IPS114_SCL_PIN 宏定义
*                  SDA                 查看 zf_device_ips114.h 中 IPS114_SDA_PIN 宏定义
*                  RST                 查看 zf_device_ips114.h 中 IPS114_RST_PIN 宏定义
*                  DC                  查看 zf_device_ips114.h 中 IPS114_DC_PIN 宏定义
*                  CS                  查看 zf_device_ips114.h 中 IPS114_CS_PIN 宏定义
*                  BLK                 查看 zf_device_ips114.h 中 IPS114_BLK_PIN 宏定义
*                  VCC                 3 .3V电源
*                  GND                 电源地
*                  最大分辨率 135 * 240
*                  ------------------------------------
********************************************************************************************************************/

#ifndef _zf_device_ips114_h_
#define _zf_device_ips114_h_

#include "zf_common_typedef.h"

#define IPS114_USE_SOFT_SPI             (0)                                     // 默认使用硬件 SPI 方式驱动 建议使用硬件 SPI 方式驱动
#if IPS114_USE_SOFT_SPI                                                         // 这两段 颜色正常的才是正确的 颜色灰的就是没有用的
//====================================================软件 SPI 驱动==================================================
#define IPS114_SOFT_SPI_DELAY           (0 )                                    // 软件 SPI 的时钟延时周期 数值越小 SPI 通信速率越快
#define IPS114_SCL_PIN                  (P15_3)                                 // 软件 SPI SCK 引脚
#define IPS114_SDA_PIN                  (P15_5)                                 // 软件 SPI MOSI 引脚
//====================================================软件 SPI 驱动==================================================
#else
//====================================================硬件 SPI 驱动==================================================
#define IPS114_SPI_SPEED                (60*1000*1000)                          // 硬件 SPI 速率
#define IPS114_SPI                      (SPI_2)                                 // 硬件 SPI 号
#define IPS114_SCL_PIN                  (SPI2_SCLK_P15_3)                       // 硬件 SPI SCK 引脚
#define IPS114_SDA_PIN                  (SPI2_MOSI_P15_5)                       // 硬件 SPI MOSI 引脚
#define IPS114_SDA_IN_PIN               (SPI2_MISO_P15_4)                       // 定义SPI_MISO引脚  IPS没有MISO引脚，但是这里任然需要定义，在spi的初始化时需要使用
//====================================================硬件 SPI 驱动==================================================
#endif

#define IPS114_RST_PIN                  (P15_1)                                  // 液晶复位引脚定义
#define IPS114_DC_PIN                   (P15_0 )                                 // 液晶命令位引脚定义
#define IPS114_CS_PIN                   (P15_2)                                  // CS 片选引脚
#define IPS114_BLK_PIN                  (P15_4)                                  // 液晶背光引脚定义

#define IPS114_DEFAULT_DISPLAY_DIR      (IPS114_CROSSWISE_180)                   // 默认的显示方向
#define IPS114_DEFAULT_PENCOLOR         (RGB565_RED)                             // 默认的画笔颜色
#define IPS114_DEFAULT_BGCOLOR          (RGB565_WHITE)                           // 默认的背景颜色
#define IPS114_DEFAULT_DISPLAY_FONT     (IPS114_8X16_FONT)                       // 默认的字体模式

#define IPS114_DC(x)                    ((x) ? (gpio_high(IPS114_DC_PIN))  : (gpio_low(IPS114_DC_PIN)))
#define IPS114_RST(x)                   ((x) ? (gpio_high(IPS114_RST_PIN)) : (gpio_low(IPS114_RST_PIN)))
#define IPS114_CS(x)                    ((x) ? (gpio_high(IPS114_CS_PIN))  : (gpio_low(IPS114_CS_PIN)))
#define IPS114_BLK(x)                   ((x) ? (gpio_high(IPS114_BLK_PIN)) : (gpio_low(IPS114_BLK_PIN)))

//=================================================定义 IPS114 参数结构体===============================================
typedef enum
{
    IPS114_PORTAIT                      = 0,                                     // 竖屏模式
    IPS114_PORTAIT_180                  = 1,                                     // 竖屏模式  旋转180
    IPS114_CROSSWISE                    = 2,                                     // 横屏模式
    IPS114_CROSSWISE_180                = 3,                                     // 横屏模式  旋转180
}ips114_dir_enum;

typedef enum
{
    IPS114_6X8_FONT                     = 0,                                     // 6x8      字体
    IPS114_8X16_FONT                    = 1,                                     // 8x16     字体
    IPS114_16X16_FONT                   = 2,                                     // 16x16    字体 目前不支持
}ips114_font_size_enum;
//=================================================定义 IPS114 参数结构体===============================================

//===================================================IPS114 基础函数==================================================
void    ips114_clear                    (void);
void    ips114_full                     (const uint16 color);
void    ips114_set_dir                  (ips114_dir_enum dir);
void    ips114_set_font                 (ips114_font_size_enum font);
void    ips114_set_color                (const uint16 pen, const  uint16 bgcolor);
void    ips114_draw_point               (uint16 x, uint16 y, const uint16 color);
void    ips114_draw_line                (uint16 x_start, uint16 y_start, uint16 x_end, uint16 y_end, const uint16 color);

void    ips114_show_char                (uint16 x, uint16 y, const char dat);
void    ips114_show_string              (uint16 x, uint16 y, const char dat[]);
void    ips114_show_int                 (uint16 x,uint16 y, const int32 dat, uint8 num);
void    ips114_show_uint                (uint16 x,uint16 y, const uint32 dat, uint8 num);
void    ips114_show_float               (uint16 x,uint16 y, const float dat, uint8 num, uint8 pointnum);

void    ips114_show_binary_image        (uint16 x, uint16 y, const uint8 *image, uint16 width, uint16 height, uint16 dis_width, uint16 dis_height);
void    ips114_show_gray_image          (uint16 x, uint16 y, const uint8 *image, uint16 width, uint16 height, uint16 dis_width, uint16 dis_height, uint8 threshold);
void    ips114_show_rgb565_image        (uint16 x, uint16 y, const uint16 *image, uint16 width, uint16 height, uint16 dis_width, uint16 dis_height, uint8 color_mode);

void    ips114_show_wave                (uint16 x, uint16 y, const uint16 *wave, uint16 width, uint16 value_max, uint16 dis_width, uint16 dis_value_max);
void    ips114_show_chinese             (uint16 x, uint16 y, uint8 size, const uint8 *chinese_buffer, uint8 number, const uint16 color);
void    ips114_init                     (void);
//===================================================IPS114 基础函数==================================================


//===================================================IPS114 扩展函数==================================================
//-------------------------------------------------------------------------------------------------------------------
// 函数简介     IPS114 显示小钻风图像
// 参数说明     p               图像数组指针
// 参数说明     width           图像实际宽度
// 参数说明     height          图像实际高度
// 返回参数     void
// 使用示例     ips114_displayimage7725(ov7725_image_binary[0], OV7725_W, OV7725_H);
// 备注信息     拓展的一键显示函数，默认无缩放，从屏幕坐标起始点开始显示
//-------------------------------------------------------------------------------------------------------------------
#define ips114_displayimage7725(p, width, height)       (ips114_show_binary_image(0, 0, (p), OV7725_W, OV7725_H, (width), (height)))

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     IPS114 显示总钻风图像
// 参数说明     p               图像数组指针
// 参数说明     width           图像实际宽度
// 参数说明     height          图像实际高度
// 返回参数     void
// 使用示例     ips114_displayimage03x(mt9v03x_image[0], MT9V03X_W, MT9V03X_H);
// 备注信息     拓展的一键显示函数，默认无缩放，从屏幕坐标起始点开始显示
//-------------------------------------------------------------------------------------------------------------------
#define ips114_displayimage03x(p, width, height)        (ips114_show_gray_image(0, 0, (p), MT9V03X_W, MT9V03X_H, (width), (height), 0))

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     IPS114 显示凌瞳图像
// 参数说明     p               图像数组指针
// 参数说明     width           图像实际宽度
// 参数说明     height          图像实际高度
// 返回参数     void
// 使用示例     ips114_displayimage8660(scc8660_image[0], SCC8660_W, SCC8660_H);
// 备注信息     拓展的一键显示函数，默认无缩放，从屏幕坐标起始点开始显示
//-------------------------------------------------------------------------------------------------------------------
#define ips114_displayimage8660(p, width, height)       (ips114_show_rgb565_image(0, 0, (p), SCC8660_W, SCC8660_H, (width), (height), 1))

//===================================================IPS114 扩展函数==================================================

#endif
