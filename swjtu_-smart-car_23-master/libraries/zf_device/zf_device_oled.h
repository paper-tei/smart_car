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
* 文件名称          zf_device_oled
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
*                  D0                 查看 zf_device_oled.h 中 OLED_D0_PIN 宏定义
*                  D1                 查看 zf_device_oled.h 中 OLED_D1_PIN 宏定义
*                  RES                查看 zf_device_oled.h 中 OLED_RES_PIN 宏定义
*                  DC                 查看 zf_device_oled.h 中 OLED_DC_PIN 宏定义
*                  CS                 查看 zf_device_oled.h 中 OLED_CS_PIN 宏定义
*                  VCC                3.3V电源
*                  GND                电源地
*                  ------------------------------------
********************************************************************************************************************/

#ifndef _zf_device_oled_h_
#define _zf_device_oled_h_

#include "zf_device_type.h"

#define OLED_USE_SOFT_SPI               (0 )                                    // 默认使用硬件 SPI 方式驱动 建议使用硬件 SPI 方式驱动
#if OLED_USE_SOFT_SPI                                                           // 这两段 颜色正常的才是正确的 颜色灰的就是没有用的
//====================================================软件 SPI 驱动====================================================
#define OLED_SOFT_SPI_DELAY             (0 )                                    // 软件 SPI 的时钟延时周期 数值越小 SPI 通信速率越快
#define OLED_D0_PIN                     (P15_3)                                 // 软件 SPI SCK 引脚
#define OLED_D1_PIN                     (P15_5)                                 // 软件 SPI MOSI 引脚
//====================================================软件 SPI 驱动====================================================
#else
//====================================================硬件 SPI 驱动====================================================
#define OLED_SPI_SPEED                  (30*1000*1000)                          // 硬件 SPI 速率
#define OLED_SPI                        (SPI_2)                                 // 硬件 SPI 号
#define OLED_D0_PIN                     (SPI2_SCLK_P15_3 )                      // 硬件 SPI SCK 引脚
#define OLED_D1_PIN                     (SPI2_MOSI_P15_5)                       // 硬件 SPI MOSI 引脚
#define OLED_D1_PIN_IN                  (SPI2_MISO_P15_4)                       // 硬件 SPI MISO 引脚  OLED没有MISO引脚，但是这里任然需要定义，在spi的初始化时需要使用
//====================================================硬件 SPI 驱动====================================================
#endif


#define OLED_RES_PIN                    (P15_1)                                 // 液晶复位引脚定义
#define OLED_DC_PIN                     (P15_0 )                                // 液晶命令位引脚定义
#define OLED_CS_PIN                     (P15_2 )                                // CS 片选引脚

#define OLED_RES(x)                     ((x) ? (gpio_high(OLED_RES_PIN)) : (gpio_low(OLED_RES_PIN)))
#define OLED_DC(x)                      ((x) ? (gpio_high(OLED_DC_PIN))  : (gpio_low(OLED_DC_PIN)))
#define OLED_CS(x)                      ((x) ? (gpio_high(OLED_CS_PIN))  : (gpio_low(OLED_CS_PIN)))

#define OLED_BRIGHTNESS                 (0x7f)                                  // 设置OLED亮度 越大越亮 范围0-0XFF
#define OLED_DEFAULT_DISPLAY_DIR        (OLED_CROSSWISE)                        // 默认的显示方向
#define OLED_DEFAULT_DISPLAY_FONT       (OLED_6X8_FONT )                        // 默认的字体模式
#define OLED_X_MAX                      (128)
#define OLED_Y_MAX                      (64 )

//=================================================定义 OLED 参数结构体===============================================
typedef enum
{
    OLED_CROSSWISE                      = 0,                                    // 横屏模式
    OLED_CROSSWISE_180                  = 1,                                    // 横屏模式  旋转180
}oled_dir_enum;

typedef enum
{
    OLED_6X8_FONT                       = 0,                                    // 6x8      字体
    OLED_8X16_FONT                      = 1,                                    // 8x16     字体
    OLED_16X16_FONT                     = 2,                                    // 16x16    字体 目前不支持
}oled_font_size_enum;
//=================================================定义 OLED 参数结构体===============================================

//===================================================OLED 基础函数==================================================
void    oled_clear                      (void);
void    oled_full                       (const uint8 color);
void    oled_set_dir                    (oled_dir_enum dir);
void    oled_set_font                   (oled_font_size_enum font);
void    oled_draw_point                 (uint16 x, uint16 y, const uint8 color);

void    oled_show_string                (uint16 x, uint16 y, const char ch[]);
void    oled_show_int                   (uint16 x, uint16 y, const int32 dat, uint8 num);
void    oled_show_uint                  (uint16 x, uint16 y, const uint32 dat, uint8 num);
void    oled_show_float                 (uint16 x, uint16 y, const float dat, uint8 num, uint8 pointnum);

void    oled_show_binary_image          (uint16 x, uint16 y, const uint8 *image, uint16 width, uint16 height, uint16 dis_width, uint16 dis_height);
void    oled_show_gray_image            (uint16 x, uint16 y, const uint8 *image, uint16 width, uint16 height, uint16 dis_width, uint16 dis_height, uint8 threshold);

void    oled_show_wave                  (uint16 x, uint16 y, const uint16 *image, uint16 width, uint16 value_max, uint16 dis_width, uint16 dis_value_max);
void    oled_show_chinese               (uint16 x, uint16 y, uint8 size, const uint8 *chinese_buffer, uint8 number);
void    oled_init                       (void);
//===================================================OLED 基础函数==================================================

//===================================================OLED 扩展函数==================================================
//-------------------------------------------------------------------------------------------------------------------
// 函数简介     OLED 显示小钻风图像
// 参数说明     p               图像数组指针
// 参数说明     width           图像实际宽度
// 参数说明     height          图像实际高度
// 返回参数     void
// 使用示例     oled_displayimage7725(ov7725_image_binary[0], OV7725_W, OV7725_H);
// 备注信息     拓展的一键显示函数，默认缩放至64x128显示
//-------------------------------------------------------------------------------------------------------------------
#define oled_displayimage7725(p,width,height)                                   (oled_show_binary_image(0, 0, (p), (width), (height), 128, 64))
//-------------------------------------------------------------------------------------------------------------------
// 函数简介     OLED 显示总钻风图像
// 参数说明     p               图像数组指针
// 参数说明     width           图像实际宽度
// 参数说明     height          图像实际高度
// 参数说明     x               对比度（OLED屏幕无法显示灰度，必须二值化）
// 返回参数     void
// 使用示例     oled_displayimage03x(mt9v03x_image[0], MT9V03X_W, MT9V03X_H, 100);
// 备注信息     拓展的一键显示函数，默认缩放至64x128显示
//-------------------------------------------------------------------------------------------------------------------
#define oled_displayimage03x(p,width,height,x)                                  (oled_show_gray_image(0, 0, (p), (width), (height), 128, 64, (x)))
//-------------------------------------------------------------------------------------------------------------------
// 函数简介     OLED 显示总钻风图像
// 参数说明     p               图像数组指针
// 参数说明     width           图像实际宽度
// 参数说明     height          图像实际高度
// 参数说明     dis_width       图像缩放宽度
// 参数说明     dis_height      图像缩放高度
// 参数说明     x               对比度（OLED屏幕无法显示灰度，必须二值化）
// 返回参数     void
// 使用示例     oled_displayimage03x_zoom(mt9v03x_image[0], 78, 50, 100);
// 备注信息     拓展的一键显示函数，用户可以自定义缩放后显示的图像大小
//-------------------------------------------------------------------------------------------------------------------
#define oled_displayimage03x_zoom(p,width,height,dis_width,dis_height,x)        (oled_show_gray_image(0, 0, (p), (width), (height), (dis_width,) (dis_height), (x)))

//===================================================OLED 扩展函数==================================================



#endif
