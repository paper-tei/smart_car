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
* 文件名称          zf_device_dl1a_2
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
*                   ------------------------------------
*                   模块管脚            单片机管脚
*                   SCL                 查看 zf_device_dl1a_2.h 中 DL1A_2_SCL_PIN  宏定义
*                   SDA                 查看 zf_device_dl1a_2.h 中 DL1A_2_SDA_PIN  宏定义
*                   VCC                 5V 电源
*                   GND                 电源地
*                   ------------------------------------
*                   ------------------------------------
********************************************************************************************************************/

#ifndef _ZF_DEVICE_DL1A_2_H_
#define _ZF_DEVICE_DL1A_2_H_

#include "zf_common_typedef.h"


// 需要注意的是 DL1A 最高支持 400KHz 的 IIC 通信速率
// 需要注意的是 DL1A 最高支持 400KHz 的 IIC 通信速率
// 需要注意的是 DL1A 最高支持 400KHz 的 IIC 通信速率

#define DL1A_2_USE_SOFT_IIC        (1)                                         // 默认使用软件 IIC 方式驱动 建议使用软件 IIC 方式
#if DL1A_2_USE_SOFT_IIC                                                        // 这两段 颜色正常的才是正确的 颜色灰的就是没有用的
//====================================================软件 IIC 驱动====================================================
#define DL1A_2_SOFT_IIC_DELAY      (100)                                       // 软件 IIC 的时钟延时周期 数值越小 IIC 通信速率越快
// #define DL1A_SCL_PIN             (P33_13)                                     // 软件 IIC SCL 引脚 连接 VL53L0X 的 SCL 引脚
// #define DL1A_SDA_PIN             (P32_4)                                     // 软件 IIC SDA 引脚 连接 VL53L0X 的 SDA 引脚
#define DL1A_2_SCL_PIN             (P11_6)                                     // 软件 IIC SCL 引脚 连接 VL53L0X 的 SCL 引脚
#define DL1A_2_SDA_PIN             (P11_3)                                     // 软件 IIC SDA 引脚 连接 VL53L0X 的 SDA 引脚
//====================================================软件 IIC 驱动====================================================
#else
#error "暂不支持硬件IIC通讯"
#endif
//TOF2

#define DL1A_2_XSHUT_PIN           (P15_7)
// #define DL1A_XSHUT_PIN           (P33_11)
#define DL1A_2_TIMEOUT_COUNT       (0x00FF)                                    // VL53L0X 超时计数

//================================================定义 DL1A 内部地址================================================
#define DL1A_2_DEV_ADDR                                       (0x52 >> 1)         // 0b0101001

#define DL1A_2_SYSRANGE_START                                 (0x00)

#define DL1A_2_SYSTEM_SEQUENCE_CONFIG                         (0x01)
#define DL1A_2_SYSTEM_INTERMEASUREMENT_PERIOD                 (0x04)
#define DL1A_2_SYSTEM_RANGE_CONFIG                            (0x09)
#define DL1A_2_SYSTEM_INTERRUPT_GPIO_CONFIG                   (0x0A)
#define DL1A_2_SYSTEM_INTERRUPT_CLEAR                         (0x0B)
#define DL1A_2_SYSTEM_THRESH_HIGH                             (0x0C)
#define DL1A_2_SYSTEM_THRESH_LOW                              (0x0E)
#define DL1A_2_SYSTEM_HISTOGRAM_BIN                           (0x81)

#define DL1A_2_RESULT_INTERRUPT_STATUS                        (0x13)
#define DL1A_2_RESULT_RANGE_STATUS                            (0x14)
#define DL1A_2_RESULT_PEAK_SIGNAL_RATE_REF                    (0xB6)
#define DL1A_2_RESULT_CORE_AMBIENT_WINDOW_EVENTS_RTN          (0xBC)
#define DL1A_2_RESULT_CORE_RANGING_TOTAL_EVENTS_RTN           (0xC0)
#define DL1A_2_RESULT_CORE_AMBIENT_WINDOW_EVENTS_REF          (0xD0)
#define DL1A_2_RESULT_CORE_RANGING_TOTAL_EVENTS_REF           (0xD4)

#define DL1A_2_PRE_RANGE_CONFIG_MIN_SNR                       (0x27)
#define DL1A_2_PRE_RANGE_CONFIG_VCSEL_PERIOD                  (0x50)
#define DL1A_2_PRE_RANGE_CONFIG_TIMEOUT_MACROP_HI             (0x51)
#define DL1A_2_PRE_RANGE_CONFIG_TIMEOUT_MACROP_LO             (0x52)
#define DL1A_2_PRE_RANGE_CONFIG_VALID_PHASE_LOW               (0x56)
#define DL1A_2_PRE_RANGE_CONFIG_VALID_PHASE_HIGH              (0x57)
#define DL1A_2_PRE_RANGE_CONFIG_SIGMA_THRESH_HI               (0x61)
#define DL1A_2_PRE_RANGE_CONFIG_SIGMA_THRESH_LO               (0x62)
#define DL1A_2_PRE_RANGE_MIN_COUNT_RATE_RTN_LIMIT             (0x64)

#define DL1A_2_FINAL_RANGE_CONFIG_VALID_PHASE_LOW             (0x47)
#define DL1A_2_FINAL_RANGE_CONFIG_VALID_PHASE_HIGH            (0x48)
#define DL1A_2_FINAL_RANGE_CONFIG_MIN_COUNT_RATE_RTN_LIMIT    (0x44)
#define DL1A_2_FINAL_RANGE_CONFIG_MIN_SNR                     (0x67)
#define DL1A_2_FINAL_RANGE_CONFIG_VCSEL_PERIOD                (0x70)
#define DL1A_2_FINAL_RANGE_CONFIG_TIMEOUT_MACROP_HI           (0x71)
#define DL1A_2_FINAL_RANGE_CONFIG_TIMEOUT_MACROP_LO           (0x72)

#define DL1A_2_GLOBAL_CONFIG_VCSEL_WIDTH                      (0x32)
#define DL1A_2_GLOBAL_CONFIG_SPAD_ENABLES_REF_0               (0xB0)
#define DL1A_2_GLOBAL_CONFIG_SPAD_ENABLES_REF_1               (0xB1)
#define DL1A_2_GLOBAL_CONFIG_SPAD_ENABLES_REF_2               (0xB2)
#define DL1A_2_GLOBAL_CONFIG_SPAD_ENABLES_REF_3               (0xB3)
#define DL1A_2_GLOBAL_CONFIG_SPAD_ENABLES_REF_4               (0xB4)
#define DL1A_2_GLOBAL_CONFIG_SPAD_ENABLES_REF_5               (0xB5)
#define DL1A_2_GLOBAL_CONFIG_REF_EN_START_SELECT              (0xB6)

#define DL1A_2_ALGO_PART_TO_PART_RANGE_OFFSET_MM              (0x28)
#define DL1A_2_ALGO_PHASECAL_LIM                              (0x30)
#define DL1A_2_ALGO_PHASECAL_CONFIG_TIMEOUT                   (0x30)

#define DL1A_2_HISTOGRAM_CONFIG_INITIAL_PHASE_SELECT          (0x33)
#define DL1A_2_HISTOGRAM_CONFIG_READOUT_CTRL                  (0x55)

#define DL1A_2_DYNAMIC_SPAD_NUM_REQUESTED_REF_SPAD            (0x4E)
#define DL1A_2_DYNAMIC_SPAD_REF_EN_START_OFFSET               (0x4F)

#define DL1A_2_MSRC_CONFIG_TIMEOUT_MACROP                     (0x46)
#define DL1A_2_MSRC_CONFIG                                    (0x60)

#define DL1A_2_IDENTIFICATION_MODEL_ID                        (0xC0)
#define DL1A_2_IDENTIFICATION_REVISION_ID                     (0xC2)

#define DL1A_2_CROSSTALK_COMPENSATION_PEAK_RATE_MCPS          (0x20)

#define DL1A_2_POWER_MANAGEMENT_GO1_POWER_FORCE               (0x80)

#define DL1A_2_GPIO_HV_MUX_ACTIVE_HIGH                        (0x84)

#define DL1A_2_I2C_SLAVE_DEVICE_ADDRESS                       (0x8A)

#define DL1A_2_SOFT_RESET_GO2_SOFT_RESET_N                    (0xBF)

#define DL1A_2_OSC_CALIBRATE_VAL                              (0xF8)

#define DL1A_2_IO_VOLTAGE_CONFIG                              (0x89)              // IO 电压设置寄存器地址 默认 1V8 使用修改为 2V8

//================================================定义 DL1A_2 内部地址================================================

#define DL1A_2_MIN_TIMING_BUDGET                              (20000)

#define DL1A_2_GET_START_OVERHEAD                             (1910)
#define DL1A_2_SET_START_OVERHEAD                             (1320)
#define DL1A_2_END_OVERHEAD                                   (960 )
#define DL1A_2_TCC_OVERHEAD                                   (590 )
#define DL1A_2_DSS_OVERHEAD                                   (690 )
#define DL1A_2_MSRC_OVERHEAD                                  (660 )
#define DL1A_2_PRERANGE_OVERHEAD                              (660 )
#define DL1A_2_FINALlRANGE_OVERHEAD                           (550 )

typedef enum
{
    DL1A_2_VCSEL_PERIOD_PER_RANGE,
    DL1A_2_VCSEL_PERIOD_FINAL_RANGE,
}dl1a_2_vcsel_period_type_enum;

typedef struct
{
    uint8 tcc;
    uint8 msrc;
    uint8 dss;
    uint8 pre_range;
    uint8 final_range;
}dl1a_2_sequence_enables_step_struct;

typedef struct
{
    uint16 pre_range_vcsel_period_pclks;
    uint16 final_range_vcsel_period_pclks;

    uint16 msrc_dss_tcc_mclks;
    uint16 pre_range_mclks;
    uint16 final_range_mclks;
    uint32 msrc_dss_tcc_us;
    uint32 pre_range_us;
    uint32 final_range_us;
}dl1a_2_sequence_timeout_step_struct;

extern uint8 dl1a_2_finsh_flag;
extern uint16 dl1a_2_distance_mm;

void   dl1a_2_get_distance (void);

uint8  dl1a_2_init         (void);

#endif

