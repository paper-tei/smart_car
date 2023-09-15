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
* 文件名称          zf_device_mpu6050
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
*                  软件 IIC 通信引脚对应关系
*                  SCL                查看 zf_device_mpu6050.h 中 MPU6050_SOFT_IIC_SCL 宏定义
*                  SDA                查看 zf_device_mpu6050.h 中 MPU6050_SOFT_IIC_SDA 宏定义
*                  VCC                3.3V电源
*                  GND                电源地
*                  其余引脚悬空
*
*                  硬件 IIC 通信引脚应关系
*                  SCL                查看 zf_device_mpu6050.h 中 MPU6050_IIC_SCL 宏定义
*                  SDA                查看 zf_device_mpu6050.h 中 MPU6050_IIC_SDA 宏定义
*                  VCC                3.3V电源
*                  GND                电源地
*                  其余引脚悬空
*                  ------------------------------------
********************************************************************************************************************/

#include "zf_common_debug.h"
#include "zf_driver_delay.h"
#include "zf_driver_soft_iic.h"
#include "zf_device_mpu6050.h"

int16 mpu6050_gyro_x = 0, mpu6050_gyro_y = 0, mpu6050_gyro_z = 0;                       // 三轴陀螺仪数据      gyro (陀螺仪)
int16 mpu6050_acc_x  = 0, mpu6050_acc_y  = 0, mpu6050_acc_z  = 0;                       // 三轴加速度计数据    acc (accelerometer 加速度计)

#if MPU6050_USE_SOFT_IIC
static soft_iic_info_struct mpu6050_iic_struct;

#define mpu6050_write_register(reg, data)       (soft_iic_write_8bit_register(&mpu6050_iic_struct, (reg), (data)))
#define mpu6050_read_register(reg)              (soft_iic_read_8bit_register(&mpu6050_iic_struct, (reg)))
#define mpu6050_read_registers(reg, data, len)  (soft_iic_read_8bit_registers(&mpu6050_iic_struct, (reg), (data), (len)))
#endif

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     MPU6050 自检
// 参数说明     void
// 返回参数     uint8           1-自检失败 0-自检成功
// 使用示例     if(mpu6050_self1_check())
// 备注信息     内部调用
//-------------------------------------------------------------------------------------------------------------------
static uint8 mpu6050_self1_check (void)
{
    uint8 dat = 0, return_state = 0;
    uint16 timeout_count = 0;

    mpu6050_write_register(MPU6050_PWR_MGMT_1, 0x00);                                   // 解除休眠状态
    mpu6050_write_register(MPU6050_SMPLRT_DIV, 0x07);                                   // 125HZ采样率
    while(0x07 != dat)
    {
        if(timeout_count ++ > MPU6050_TIMEOUT_COUNT)
        {
            return_state =  1;
            break;
        }
        dat = mpu6050_read_register(MPU6050_SMPLRT_DIV);
        system_delay_ms(10);
    }
    return return_state;
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     获取 MPU6050 加速度计数据
// 参数说明     void
// 返回参数     void
// 使用示例     mpu6050_get_acc();                              // 执行该函数后，直接查看对应的变量即可
// 备注信息
//-------------------------------------------------------------------------------------------------------------------
void mpu6050_get_acc (void)
{
    uint8 dat[6];

    mpu6050_read_registers(MPU6050_ACCEL_XOUT_H, dat, 6);
    mpu6050_acc_x = (int16)(((uint16)dat[0] << 8 | dat[1]));
    mpu6050_acc_y = (int16)(((uint16)dat[2] << 8 | dat[3]));
    mpu6050_acc_z = (int16)(((uint16)dat[4] << 8 | dat[5]));
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     获取 MPU6050 陀螺仪数据
// 参数说明     void
// 返回参数     void
// 使用示例     mpu6050_get_gyro();                             // 执行该函数后，直接查看对应的变量即可
// 备注信息
//-------------------------------------------------------------------------------------------------------------------
void mpu6050_get_gyro (void)
{
    uint8 dat[6];

    mpu6050_read_registers(MPU6050_GYRO_XOUT_H, dat, 6);
    mpu6050_gyro_x = (int16)(((uint16)dat[0] << 8 | dat[1]));
    mpu6050_gyro_y = (int16)(((uint16)dat[2] << 8 | dat[3]));
    mpu6050_gyro_z = (int16)(((uint16)dat[4] << 8 | dat[5]));
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     将 MPU6050 加速度计数据转换为实际物理数据
// 参数说明     gyro_value              // 任意轴的加速度计数据
// 返回参数     void
// 使用示例     float data = mpu6050_acc_transition(imu660ra_acc_x);  //单位为 g(m/s^2)
// 备注信息
//-------------------------------------------------------------------------------------------------------------------
float mpu6050_acc_transition (int16 acc_value)
{
    float acc_data = 0;
    switch(MPU6050_ACC_SAMPLE)
    {
        case 0x00: acc_data = (float)acc_value / 16384; break;      // 0x00 加速度计量程为:±2g          获取到的加速度计数据 除以16384      可以转化为带物理单位的数据，单位：g(m/s^2)
        case 0x08: acc_data = (float)acc_value / 8192;  break;      // 0x08 加速度计量程为:±4g          获取到的加速度计数据 除以8192       可以转化为带物理单位的数据，单位：g(m/s^2)
        case 0x10: acc_data = (float)acc_value / 4096;  break;      // 0x10 加速度计量程为:±8g          获取到的加速度计数据 除以4096       可以转化为带物理单位的数据，单位：g(m/s^2)
        case 0x18: acc_data = (float)acc_value / 2048;  break;      // 0x18 加速度计量程为:±16g         获取到的加速度计数据 除以2048       可以转化为带物理单位的数据，单位：g(m/s^2)
        default: break;
    }
    return acc_data;
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     将 MPU6050 陀螺仪数据转换为实际物理数据
// 参数说明     gyro_value              // 任意轴的陀螺仪数据
// 返回参数     void
// 使用示例     float data = mpu6050_gyro_transition(imu660ra_gyro_x);  // 单位为°/s
// 备注信息
//-------------------------------------------------------------------------------------------------------------------
float mpu6050_gyro_transition (int16 gyro_value)
{
    float gyro_data = 0;
    switch(MPU6050_GYR_SAMPLE)
    {
        case 0x00: gyro_data = (float)gyro_value / 131.2f;  break;  //  0x00 陀螺仪量程为:±250 dps     获取到的陀螺仪数据除以131           可以转化为带物理单位的数据，单位为：°/s
        case 0x08: gyro_data = (float)gyro_value / 65.6f;   break;  //  0x08 陀螺仪量程为:±500 dps     获取到的陀螺仪数据除以65.5          可以转化为带物理单位的数据，单位为：°/s
        case 0x10: gyro_data = (float)gyro_value / 32.8f;   break;  //  0x10 陀螺仪量程为:±1000dps     获取到的陀螺仪数据除以32.8          可以转化为带物理单位的数据，单位为：°/s
        case 0x18: gyro_data = (float)gyro_value / 16.4f;   break;  //  0x18 陀螺仪量程为:±2000dps     获取到的陀螺仪数据除以16.4          可以转化为带物理单位的数据，单位为：°/s
        default: break;
    }
    return gyro_data;
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     初始化 MPU6050
// 参数说明     void
// 返回参数     uint8           1-初始化失败 0-初始化成功
// 使用示例     mpu6050_init();
// 备注信息
//-------------------------------------------------------------------------------------------------------------------
uint8 mpu6050_init (void)
{
    uint8 return_state = 0;
#if MPU6050_USE_SOFT_IIC
    soft_iic_init(&mpu6050_iic_struct, MPU6050_DEV_ADDR, MPU6050_SOFT_IIC_DELAY, MPU6050_SCL_PIN, MPU6050_SDA_PIN);
#else
    iic_init(MPU6050_IIC, MPU6050_DEV_ADDR, MPU6050_IIC_SPEED, MPU6050_SCL_PIN, MPU6050_SDA_PIN);
#endif
    system_delay_ms(100);                                                       // 上电延时

    do
    {
        if(mpu6050_self1_check())
        {
            // 如果程序在输出了断言信息 并且提示出错位置在这里
            // 那么就是 MPU6050 自检出错并超时退出了
            // 检查一下接线有没有问题 如果没问题可能就是坏了
            zf_log(0, "MPU6050 self check error.");
            return_state = 1;
            break;
        }
        mpu6050_write_register(MPU6050_PWR_MGMT_1, 0x00);                       // 解除休眠状态
        mpu6050_write_register(MPU6050_SMPLRT_DIV, 0x07);                       // 125HZ采样率
        mpu6050_write_register(MPU6050_CONFIG, 0x04);
        mpu6050_write_register(MPU6050_GYRO_CONFIG, MPU6050_GYR_SAMPLE);       // 2000°/s
        mpu6050_write_register(MPU6050_ACCEL_CONFIG, MPU6050_ACC_SAMPLE);       // 8g(m/s^2)
        mpu6050_write_register(MPU6050_USER_CONTROL, 0x00);
        mpu6050_write_register(MPU6050_INT_PIN_CFG, 0x02);

        // MPU6050_GYRO_CONFIG寄存器
        // 设置为:0x00 陀螺仪量程为:±250 dps       获取到的陀螺仪数据除以131.2        可以转化为带物理单位的数据，单位为：°/s
        // 设置为:0x08 陀螺仪量程为:±500 dps       获取到的陀螺仪数据除以65.6         可以转化为带物理单位的数据，单位为：°/s
        // 设置为:0x10 陀螺仪量程为:±1000dps       获取到的陀螺仪数据除以32.8         可以转化为带物理单位的数据，单位为：°/s
        // 设置为:0x18 陀螺仪量程为:±2000dps       获取到的陀螺仪数据除以16.4         可以转化为带物理单位的数据，单位为：°/s

        // MPU6050_ACCEL_CONFIG寄存器
        // 设置为:0x00 加速度计量程为:±2g          获取到的加速度计数据 除以16384      可以转化为带物理单位的数据，单位：g(m/s^2)
        // 设置为:0x08 加速度计量程为:±4g          获取到的加速度计数据 除以8192       可以转化为带物理单位的数据，单位：g(m/s^2)
        // 设置为:0x10 加速度计量程为:±8g          获取到的加速度计数据 除以4096       可以转化为带物理单位的数据，单位：g(m/s^2)
        // 设置为:0x18 加速度计量程为:±16g         获取到的加速度计数据 除以2048       可以转化为带物理单位的数据，单位：g(m/s^2)


    }while(0);
    return return_state;
}
