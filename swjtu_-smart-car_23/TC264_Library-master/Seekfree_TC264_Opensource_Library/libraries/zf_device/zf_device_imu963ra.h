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
* 文件名称          zf_device_imu963ra
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
*                  模块管脚             单片机管脚
*                  // 硬件 SPI 引脚
*                  SCL/SPC            查看 zf_device_imu963ra.h 中 IMU963RA_SPC_PIN 宏定义
*                  SDA/DSI            查看 zf_device_imu963ra.h 中 IMU963RA_SDI_PIN 宏定义
*                  SA0/SDO            查看 zf_device_imu963ra.h 中 IMU963RA_SDO_PIN 宏定义
*                  CS                 查看 zf_device_imu963ra.h 中 IMU963RA_CS_PIN  宏定义
*                  VCC                3.3V电源
*                  GND                电源地
*                  其余引脚悬空
*
*                  // 软件 IIC 引脚
*                  SCL/SPC            查看 zf_device_imu963ra.h 中 IMU963RA_SCL_PIN 宏定义
*                  SDA/DSI            查看 zf_device_imu963ra.h 中 IMU963RA_SDA_PIN 宏定义
*                  VCC                3.3V电源
*                  GND                电源地
*                  其余引脚悬空
*                  ------------------------------------
********************************************************************************************************************/

#ifndef _zf_device_imu963ra_h_
#define _zf_device_imu963ra_h_

#include "zf_common_typedef.h"

#define IMU963RA_USE_SOFT_IIC                       (0)                         // 默认使用硬件 SPI 方式驱动
#if IMU963RA_USE_SOFT_IIC                                                       // 这两段 颜色正常的才是正确的 颜色灰的就是没有用的
//====================================================软件 IIC 驱动====================================================
#define IMU963RA_SOFT_IIC_DELAY                     (59 )                       // 软件 IIC 的时钟延时周期 数值越小 IIC 通信速率越快
#define IMU963RA_SCL_PIN                            (P20_11)                    // 软件 IIC SCL 引脚 连接 IMU963RA 的 SCL 引脚
#define IMU963RA_SDA_PIN                            (P20_14)                    // 软件 IIC SDA 引脚 连接 IMU963RA 的 SDA 引脚
//====================================================软件 IIC 驱动====================================================
#else
//====================================================硬件 SPI 驱动====================================================
#define IMU963RA_SPI_SPEED                          (10 * 1000 * 1000)          // 硬件 SPI 速率
#define IMU963RA_SPI                                (SPI_0           )          // 硬件 SPI 号
#define IMU963RA_SPC_PIN                            (SPI0_SCLK_P20_11)          // 硬件 SPI SCK 引脚
#define IMU963RA_SDI_PIN                            (SPI0_MOSI_P20_14)          // 硬件 SPI MOSI 引脚
#define IMU963RA_SDO_PIN                            (SPI0_MISO_P20_12)          // 硬件 SPI MISO 引脚
//====================================================硬件 SPI 驱动====================================================
#endif

#define IMU963RA_CS_PIN                             (P20_13)                    // CS 片选引脚
#define IMU963RA_CS(x)                              (x? (gpio_high(IMU963RA_CS_PIN)): (gpio_low(IMU963RA_CS_PIN)))

#define IMU963RA_TIMEOUT_COUNT                      (0x00FF)                    // IMU963RA 超时计数

//================================================定义 IMU963RA 内部地址================================================
#define IMU963RA_DEV_ADDR                           (0x6B)                      // SA0接地：0x6A SA0上拉：0x6B 模块默认上拉
#define IMU963RA_SPI_W                              (0x00)
#define IMU963RA_SPI_R                              (0x80)

#define IMU963RA_FUNC_CFG_ACCESS                    (0x01)
#define IMU963RA_INT1_CTRL                          (0x0D)
#define IMU963RA_WHO_AM_I                           (0x0F)
#define IMU963RA_CTRL1_XL                           (0x10)
#define IMU963RA_CTRL2_G                            (0x11)
#define IMU963RA_CTRL3_C                            (0x12)
#define IMU963RA_CTRL4_C                            (0x13)
#define IMU963RA_CTRL5_C                            (0x14)
#define IMU963RA_CTRL6_C                            (0x15)
#define IMU963RA_CTRL7_G                            (0x16)
#define IMU963RA_CTRL9_XL                           (0x18)
#define IMU963RA_OUTX_L_G                           (0x22)
#define IMU963RA_OUTX_L_A                           (0x28)

//集线器功能相关寄存器 需要将FUNC_CFG_ACCESS的SHUB_REG_ACCESS位设置为1才能正确访问
#define IMU963RA_SENSOR_HUB_1                       (0x02)
#define IMU963RA_MASTER_CONFIG                      (0x14)
#define IMU963RA_SLV0_ADD                           (0x15)
#define IMU963RA_SLV0_SUBADD                        (0x16)
#define IMU963RA_SLV0_CONFIG                        (0x17)
#define IMU963RA_DATAWRITE_SLV0                     (0x21)
#define IMU963RA_STATUS_MASTER                      (0x22)

#define IMU963RA_MAG_ADDR                           (0x0D)                      // 7位IIC地址
#define IMU963RA_MAG_OUTX_L                         (0x00)
#define IMU963RA_MAG_CONTROL1                       (0x09)
#define IMU963RA_MAG_CONTROL2                       (0x0A)
#define IMU963RA_MAG_FBR                            (0x0B)
#define IMU963RA_MAG_CHIP_ID                        (0x0D)

#define IMU963RA_ACC_SAMPLE                         (0x3C)                      // 加速度计量程
// 设置为:0x30 加速度量程为:±2G      获取到的加速度计数据 除以16393，可以转化为带物理单位的数据，单位：g(m/s^2)
// 设置为:0x38 加速度量程为:±4G      获取到的加速度计数据 除以8197， 可以转化为带物理单位的数据，单位：g(m/s^2)
// 设置为:0x3C 加速度量程为:±8G      获取到的加速度计数据 除以4098， 可以转化为带物理单位的数据，单位：g(m/s^2)
// 设置为:0x34 加速度量程为:±16G     获取到的加速度计数据 除以2049， 可以转化为带物理单位的数据，单位：g(m/s^2)

#define IMU963RA_GYR_SAMPLE                         (0x5C)                      // 陀螺仪量程
// 设置为:0x52 陀螺仪量程为:±125dps  获取到的陀螺仪数据除以228.6，   可以转化为带物理单位的数据，单位为：°/s
// 设置为:0x50 陀螺仪量程为:±250dps  获取到的陀螺仪数据除以114.3，   可以转化为带物理单位的数据，单位为：°/s
// 设置为:0x54 陀螺仪量程为:±500dps  获取到的陀螺仪数据除以57.1，    可以转化为带物理单位的数据，单位为：°/s
// 设置为:0x58 陀螺仪量程为:±1000dps 获取到的陀螺仪数据除以28.6，    可以转化为带物理单位的数据，单位为：°/s
// 设置为:0x5C 陀螺仪量程为:±2000dps 获取到的陀螺仪数据除以14.3，    可以转化为带物理单位的数据，单位为：°/s
// 设置为:0x51 陀螺仪量程为:±4000dps 获取到的陀螺仪数据除以7.1，     可以转化为带物理单位的数据，单位为：°/s

#define IMU963RA_MAG_SAMPLE                         (0x19)                      // 地磁计量程
// 设置为:0x19 磁力计量程为:8G     获取到的加速度计数据 除以3000， 可以转化为带物理单位的数据，单位：G(高斯)
// 设置为:0x09 磁力计量程为:2G     获取到的加速度计数据 除以12000，可以转化为带物理单位的数据，单位：G(高斯)

//================================================定义 IMU963RA 内部地址================================================

//===============================================声明 IMU963RA 数据存储变量===============================================
extern int16 imu963ra_acc_x,  imu963ra_acc_y,  imu963ra_acc_z;          // 三轴陀螺仪数据
extern int16 imu963ra_gyro_x, imu963ra_gyro_y, imu963ra_gyro_z;         // 三轴加速度计数据
extern int16 imu963ra_mag_x,  imu963ra_mag_y,  imu963ra_mag_z;          // 三轴地磁计数据
//===============================================声明 IMU963RA 数据存储变量===============================================

//==================================================IMU963RA 基础函数==================================================
void  imu963ra_get_acc              (void);                                 // 获取 IMU963RA 加速度计数据
void  imu963ra_get_gyro             (void);                                 // 获取 IMU963RA 陀螺仪数据
void  imu963ra_get_mag              (void);                                 // 获取 IMU963RA 磁力计数据
float imu963ra_acc_transition       (int16 acc_value);                      // 将   IMU963RA 加速度计数据转换为实际物理数据
float imu963ra_gyro_transition      (int16 gyro_value);                     // 将   IMU963RA 陀螺仪数据转换为实际物理数据
float imu963ra_mag_transition       (int16 mag_value);                      // 将   IMU963RA 地磁计数据转换为实际物理数据
uint8 imu963ra_init                 (void);                                 // 初始化 IMU963RA
//==================================================IMU963RA 基础函数==================================================


#endif
