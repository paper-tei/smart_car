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
* 文件名称          zf_common_font
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

#ifndef _zf_common_font_h
#define _zf_common_font_h

#include "zf_common_typedef.h"

//-------常用颜色----------
typedef enum
{
    RGB565_WHITE    = (0xFFFF),                                                     // 白色
    RGB565_BLACK    = (0x0000),                                                     // 黑色
    RGB565_BLUE     = (0x001F),                                                     // 蓝色
    RGB565_PURPLE   = (0xF81F),                                                     // 紫色
    RGB565_PINK     = (0xFE19),                                                     // 粉色
    RGB565_RED      = (0xF800),                                                     // 红色
    RGB565_MAGENTA  = (0xF81F),                                                     // 品红
    RGB565_GREEN    = (0x07E0),                                                     // 绿色
    RGB565_CYAN     = (0x07FF),                                                     // 青色
    RGB565_YELLOW   = (0xFFE0),                                                     // 黄色
    RGB565_BROWN    = (0xBC40),                                                     // 棕色
    RGB565_GRAY     = (0x8430),                                                     // 灰色

    RGB565_39C5BB   = (0x3616),
    RGB565_66CCFF   = (0x665F),
}rgb565_color_enum;

extern const uint8      ascii_font_8x16[][16];
extern const uint8      ascii_font_6x8[][6];
extern const uint8      chinese_test[8][16];
extern const uint8      oled_16x16_chinese[][16];
extern const uint8      gImage_seekfree_logo[38400];


#endif
