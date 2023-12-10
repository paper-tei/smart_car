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
* 文件名称          zf_device_ov7725
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
*                  TXD/SCL            查看 zf_device_ov7725.h 中 OV7725_COF_UART_TX 或 OV7725_COF_IIC_SCL 宏定义
*                  RXD/SDA            查看 zf_device_ov7725.h 中 OV7725_COF_UART_RX 或 OV7725_COF_IIC_SDA 宏定义
*                  PCLK               查看 zf_device_ov7725.h 中 OV7725_PCLK_PIN 宏定义
*                  VSY                查看 zf_device_ov7725.h 中 OV7725_VSYNC_PIN 宏定义
*                  D0-D7              查看 zf_device_ov7725.h 中 OV7725_DATA_PIN 宏定义 从该定义开始的连续八个引脚
*                  VCC                3.3V电源
*                  GND                电源地
*                  其余引脚悬空
*                  ------------------------------------
********************************************************************************************************************/

#ifndef _zf_device_ov7725_h_
#define _zf_device_ov7725_h_

#include "zf_device_type.h"

//=================================================OV7725 驱动配置====================================================
#define OV7725_COF_UART         (UART_1)                                        // 小钻风配置串口
#define OV7725_COF_BAUR         (9600)                                          // 小钻风配置串口波特率
#define OV7725_COF_UART_TX      (UART1_RX_P02_3)                                // 小钻风 UART-TX 引脚 要接在单片机 RX 上
#define OV7725_COF_UART_RX      (UART1_TX_P02_2)                                // 小钻风 UART-RX 引脚 要接在单片机 TX 上

#define OV7725_COF_IIC_DELAY    (640 )                                          // 小钻风 IIC 延时
#define OV7725_COF_IIC_SCL      (P02_3)                                         // 小钻风 IIC-SCL 引脚
#define OV7725_COF_IIC_SDA      (P02_2)                                         // 小钻风 IIC-SDA 引脚

#define OV7725_DMA_CH           (IfxDma_ChannelId_5)                            // ERU触发DMA通道禁止随意修改

#define OV7725_PCLK_PIN         (ERU_CH2_REQ14_P02_1)                           // GPIO触发TIM引脚禁止随意修改

#define OV7725_VSYNC_PIN        (ERU_CH3_REQ6_P02_0 )                           // 场中断引脚
#define OV7725_VSYNC_PORT_PIN   (P02_0 )                                        // 场中断引脚

#define OV7725_DATA_PIN         (P00_0 )                                        // 数据引脚 这里是 只能是 GPIOx0 或者 GPIOx8 开始 连续八个引脚例如 F0-F7
#define OV7725_DATA_ADD         get_port_in_addr(OV7725_DATA_PIN)

#define OV7725_INIT_TIMEOUT     (0x0080)                                        // 默认的摄像头初始化超时时间 毫秒为单位
//=================================================OV7725 驱动配置====================================================

//=================================================OV7725 参数配置====================================================
#define OV7725_W                (160)                                           // 图像宽度 80/160/240/320
#define OV7725_H                (120)                                           // 图像高度 60/120/180/240
#define OV7725_IMAGE_SIZE       (OV7725_W * OV7725_H / 8)                       // 整体图像大小 OV7725_IMAGE_SIZE 不能超过 65535

#define OV7725_CONTRAST_DEF     (0x30)                                          // 阈值设置 摄像头二值化阈值 过大和过小的数值会被摄像头自动修正
#define OV7725_FPS_DEF          (50 )                                           // 帧率设置 最高 150 帧 但最小分辨率才能达到最高帧率
//=================================================OV7725 参数配置====================================================

//==========================================IIC 版本小钻风寄存器 本部分不允许用户更改============================================
#define OV7725_ID               (0x21)                                          // 摄像头ID号
#define OV7725_GAIN             (0x00)                                          // 以下为摄像头寄存器
#define OV7725_BLUE             (0x01)
#define OV7725_RED              (0x02)
#define OV7725_GREEN            (0x03)
#define OV7725_BAVG             (0x05)
#define OV7725_GAVG             (0x06)
#define OV7725_RAVG             (0x07)
#define OV7725_AECH             (0x08)
#define OV7725_COM2             (0x09)
#define OV7725_PID              (0x0A)
#define OV7725_VER              (0x0B)
#define OV7725_COM3             (0x0C)
#define OV7725_COM4             (0x0D)
#define OV7725_COM5             (0x0E)
#define OV7725_COM6             (0x0F)
#define OV7725_AEC              (0x10)
#define OV7725_CLKRC            (0x11)
#define OV7725_COM7             (0x12)
#define OV7725_COM8             (0x13)
#define OV7725_COM9             (0x14)
#define OV7725_COM10            (0x15)
#define OV7725_REG16            (0x16)
#define OV7725_HSTART           (0x17)
#define OV7725_HSIZE            (0x18)
#define OV7725_VSTRT            (0x19)
#define OV7725_VSIZE            (0x1A)
#define OV7725_PSHFT            (0x1B)
#define OV7725_MIDH             (0x1C)
#define OV7725_MIDL             (0x1D)
#define OV7725_LAEC             (0x1F)
#define OV7725_COM11            (0x20)
#define OV7725_BDBase           (0x22)
#define OV7725_BDMStep          (0x23)
#define OV7725_AEW              (0x24)
#define OV7725_AEB              (0x25)
#define OV7725_VPT              (0x26)
#define OV7725_REG28            (0x28)
#define OV7725_HOutSize         (0x29)
#define OV7725_EXHCH            (0x2A)
#define OV7725_EXHCL            (0x2B)
#define OV7725_VOutSize         (0x2C)
#define OV7725_ADVFL            (0x2D)
#define OV7725_ADVFH            (0x2E)
#define OV7725_YAVE             (0x2F)
#define OV7725_LumHTh           (0x30)
#define OV7725_LumLTh           (0x31)
#define OV7725_HREF             (0x32)
#define OV7725_DM_LNL           (0x33)
#define OV7725_DM_LNH           (0x34)
#define OV7725_ADoff_B          (0x35)
#define OV7725_ADoff_R          (0x36)
#define OV7725_ADoff_Gb         (0x37)
#define OV7725_ADoff_Gr         (0x38)
#define OV7725_Off_B            (0x39)
#define OV7725_Off_R            (0x3A)
#define OV7725_Off_Gb           (0x3B)
#define OV7725_Off_Gr           (0x3C)
#define OV7725_COM12            (0x3D)
#define OV7725_COM13            (0x3E)
#define OV7725_COM14            (0x3F)
#define OV7725_COM16            (0x41)
#define OV7725_TGT_B            (0x42)
#define OV7725_TGT_R            (0x43)
#define OV7725_TGT_Gb           (0x44)
#define OV7725_TGT_Gr           (0x45)
#define OV7725_LC_CTR           (0x46)
#define OV7725_LC_XC            (0x47)
#define OV7725_LC_YC            (0x48)
#define OV7725_LC_COEF          (0x49)
#define OV7725_LC_RADI          (0x4A)
#define OV7725_LC_COEFB         (0x4B)
#define OV7725_LC_COEFR         (0x4C)
#define OV7725_FixGain          (0x4D)
#define OV7725_AREF1            (0x4F)
#define OV7725_AREF6            (0x54)
#define OV7725_UFix             (0x60)
#define OV7725_VFix             (0x61)
#define OV7725_AWBb_blk         (0x62)
#define OV7725_AWB_Ctrl0        (0x63)
#define OV7725_DSP_Ctrl1        (0x64)
#define OV7725_DSP_Ctrl2        (0x65)
#define OV7725_DSP_Ctrl3        (0x66)
#define OV7725_DSP_Ctrl4        (0x67)
#define OV7725_AWB_bias         (0x68)
#define OV7725_AWBCtrl1         (0x69)
#define OV7725_AWBCtrl2         (0x6A)
#define OV7725_AWBCtrl3         (0x6B)
#define OV7725_AWBCtrl4         (0x6C)
#define OV7725_AWBCtrl5         (0x6D)
#define OV7725_AWBCtrl6         (0x6E)
#define OV7725_AWBCtrl7         (0x6F)
#define OV7725_AWBCtrl8         (0x70)
#define OV7725_AWBCtrl9         (0x71)
#define OV7725_AWBCtrl10        (0x72)
#define OV7725_AWBCtrl11        (0x73)
#define OV7725_AWBCtrl12        (0x74)
#define OV7725_AWBCtrl13        (0x75)
#define OV7725_AWBCtrl14        (0x76)
#define OV7725_AWBCtrl15        (0x77)
#define OV7725_AWBCtrl16        (0x78)
#define OV7725_AWBCtrl17        (0x79)
#define OV7725_AWBCtrl18        (0x7A)
#define OV7725_AWBCtrl19        (0x7B)
#define OV7725_AWBCtrl20        (0x7C)
#define OV7725_AWBCtrl21        (0x7D)
#define OV7725_GAM1             (0x7E)
#define OV7725_GAM2             (0x7F)
#define OV7725_GAM3             (0x80)
#define OV7725_GAM4             (0x81)
#define OV7725_GAM5             (0x82)
#define OV7725_GAM6             (0x83)
#define OV7725_GAM7             (0x84)
#define OV7725_GAM8             (0x85)
#define OV7725_GAM9             (0x86)
#define OV7725_GAM10            (0x87)
#define OV7725_GAM11            (0x88)
#define OV7725_GAM12            (0x89)
#define OV7725_GAM13            (0x8A)
#define OV7725_GAM14            (0x8B)
#define OV7725_GAM15            (0x8C)
#define OV7725_SLOP             (0x8D)
#define OV7725_DNSTh            (0x8E)
#define OV7725_EDGE0            (0x8F)
#define OV7725_EDGE1            (0x90)
#define OV7725_DNSOff           (0x91)
#define OV7725_EDGE2            (0x92)
#define OV7725_EDGE3            (0x93)
#define OV7725_MTX1             (0x94)
#define OV7725_MTX2             (0x95)
#define OV7725_MTX3             (0x96)
#define OV7725_MTX4             (0x97)
#define OV7725_MTX5             (0x98)
#define OV7725_MTX6             (0x99)
#define OV7725_MTX_Ctrl         (0x9A)
#define OV7725_BRIGHT           (0x9B)
#define OV7725_CNST             (0x9C)
#define OV7725_UVADJ0           (0x9E)
#define OV7725_UVADJ1           (0x9F)
#define OV7725_SCAL0            (0xA0)
#define OV7725_SCAL1            (0xA1)
#define OV7725_SCAL2            (0xA2)
#define OV7725_SDE              (0xA6)
#define OV7725_USAT             (0xA7)
#define OV7725_VSAT             (0xA8)
#define OV7725_HUECOS           (0xA9)
#define OV7725_HUESIN           (0xAA)
#define OV7725_SIGN             (0xAB)
#define OV7725_DSPAuto          (0xAC)
#define OV7725_DEV_ADD          (0x42 >> 1)
//==========================================IIC 版本小钻风寄存器 本部分不允许用户更改============================================

//==============================================定义 OV7725 命令枚举体==================================================
typedef enum
{
    OV7725_INIT                 = 0x00,
    OV7725_RESERVE,
    OV7725_CONTRAST,
    OV7725_FPS,
    OV7725_COL,
    OV7725_ROW,
    OV7725_CONFIG_FINISH,

    OV7725_GET_WHO_AM_I         = 0xEF,
    OV7725_GET_STATUS           = 0xF1,
    OV7725_GET_VERSION          = 0xF2,

    OV7725_SET_ADDR             = 0xFE,
    OV7725_SET_DATA             = 0xFF,
}ov7725_cmd_enum;
//==============================================定义 OV7725 命令枚举体==================================================

//==============================================声明 OV7725 数据存储变量=================================================
extern vuint8       ov7725_finish_flag;                                     // 一场图像采集完成标志位
extern uint8        ov7725_image_binary[OV7725_H][OV7725_W / 8];            // 图像保存数组
//==============================================声明 OV7725 数据存储变量=================================================

//=================================================OV7725 基础函数===================================================
uint16      ov7725_uart_get_id      (void);                                 // 获取摄像头固件 ID
uint16      ov7725_get_version      (void);                                 // 获取摄像头固件版本
uint8       ov7725_init             (void);                                 // OV7725 摄像头初始化
//=================================================OV7725 基础函数===================================================


#endif
