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
* 文件名称          zf_driver_soft_iic
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

#ifndef _zf_driver_soft_iic_h_
#define _zf_driver_soft_iic_h_

#include "zf_common_typedef.h"
#include "zf_driver_gpio.h"

typedef struct
{
    uint32              scl_pin;                                                // 用于记录对应的引脚编号
    uint32              sda_pin;                                                // 用于记录对应的引脚编号
    uint8               addr;                                                   // 器件地址 七位地址模式
    uint32              delay;                                                  // 模拟 IIC 软延时时长  0：1370KHz 10：1020KHz 20：757KHz 30: 633KHz  40: 532Khz  50: 448KHz  60: 395KHz  70: 359KHz  80: 324KHz  100: 268KHz  1000：32KHz
    void               *iic_scl;                                               // 记录 SCL 端口地址
    void               *iic_sda;                                               // 记录 SDA 端口地址
}soft_iic_info_struct;

//==================================================SOFT_IIC 基础函数====================================================
void        soft_iic_write_8bit             (soft_iic_info_struct *soft_iic_obj, const uint8 data);
void        soft_iic_write_8bit_array       (soft_iic_info_struct *soft_iic_obj, const uint8 *data, uint32 len);

void        soft_iic_write_16bit            (soft_iic_info_struct *soft_iic_obj, const uint16 data);
void        soft_iic_write_16bit_array      (soft_iic_info_struct *soft_iic_obj, const uint16 *data, uint32 len);

void        soft_iic_write_8bit_register    (soft_iic_info_struct *soft_iic_obj, const uint8 register_name, const uint8 data);
void        soft_iic_write_8bit_registers   (soft_iic_info_struct *soft_iic_obj, const uint8 register_name, const uint8 *data, uint32 len);

void        soft_iic_write_16bit_register   (soft_iic_info_struct *soft_iic_obj, const uint16 register_name, const uint16 data);
void        soft_iic_write_16bit_registers  (soft_iic_info_struct *soft_iic_obj, const uint16 register_name, const uint16 *data, uint32 len);

uint8       soft_iic_read_8bit              (soft_iic_info_struct *soft_iic_obj);
void        soft_iic_read_8bit_array        (soft_iic_info_struct *soft_iic_obj, uint8 *data, uint32 len);

uint16      soft_iic_read_16bit             (soft_iic_info_struct *soft_iic_obj);
void        soft_iic_read_16bit_array       (soft_iic_info_struct *soft_iic_obj, uint16 *data, uint32 len);

uint8       soft_iic_read_8bit_register     (soft_iic_info_struct *soft_iic_obj, const uint8 register_name);
void        soft_iic_read_8bit_registers    (soft_iic_info_struct *soft_iic_obj, const uint8 register_name, uint8 *data, uint32 len);

uint16      soft_iic_read_16bit_register    (soft_iic_info_struct *soft_iic_obj, const uint16 register_name);
void        soft_iic_read_16bit_registers   (soft_iic_info_struct *soft_iic_obj, const uint16 register_name, uint16 *data, uint32 len);

void        soft_iic_sccb_write_register    (soft_iic_info_struct *soft_iic_obj, const uint8 register_name, uint8 data);
uint8       soft_iic_sccb_read_register     (soft_iic_info_struct *soft_iic_obj, const uint8 register_name);

void        soft_iic_init                   (soft_iic_info_struct *soft_iic_obj, uint8 addr, uint32 delay, gpio_pin_enum scl_pin, gpio_pin_enum sda_pin);
//==================================================SOFT_IIC 基础函数====================================================


#endif
