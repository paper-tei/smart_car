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
* 文件名称          zf_device_dl1a
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
*                   SCL                 查看 zf_device_dl1a.h 中 DL1A_SCL_PIN  宏定义
*                   SDA                 查看 zf_device_dl1a.h 中 DL1A_SDA_PIN  宏定义
*                   VCC                 5V 电源
*                   GND                 电源地
*                   ------------------------------------
********************************************************************************************************************/

#include "zf_common_debug.h"
#include "zf_driver_delay.h"
#include "zf_driver_soft_iic.h"
#include "zf_device_dl1a.h"

uint8 dl1a_finsh_flag;
uint16 dl1a_distance_mm;

#if DL1A_USE_SOFT_IIC
static soft_iic_info_struct dl1a_iic_struct;

#define dl1a_write_array(data, len)          (soft_iic_write_8bit_array(&dl1a_iic_struct, (data), (len)))
#define dl1a_write_register(reg, data)       (soft_iic_write_8bit_register(&dl1a_iic_struct, (reg), (data)))
#define dl1a_read_register(reg)              (soft_iic_read_8bit_register(&dl1a_iic_struct, (reg)))
#define dl1a_read_registers(reg, data, len)  (soft_iic_read_8bit_registers(&dl1a_iic_struct, (reg), (data), (len)))
#else
#define dl1a_write_array(data, len)          (iic_write_8bit_array(DL1A_IIC, DL1A_DEV_ADDR, (data), (len)))
#define dl1a_write_register(reg, data)       (iic_write_8bit_register(DL1A_IIC, DL1A_DEV_ADDR, (reg), (data)))
#define dl1a_read_register(reg)              (iic_read_8bit_register(DL1A_IIC, DL1A_DEV_ADDR, (reg)))
#define dl1a_read_registers(reg, data, len)  (iic_read_8bit_registers(DL1A_IIC, DL1A_DEV_ADDR, (reg), (data), (len)))
#endif

// 这个速率表示从目标反射并被设备检测到的信号的振幅
// 设置此限制可以确定传感器报告有效读数所需的最小测量值
// 设置一个较低的限制可以增加传感器的测量范围
// 但似乎也增加了 <由于来自目标以外的物体的不需要的反射导致> 得到不准确读数的可能性
// 默认为 0.25 MCPS 可预设范围为 0 - 511.99
#define DL1A_DEFAULT_RATE_LIMIT  (0.25)

// 从寄存器数据解码 PCLKs 中 VCSEL (vertical cavity surface emitting laser) 的脉宽周期
#define decode_vcsel_period(reg_val)            (((reg_val) + 1) << 1)

// 从 PCLK 中的 VCSEL 周期计算宏周期 (以 *纳秒为单位)
// PLL_period_ps = 1655
// macro_period_vclks = 2304
#define calc_macro_period(vcsel_period_pclks)   ((((uint32)2304 * (vcsel_period_pclks) * 1655) + 500) / 1000)

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     获取设备 SPAD 信息
// 参数说明     index           索引
// 参数说明     type            类型值
// 返回参数     uint8           是否成功 0-成功 1-失败
// 使用示例     dl1a_get_spad_info(index, type_is_aperture);
// 备注信息
//-------------------------------------------------------------------------------------------------------------------
static uint8 dl1a_get_spad_info (uint8 *index, uint8 *type_is_aperture)
{
    uint8 tmp = 0;
    uint8 return_state = 0;
    volatile uint16 loop_count = 0;

    do
    {
        dl1a_write_register(0x80, 0x01);
        dl1a_write_register(0xFF, 0x01);
        dl1a_write_register(0x00, 0x00);

        dl1a_write_register(0xFF, 0x06);
        dl1a_read_registers(0x83, &tmp, 1);
        dl1a_write_register(0x83, tmp | 0x04);
        dl1a_write_register(0xFF, 0x07);
        dl1a_write_register(0x81, 0x01);

        dl1a_write_register(0x80, 0x01);

        dl1a_write_register(0x94, 0x6b);
        dl1a_write_register(0x83, 0x00);

        tmp = 0x00;
        while(tmp == 0x00 || tmp == 0xFF)
        {
            system_delay_ms(1);
            dl1a_read_registers(0x83, &tmp, 1);
            if(loop_count++ > DL1A_TIMEOUT_COUNT)
            {
                return_state = 1;
                break;
            }

        }
        if(return_state)
        {
            break;
        }
        dl1a_write_register(0x83, 0x01);
        dl1a_read_registers(0x92, &tmp, 1);

        *index = tmp & 0x7f;
        *type_is_aperture = (tmp >> 7) & 0x01;

        dl1a_write_register(0x81, 0x00);
        dl1a_write_register(0xFF, 0x06);
        dl1a_read_registers(0x83, &tmp, 1);
        dl1a_write_register(0x83, tmp);
        dl1a_write_register(0xFF, 0x01);
        dl1a_write_register(0x00, 0x01);

        dl1a_write_register(0xFF, 0x00);
        dl1a_write_register(0x80, 0x00);
    }while(0);

    return return_state;
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     将超时数值从 MCLKs 转换到对应的 ms
// 参数说明     timeout_period_mclks    超时周期 MCLKs
// 参数说明     vcsel_period_pclks      PCLK 值
// 返回参数     uint32                  返回超时数值
// 使用示例     dl1a_timeout_mclks_to_microseconds(timeout_period_mclks, vcsel_period_pclks);
// 备注信息     将序列步骤超时从具有给定 VCSEL 周期的 MCLK (以 PCLK 为单位)转换为微秒
//-------------------------------------------------------------------------------------------------------------------
static uint32 dl1a_timeout_mclks_to_microseconds (uint16 timeout_period_mclks, uint8 vcsel_period_pclks)
{
    uint32 macro_period_ns = calc_macro_period(vcsel_period_pclks);

    return ((timeout_period_mclks * macro_period_ns) + (macro_period_ns / 2)) / 1000;
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     将超时数值从 ms 转换到对应的 MCLKs
// 参数说明     timeout_period_us   超时周期 微秒单位
// 参数说明     vcsel_period_pclks  PCLK 值
// 返回参数     uint32              返回超时数值
// 使用示例     dl1a_timeout_microseconds_to_mclks(timeout_period_us, vcsel_period_pclks);
// 备注信息     将序列步骤超时从微秒转换为具有给定 VCSEL 周期的 MCLK (以 PCLK 为单位)
//-------------------------------------------------------------------------------------------------------------------
static uint32 dl1a_timeout_microseconds_to_mclks (uint32 timeout_period_us, uint8 vcsel_period_pclks)
{
    uint32 macro_period_ns = calc_macro_period(vcsel_period_pclks);

    return (((timeout_period_us * 1000) + (macro_period_ns / 2)) / macro_period_ns);
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     对超时数值进行解码
// 参数说明     reg_val         超时时长 寄存器值
// 返回参数     uint16          返回超时数值
// 使用示例     dl1a_decode_timeout(reg_val);
// 备注信息     从寄存器值解码 MCLK 中的序列步骤超时
//-------------------------------------------------------------------------------------------------------------------
static uint16 dl1a_decode_timeout (uint16 reg_val)
{
  // 格式: (LSByte * 2 ^ MSByte) + 1
    return  (uint16)((reg_val & 0x00FF) <<
            (uint16)((reg_val & 0xFF00) >> 8)) + 1;
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     对超时数值进行编码
// 参数说明     timeout_mclks   超时时长 -MCLKs 值
// 返回参数     uint16          返回编码值
// 使用示例     dl1a_encode_timeout(timeout_mclks);
// 备注信息     在 MCLK 中对超时的序列步骤超时寄存器值进行编码
//-------------------------------------------------------------------------------------------------------------------
static uint16 dl1a_encode_timeout (uint16 timeout_mclks)
{
    uint32 ls_byte = 0;
    uint16 ms_byte = 0;
    uint16 return_data = 0;

    if (timeout_mclks > 0)
    {
        // 格式: (LSByte * 2 ^ MSByte) + 1
        ls_byte = timeout_mclks - 1;
        while ((ls_byte & 0xFFFFFF00) > 0)
        {
            ls_byte >>= 1;
            ms_byte++;
        }
        return_data = (ms_byte << 8) | ((uint16)ls_byte & 0xFF);
    }
    return return_data;
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     获取序列步骤使能设置
// 参数说明     enables         序列使能步骤结构体
// 返回参数     void
// 使用示例     dl1a_get_sequence_step_enables(enables);
// 备注信息
//-------------------------------------------------------------------------------------------------------------------
static void dl1a_get_sequence_step_enables(dl1a_sequence_enables_step_struct *enables)
{
    uint8 sequence_config = 0;
    dl1a_read_registers(DL1A_SYSTEM_SEQUENCE_CONFIG, &sequence_config, 1);

    enables->tcc          = (sequence_config >> 4) & 0x1;
    enables->dss          = (sequence_config >> 3) & 0x1;
    enables->msrc         = (sequence_config >> 2) & 0x1;
    enables->pre_range    = (sequence_config >> 6) & 0x1;
    enables->final_range  = (sequence_config >> 7) & 0x1;
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     获取脉冲周期
// 参数说明     type            预量程类型
// 返回参数     uint8           返回的周期值
// 使用示例     dl1a_get_vcsel_pulse_period(DL1A_VCSEL_PERIOD_PER_RANGE);
// 备注信息     在 PCLKs 中获取给定周期类型的 VCSEL 脉冲周期
//-------------------------------------------------------------------------------------------------------------------
static uint8 dl1a_get_vcsel_pulse_period (dl1a_vcsel_period_type_enum type)
{
    uint8 data_buffer = 0;
    if (type == DL1A_VCSEL_PERIOD_PER_RANGE)
    {
        dl1a_read_registers(DL1A_PRE_RANGE_CONFIG_VCSEL_PERIOD, &data_buffer, 1);
        data_buffer = decode_vcsel_period(data_buffer);
    }
    else if (type == DL1A_VCSEL_PERIOD_FINAL_RANGE)
    {
        dl1a_read_registers(DL1A_FINAL_RANGE_CONFIG_VCSEL_PERIOD, &data_buffer, 1);
        data_buffer = decode_vcsel_period(data_buffer);
    }
    else
    {
        data_buffer = 255;
    }
    return data_buffer;
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     获取序列步骤超时设置
// 参数说明     enables         序列使能步骤结构体
// 参数说明     timeouts        序列超时步骤结构体
// 返回参数     void
// 使用示例     dl1a_get_sequence_step_timeouts(enables, timeouts);
// 备注信息     获取所有超时而不仅仅是请求的超时 并且还存储中间值
//-------------------------------------------------------------------------------------------------------------------
static void dl1a_get_sequence_step_timeouts (dl1a_sequence_enables_step_struct const *enables, dl1a_sequence_timeout_step_struct *timeouts)
{
    uint8 reg_buffer[2];
    uint16 reg16_buffer = 0;

    timeouts->pre_range_vcsel_period_pclks = dl1a_get_vcsel_pulse_period(DL1A_VCSEL_PERIOD_PER_RANGE);

    dl1a_read_registers(DL1A_MSRC_CONFIG_TIMEOUT_MACROP, reg_buffer, 1);
    timeouts->msrc_dss_tcc_mclks = reg_buffer[0] + 1;
    timeouts->msrc_dss_tcc_us = dl1a_timeout_mclks_to_microseconds(timeouts->msrc_dss_tcc_mclks, (uint8)timeouts->pre_range_vcsel_period_pclks);

    dl1a_read_registers(DL1A_PRE_RANGE_CONFIG_TIMEOUT_MACROP_HI, reg_buffer, 2);
    reg16_buffer = ((uint16) reg_buffer[0] << 8) | reg_buffer[1];
    timeouts->pre_range_mclks = dl1a_decode_timeout(reg16_buffer);
    timeouts->pre_range_us = dl1a_timeout_mclks_to_microseconds(timeouts->pre_range_mclks, (uint8)timeouts->pre_range_vcsel_period_pclks);

    timeouts->final_range_vcsel_period_pclks = dl1a_get_vcsel_pulse_period(DL1A_VCSEL_PERIOD_FINAL_RANGE);

    dl1a_read_registers(DL1A_FINAL_RANGE_CONFIG_TIMEOUT_MACROP_HI, reg_buffer, 2);
    reg16_buffer = ((uint16) reg_buffer[0] << 8) | reg_buffer[1];
    timeouts->final_range_mclks = dl1a_decode_timeout(reg16_buffer);

    if (enables->pre_range)
    {
        timeouts->final_range_mclks -= timeouts->pre_range_mclks;
    }

    timeouts->final_range_us = dl1a_timeout_mclks_to_microseconds(timeouts->final_range_mclks, (uint8)timeouts->final_range_vcsel_period_pclks);
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     执行单次参考校准
// 参数说明     vhv_init_byte   预设校准值
// 返回参数     uint8           操作是否成功 0-成功 1-失败
// 使用示例     dl1a_get_vcsel_pulse_period(DL1A_VCSEL_PERIOD_PER_RANGE);
// 备注信息     在 PCLKs 中获取给定周期类型的 VCSEL 脉冲周期
//-------------------------------------------------------------------------------------------------------------------
static uint8 dl1a_perform_single_ref_calibration (uint8 vhv_init_byte)
{
    uint8 return_state = 0;
    uint8 data_buffer = 0;
    volatile uint16 loop_count = 0;
    do
    {
        dl1a_write_register(DL1A_SYSRANGE_START, 0x01 | vhv_init_byte);
        dl1a_read_registers(DL1A_MSRC_CONFIG_TIMEOUT_MACROP, &data_buffer, 1);
        while ((data_buffer & 0x07) == 0)
        {
            system_delay_ms(1);
            dl1a_read_registers(DL1A_MSRC_CONFIG_TIMEOUT_MACROP, &data_buffer, 1);
            if (loop_count ++ > DL1A_TIMEOUT_COUNT)
            {
                return_state = 1;
                break;
            }
        }
        if(return_state)
        {
            break;
        }
        dl1a_write_register(DL1A_SYSTEM_INTERRUPT_CLEAR, 0x01);
        dl1a_write_register(DL1A_SYSRANGE_START, 0x00);
    }while(0);

    return return_state;
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     设置测量定时预算 (以微秒为单位)
// 参数说明     budget_us       设定的测量允许的时间
// 返回参数     uint8           操作结果 0-成功 1-失败
// 使用示例     dl1a_set_measurement_timing_budget(measurement_timing_budget_us);
// 备注信息     这是一次测量允许的时间
//              即在测距序列的子步骤之间分配时间预算
//              更长的时间预算允许更精确的测量
//              增加一个N倍的预算可以减少一个sqrt(N)倍的范围测量标准偏差
//              默认为33毫秒 最小值为20 ms
//-------------------------------------------------------------------------------------------------------------------
static uint8 dl1a_set_measurement_timing_budget (uint32 budget_us)
{
    uint8 return_state = 0;
    uint8 data_buffer[3];
    uint16 data = 0;

    dl1a_sequence_enables_step_struct enables;
    dl1a_sequence_timeout_step_struct timeouts;

    do
    {
        if (budget_us < DL1A_MIN_TIMING_BUDGET)
        {
            return_state = 1;
            break;
        }

        uint32 used_budget_us = DL1A_SET_START_OVERHEAD + DL1A_END_OVERHEAD;
        dl1a_get_sequence_step_enables(&enables);
        dl1a_get_sequence_step_timeouts(&enables, &timeouts);

        if (enables.tcc)
        {
            used_budget_us += (timeouts.msrc_dss_tcc_us + DL1A_TCC_OVERHEAD);
        }

        if (enables.dss)
        {
            used_budget_us += 2 * (timeouts.msrc_dss_tcc_us + DL1A_DSS_OVERHEAD);
        }
        else if (enables.msrc)
        {
            used_budget_us += (timeouts.msrc_dss_tcc_us + DL1A_MSRC_OVERHEAD);
        }

        if (enables.pre_range)
        {
            used_budget_us += (timeouts.pre_range_us + DL1A_PRERANGE_OVERHEAD);
        }

        if (enables.final_range)
        {
            // 请注意 最终范围超时由计时预算和序列中所有其他超时的总和决定
            // 如果没有空间用于最终范围超时 则将设置错误
            // 否则 剩余时间将应用于最终范围
            used_budget_us += DL1A_FINALlRANGE_OVERHEAD;
            if (used_budget_us > budget_us)
            {
                // 请求的超时太大
                return_state = 1;
                break;
            }

            // 对于最终超时范围 必须添加预量程范围超时
            // 为此 最终超时和预量程超时必须以宏周期 MClks 表示
            // 因为它们具有不同的 VCSEL 周期
            uint32 final_range_timeout_us = budget_us - used_budget_us;
            uint16 final_range_timeout_mclks =
            (uint16)dl1a_timeout_microseconds_to_mclks(final_range_timeout_us,
                     (uint8)timeouts.final_range_vcsel_period_pclks);

            if (enables.pre_range)
            {
                final_range_timeout_mclks += timeouts.pre_range_mclks;
            }

            data = dl1a_encode_timeout(final_range_timeout_mclks);
            data_buffer[0] = DL1A_FINAL_RANGE_CONFIG_TIMEOUT_MACROP_HI;
            data_buffer[1] = ((data >> 8) & 0xFF);
            data_buffer[2] = (data & 0xFF);
            dl1a_write_array(data_buffer, 3);
        }
    }while(0);
    return return_state;
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     获取测量定时预算 (以微秒为单位)
// 参数说明     void
// 返回参数     uint32          已设定的测量允许的时间
// 使用示例     dl1a_get_measurement_timing_budget();
// 备注信息
//-------------------------------------------------------------------------------------------------------------------
static uint32 dl1a_get_measurement_timing_budget (void)
{
    dl1a_sequence_enables_step_struct enables;
    dl1a_sequence_timeout_step_struct timeouts;

    // 开始和结束开销时间始终存在
    uint32 budget_us = DL1A_GET_START_OVERHEAD + DL1A_END_OVERHEAD;

    dl1a_get_sequence_step_enables(&enables);
    dl1a_get_sequence_step_timeouts(&enables, &timeouts);

    if (enables.tcc)
    {
        budget_us += (timeouts.msrc_dss_tcc_us + DL1A_TCC_OVERHEAD);
    }

    if (enables.dss)
    {
        budget_us += 2 * (timeouts.msrc_dss_tcc_us + DL1A_DSS_OVERHEAD);
    }
    else if (enables.msrc)
    {
        budget_us += (timeouts.msrc_dss_tcc_us + DL1A_MSRC_OVERHEAD);
    }

    if (enables.pre_range)
    {
        budget_us += (timeouts.pre_range_us + DL1A_PRERANGE_OVERHEAD);
    }

    if (enables.final_range)
    {
        budget_us += (timeouts.final_range_us + DL1A_FINALlRANGE_OVERHEAD);
    }

    return budget_us;
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     设置返回信号速率限制 该值单位为 MCPS (百万次每秒)
// 参数说明     limit_mcps      设置的最小速率
// 返回参数     void
// 使用示例     dl1a_set_signal_rate_limit(0.25);
// 备注信息     这个速率表示从目标反射并被设备检测到的信号的振幅
//              设置此限制可以确定传感器报告有效读数所需的最小测量值
//              设置一个较低的限制可以增加传感器的测量范围
//              但似乎也增加了 <由于来自目标以外的物体的不需要的反射导致> 得到不准确读数的可能性
//              默认为 0.25 MCPS 可预设范围为 0 - 511.99
//-------------------------------------------------------------------------------------------------------------------
static void dl1a_set_signal_rate_limit (float limit_mcps)
{
    zf_assert(limit_mcps >= 0 || limit_mcps <= 511.99);
    uint8 data_buffer[3];
    uint16 limit_mcps_16bit = (limit_mcps * (1 << 7));

    data_buffer[0] = DL1A_FINAL_RANGE_CONFIG_MIN_COUNT_RATE_RTN_LIMIT;
    data_buffer[1] = ((limit_mcps_16bit >> 8) & 0xFF);
    data_buffer[2] = (limit_mcps_16bit & 0xFF);

    dl1a_write_array(data_buffer, 3);
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     返回以毫米为单位的范围读数
// 参数说明     void
// 返回参数     void
// 使用示例     dl1a_get_distance();
// 备注信息     在开始单次射程测量后也调用此函数
//-------------------------------------------------------------------------------------------------------------------
void dl1a_get_distance (void)
{
    uint8 reg_databuffer[3];

    dl1a_read_registers(DL1A_RESULT_INTERRUPT_STATUS, reg_databuffer, 1);
    if((reg_databuffer[0] & 0x07) != 0)
    {
        // 假设线性度校正增益为默认值 1000 且未启用分数范围
        dl1a_read_registers(DL1A_RESULT_RANGE_STATUS + 10, reg_databuffer, 2);
        dl1a_distance_mm = ((uint16_t)reg_databuffer[0] << 8);
        dl1a_distance_mm |= reg_databuffer[1];

        dl1a_write_register(DL1A_SYSTEM_INTERRUPT_CLEAR, 0x01);
        dl1a_finsh_flag = 1;
    }
    if(reg_databuffer[0] & 0x10)
    {
        dl1a_read_registers(DL1A_RESULT_RANGE_STATUS + 10, reg_databuffer, 2);
        dl1a_write_register(DL1A_SYSTEM_INTERRUPT_CLEAR, 0x01);
    }
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     初始化 DL1A
// 参数说明     void
// 返回参数     uint8           1-初始化失败 0-初始化成功
// 使用示例     dl1a_init();
// 备注信息
//-------------------------------------------------------------------------------------------------------------------
uint8 dl1a_init (void)
{
    uint32 measurement_timing_budget_us;
    uint8 stop_variable;
    uint8 return_state = 0;
    uint8 reg_data_buffer ;
    uint8 ref_spad_map[6];
    uint8 data_buffer[7];

    memset(ref_spad_map, 0, 6);
    memset(data_buffer, 0, 7);

#if DL1A_USE_SOFT_IIC
    soft_iic_init(&dl1a_iic_struct, DL1A_DEV_ADDR, DL1A_SOFT_IIC_DELAY, DL1A_SCL_PIN, DL1A_SDA_PIN);
#else
    iic_init(DL1A_IIC, DL1A_DEV_ADDR, DL1A_IIC_SPEED, DL1A_SCL_PIN, DL1A_SDA_PIN);
#endif
    gpio_init(DL1A_XSHUT_PIN, GPO, GPIO_HIGH, GPO_PUSH_PULL);

    do
    {
        system_delay_ms(100);
        gpio_low(DL1A_XSHUT_PIN);
        system_delay_ms(50);
        gpio_high(DL1A_XSHUT_PIN);
        system_delay_ms(100);

        // -------------------------------- DL1A 启动初始化 --------------------------------
        reg_data_buffer = dl1a_read_register(DL1A_IO_VOLTAGE_CONFIG);         // 传感器默认 IO 为 1.8V 模式
        dl1a_write_register(DL1A_IO_VOLTAGE_CONFIG, reg_data_buffer | 0x01);  // 配置 IO 为 2.8V 模式

        dl1a_write_register(0x88, 0x00);                                         // 设置为标准 IIC 模式

        dl1a_write_register(0x80, 0x01);
        dl1a_write_register(0xFF, 0x01);
        dl1a_write_register(0x00, 0x00);

        dl1a_read_registers(0x91, &stop_variable , 1);

        dl1a_write_register(0x00, 0x01);
        dl1a_write_register(0xFF, 0x00);
        dl1a_write_register(0x80, 0x00);

        // 禁用 SIGNAL_RATE_MSRC(bit1) 和 SIGNAL_RATE_PRE_RANGE(bit4) 限制检查
        reg_data_buffer = dl1a_read_register(DL1A_MSRC_CONFIG);
        dl1a_write_register(DL1A_MSRC_CONFIG, reg_data_buffer | 0x12);

        dl1a_set_signal_rate_limit(DL1A_DEFAULT_RATE_LIMIT);                  // 设置信号速率限制
        dl1a_write_register(DL1A_SYSTEM_SEQUENCE_CONFIG, 0xFF);
        // -------------------------------- DL1A 启动初始化 --------------------------------

        // -------------------------------- DL1A 配置初始化 --------------------------------
        if (dl1a_get_spad_info(&data_buffer[0], &data_buffer[1]))
        {
            return_state = 1;
            zf_log(0, "DL1A self check error.");
            break;
        }

        // 从 GLOBAL_CONFIG_SPAD_ENABLES_REF_[0-6] 获取 SPAD map (RefGoodSpadMap) 数据
        dl1a_read_registers(DL1A_GLOBAL_CONFIG_SPAD_ENABLES_REF_0, ref_spad_map, 6);

        dl1a_write_register(0xFF, 0x01);
        dl1a_write_register(DL1A_DYNAMIC_SPAD_REF_EN_START_OFFSET, 0x00);
        dl1a_write_register(DL1A_DYNAMIC_SPAD_NUM_REQUESTED_REF_SPAD, 0x2C);
        dl1a_write_register(0xFF, 0x00);
        dl1a_write_register(DL1A_GLOBAL_CONFIG_REF_EN_START_SELECT, 0xB4);

        data_buffer[2] = data_buffer[1] ? 12 : 0; // 12 is the first aperture spad
        for (uint8 i = 0; i < 48; i++)
        {
            if (i < data_buffer[2] || data_buffer[3] == data_buffer[0])
            {
                // 此位低于应启用的第一个位
                // 或者 (eference_spad_count) 位已启用
                // 因此此位为零
                ref_spad_map[i / 8] &= ~(1 << (i % 8));
            }
            else if ((ref_spad_map[i / 8] >> (i % 8)) & 0x1)
            {
                data_buffer[3]++;
            }
        }

        data_buffer[0] = DL1A_GLOBAL_CONFIG_SPAD_ENABLES_REF_0;
        for(uint8 i = 1; i < 7; i++)
        {
            data_buffer[1] = ref_spad_map[i-1];
        }
        dl1a_write_array(data_buffer, 7);

        // 默认转换设置 version 02/11/2015_v36
        dl1a_write_register(0xFF, 0x01);
        dl1a_write_register(0x00, 0x00);
        dl1a_write_register(0xFF, 0x00);
        dl1a_write_register(0x09, 0x00);
        dl1a_write_register(0x10, 0x00);
        dl1a_write_register(0x11, 0x00);
        dl1a_write_register(0x24, 0x01);
        dl1a_write_register(0x25, 0xFF);
        dl1a_write_register(0x75, 0x00);
        dl1a_write_register(0xFF, 0x01);
        dl1a_write_register(0x4E, 0x2C);
        dl1a_write_register(0x48, 0x00);
        dl1a_write_register(0x30, 0x20);
        dl1a_write_register(0xFF, 0x00);
        dl1a_write_register(0x30, 0x09);
        dl1a_write_register(0x54, 0x00);
        dl1a_write_register(0x31, 0x04);
        dl1a_write_register(0x32, 0x03);
        dl1a_write_register(0x40, 0x83);
        dl1a_write_register(0x46, 0x25);
        dl1a_write_register(0x60, 0x00);
        dl1a_write_register(0x27, 0x00);
        dl1a_write_register(0x50, 0x06);
        dl1a_write_register(0x51, 0x00);
        dl1a_write_register(0x52, 0x96);
        dl1a_write_register(0x56, 0x08);
        dl1a_write_register(0x57, 0x30);
        dl1a_write_register(0x61, 0x00);
        dl1a_write_register(0x62, 0x00);
        dl1a_write_register(0x64, 0x00);
        dl1a_write_register(0x65, 0x00);
        dl1a_write_register(0x66, 0xA0);
        dl1a_write_register(0xFF, 0x01);
        dl1a_write_register(0x22, 0x32);
        dl1a_write_register(0x47, 0x14);
        dl1a_write_register(0x49, 0xFF);
        dl1a_write_register(0x4A, 0x00);
        dl1a_write_register(0xFF, 0x00);
        dl1a_write_register(0x7A, 0x0A);
        dl1a_write_register(0x7B, 0x00);
        dl1a_write_register(0x78, 0x21);
        dl1a_write_register(0xFF, 0x01);
        dl1a_write_register(0x23, 0x34);
        dl1a_write_register(0x42, 0x00);
        dl1a_write_register(0x44, 0xFF);
        dl1a_write_register(0x45, 0x26);
        dl1a_write_register(0x46, 0x05);
        dl1a_write_register(0x40, 0x40);
        dl1a_write_register(0x0E, 0x06);
        dl1a_write_register(0x20, 0x1A);
        dl1a_write_register(0x43, 0x40);
        dl1a_write_register(0xFF, 0x00);
        dl1a_write_register(0x34, 0x03);
        dl1a_write_register(0x35, 0x44);
        dl1a_write_register(0xFF, 0x01);
        dl1a_write_register(0x31, 0x04);
        dl1a_write_register(0x4B, 0x09);
        dl1a_write_register(0x4C, 0x05);
        dl1a_write_register(0x4D, 0x04);
        dl1a_write_register(0xFF, 0x00);
        dl1a_write_register(0x44, 0x00);
        dl1a_write_register(0x45, 0x20);
        dl1a_write_register(0x47, 0x08);
        dl1a_write_register(0x48, 0x28);
        dl1a_write_register(0x67, 0x00);
        dl1a_write_register(0x70, 0x04);
        dl1a_write_register(0x71, 0x01);
        dl1a_write_register(0x72, 0xFE);
        dl1a_write_register(0x76, 0x00);
        dl1a_write_register(0x77, 0x00);
        dl1a_write_register(0xFF, 0x01);
        dl1a_write_register(0x0D, 0x01);
        dl1a_write_register(0xFF, 0x00);
        dl1a_write_register(0x80, 0x01);
        dl1a_write_register(0x01, 0xF8);
        dl1a_write_register(0xFF, 0x01);
        dl1a_write_register(0x8E, 0x01);
        dl1a_write_register(0x00, 0x01);
        dl1a_write_register(0xFF, 0x00);
        dl1a_write_register(0x80, 0x00);

        // 将中断配置设置为新样品就绪
        dl1a_write_register(DL1A_SYSTEM_INTERRUPT_GPIO_CONFIG, 0x04);
        reg_data_buffer = dl1a_read_register(DL1A_GPIO_HV_MUX_ACTIVE_HIGH);
        dl1a_write_register(DL1A_GPIO_HV_MUX_ACTIVE_HIGH, reg_data_buffer & ~0x10);
        dl1a_write_register(DL1A_SYSTEM_INTERRUPT_CLEAR, 0x01);

        measurement_timing_budget_us  = dl1a_get_measurement_timing_budget();

        // 默认情况下禁用 MSRC 和 TCC
        // MSRC = Minimum Signal Rate Check
        // TCC = Target CentreCheck
        dl1a_write_register(DL1A_SYSTEM_SEQUENCE_CONFIG, 0xE8);
        dl1a_set_measurement_timing_budget(measurement_timing_budget_us);    // 重新计算时序预算
        // -------------------------------- DL1A 配置初始化 --------------------------------

        dl1a_write_register(DL1A_SYSTEM_SEQUENCE_CONFIG, 0x01);
        if (dl1a_perform_single_ref_calibration(0x40))
        {
            return_state = 1;
            zf_log(0, "DL1A perform single reference calibration error.");
            break;
        }
        dl1a_write_register(DL1A_SYSTEM_SEQUENCE_CONFIG, 0x02);
        if (dl1a_perform_single_ref_calibration(0x00))
        {
            return_state = 1;
            zf_log(0, "DL1A perform single reference calibration error.");
            break;
        }
        dl1a_write_register(DL1A_SYSTEM_SEQUENCE_CONFIG, 0xE8);           // 恢复以前的序列配置

        system_delay_ms(100);

        dl1a_write_register(0x80, 0x01);
        dl1a_write_register(0xFF, 0x01);
        dl1a_write_register(0x00, 0x00);
        dl1a_write_register(0x91, stop_variable);
        dl1a_write_register(0x00, 0x01);
        dl1a_write_register(0xFF, 0x00);
        dl1a_write_register(0x80, 0x00);

        dl1a_write_register(DL1A_SYSRANGE_START, 0x02);
    }while(0);

    return return_state;
}
