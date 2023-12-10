/*********************************************************************************************************************
* TC264 Opensourec Library ����TC264 ��Դ�⣩��һ�����ڹٷ� SDK �ӿڵĵ�������Դ��
* Copyright (c) 2022 SEEKFREE ��ɿƼ�
*
* ���ļ��� TC264 ��Դ���һ����
*
* TC264 ��Դ�� ��������
* �����Ը��������������ᷢ���� GPL��GNU General Public License���� GNUͨ�ù������֤��������
* �� GPL �ĵ�3�棨�� GPL3.0������ѡ��ģ��κκ����İ汾�����·�����/���޸���
*
* ����Դ��ķ�����ϣ�����ܷ������ã�����δ�������κεı�֤
* ����û�������������Ի��ʺ��ض���;�ı�֤
* ����ϸ����μ� GPL
*
* ��Ӧ�����յ�����Դ���ͬʱ�յ�һ�� GPL �ĸ���
* ���û�У������<https://www.gnu.org/licenses/>
*
* ����ע����
* ����Դ��ʹ�� GPL3.0 ��Դ���֤Э�� �����������Ϊ���İ汾
* �������Ӣ�İ��� libraries/doc �ļ����µ� GPL3_permission_statement.txt �ļ���
* ���֤������ libraries �ļ����� �����ļ����µ� LICENSE �ļ�
* ��ӭ��λʹ�ò����������� ���޸�����ʱ���뱣����ɿƼ��İ�Ȩ����������������
*
* �ļ�����          zf_device_dl1a
* ��˾����          �ɶ���ɿƼ����޹�˾
* �汾��Ϣ          �鿴 libraries/doc �ļ����� version �ļ� �汾˵��
* ��������          ADS v1.8.0
* ����ƽ̨          TC264D
* ��������          https://seekfree.taobao.com/
*
* �޸ļ�¼
* ����              ����                ��ע
* 2022-09-15       pudding            first version
********************************************************************************************************************/
/*********************************************************************************************************************
* ���߶��壺
*                   ------------------------------------
*                   ģ��ܽ�            ��Ƭ���ܽ�
*                   SCL                 �鿴 zf_device_dl1a.h �� DL1A_SCL_PIN  �궨��
*                   SDA                 �鿴 zf_device_dl1a.h �� DL1A_SDA_PIN  �궨��
*                   VCC                 5V ��Դ
*                   GND                 ��Դ��
*                   ------------------------------------
*                   ------------------------------------
********************************************************************************************************************/

#ifndef _ZF_DEVICE_DL1A_H_
#define _ZF_DEVICE_DL1A_H_

#include "zf_common_typedef.h"


// ��Ҫע����� DL1A ���֧�� 400KHz �� IIC ͨ������
// ��Ҫע����� DL1A ���֧�� 400KHz �� IIC ͨ������
// ��Ҫע����� DL1A ���֧�� 400KHz �� IIC ͨ������

#define DL1A_USE_SOFT_IIC        (1)                                         // Ĭ��ʹ����� IIC ��ʽ���� ����ʹ����� IIC ��ʽ
#if DL1A_USE_SOFT_IIC                                                        // ������ ��ɫ�����Ĳ�����ȷ�� ��ɫ�ҵľ���û���õ�
//====================================================��� IIC ����====================================================
#define DL1A_SOFT_IIC_DELAY      (100)                                       // ��� IIC ��ʱ����ʱ���� ��ֵԽС IIC ͨ������Խ��
#define DL1A_SCL_PIN             (P33_13)                                     // ��� IIC SCL ���� ���� VL53L0X �� SCL ����
#define DL1A_SDA_PIN             (P32_4)                                     // ��� IIC SDA ���� ���� VL53L0X �� SDA ����
//====================================================��� IIC ����====================================================
#else
#error "�ݲ�֧��Ӳ��IICͨѶ"
#endif

#define DL1A_XSHUT_PIN           (P33_11)
#define DL1A_TIMEOUT_COUNT       (0x00FF)                                    // VL53L0X ��ʱ����

//================================================���� DL1A �ڲ���ַ================================================
#define DL1A_DEV_ADDR                                       (0x52 >> 1)         // 0b0101001

#define DL1A_SYSRANGE_START                                 (0x00)

#define DL1A_SYSTEM_SEQUENCE_CONFIG                         (0x01)
#define DL1A_SYSTEM_INTERMEASUREMENT_PERIOD                 (0x04)
#define DL1A_SYSTEM_RANGE_CONFIG                            (0x09)
#define DL1A_SYSTEM_INTERRUPT_GPIO_CONFIG                   (0x0A)
#define DL1A_SYSTEM_INTERRUPT_CLEAR                         (0x0B)
#define DL1A_SYSTEM_THRESH_HIGH                             (0x0C)
#define DL1A_SYSTEM_THRESH_LOW                              (0x0E)
#define DL1A_SYSTEM_HISTOGRAM_BIN                           (0x81)

#define DL1A_RESULT_INTERRUPT_STATUS                        (0x13)
#define DL1A_RESULT_RANGE_STATUS                            (0x14)
#define DL1A_RESULT_PEAK_SIGNAL_RATE_REF                    (0xB6)
#define DL1A_RESULT_CORE_AMBIENT_WINDOW_EVENTS_RTN          (0xBC)
#define DL1A_RESULT_CORE_RANGING_TOTAL_EVENTS_RTN           (0xC0)
#define DL1A_RESULT_CORE_AMBIENT_WINDOW_EVENTS_REF          (0xD0)
#define DL1A_RESULT_CORE_RANGING_TOTAL_EVENTS_REF           (0xD4)

#define DL1A_PRE_RANGE_CONFIG_MIN_SNR                       (0x27)
#define DL1A_PRE_RANGE_CONFIG_VCSEL_PERIOD                  (0x50)
#define DL1A_PRE_RANGE_CONFIG_TIMEOUT_MACROP_HI             (0x51)
#define DL1A_PRE_RANGE_CONFIG_TIMEOUT_MACROP_LO             (0x52)
#define DL1A_PRE_RANGE_CONFIG_VALID_PHASE_LOW               (0x56)
#define DL1A_PRE_RANGE_CONFIG_VALID_PHASE_HIGH              (0x57)
#define DL1A_PRE_RANGE_CONFIG_SIGMA_THRESH_HI               (0x61)
#define DL1A_PRE_RANGE_CONFIG_SIGMA_THRESH_LO               (0x62)
#define DL1A_PRE_RANGE_MIN_COUNT_RATE_RTN_LIMIT             (0x64)

#define DL1A_FINAL_RANGE_CONFIG_VALID_PHASE_LOW             (0x47)
#define DL1A_FINAL_RANGE_CONFIG_VALID_PHASE_HIGH            (0x48)
#define DL1A_FINAL_RANGE_CONFIG_MIN_COUNT_RATE_RTN_LIMIT    (0x44)
#define DL1A_FINAL_RANGE_CONFIG_MIN_SNR                     (0x67)
#define DL1A_FINAL_RANGE_CONFIG_VCSEL_PERIOD                (0x70)
#define DL1A_FINAL_RANGE_CONFIG_TIMEOUT_MACROP_HI           (0x71)
#define DL1A_FINAL_RANGE_CONFIG_TIMEOUT_MACROP_LO           (0x72)

#define DL1A_GLOBAL_CONFIG_VCSEL_WIDTH                      (0x32)
#define DL1A_GLOBAL_CONFIG_SPAD_ENABLES_REF_0               (0xB0)
#define DL1A_GLOBAL_CONFIG_SPAD_ENABLES_REF_1               (0xB1)
#define DL1A_GLOBAL_CONFIG_SPAD_ENABLES_REF_2               (0xB2)
#define DL1A_GLOBAL_CONFIG_SPAD_ENABLES_REF_3               (0xB3)
#define DL1A_GLOBAL_CONFIG_SPAD_ENABLES_REF_4               (0xB4)
#define DL1A_GLOBAL_CONFIG_SPAD_ENABLES_REF_5               (0xB5)
#define DL1A_GLOBAL_CONFIG_REF_EN_START_SELECT              (0xB6)

#define DL1A_ALGO_PART_TO_PART_RANGE_OFFSET_MM              (0x28)
#define DL1A_ALGO_PHASECAL_LIM                              (0x30)
#define DL1A_ALGO_PHASECAL_CONFIG_TIMEOUT                   (0x30)

#define DL1A_HISTOGRAM_CONFIG_INITIAL_PHASE_SELECT          (0x33)
#define DL1A_HISTOGRAM_CONFIG_READOUT_CTRL                  (0x55)

#define DL1A_DYNAMIC_SPAD_NUM_REQUESTED_REF_SPAD            (0x4E)
#define DL1A_DYNAMIC_SPAD_REF_EN_START_OFFSET               (0x4F)

#define DL1A_MSRC_CONFIG_TIMEOUT_MACROP                     (0x46)
#define DL1A_MSRC_CONFIG                                    (0x60)

#define DL1A_IDENTIFICATION_MODEL_ID                        (0xC0)
#define DL1A_IDENTIFICATION_REVISION_ID                     (0xC2)

#define DL1A_CROSSTALK_COMPENSATION_PEAK_RATE_MCPS          (0x20)

#define DL1A_POWER_MANAGEMENT_GO1_POWER_FORCE               (0x80)

#define DL1A_GPIO_HV_MUX_ACTIVE_HIGH                        (0x84)

#define DL1A_I2C_SLAVE_DEVICE_ADDRESS                       (0x8A)

#define DL1A_SOFT_RESET_GO2_SOFT_RESET_N                    (0xBF)

#define DL1A_OSC_CALIBRATE_VAL                              (0xF8)

#define DL1A_IO_VOLTAGE_CONFIG                              (0x89)              // IO ��ѹ���üĴ�����ַ Ĭ�� 1V8 ʹ���޸�Ϊ 2V8

//================================================���� DL1A �ڲ���ַ================================================

#define DL1A_MIN_TIMING_BUDGET                              (20000)

#define DL1A_GET_START_OVERHEAD                             (1910)
#define DL1A_SET_START_OVERHEAD                             (1320)
#define DL1A_END_OVERHEAD                                   (960 )
#define DL1A_TCC_OVERHEAD                                   (590 )
#define DL1A_DSS_OVERHEAD                                   (690 )
#define DL1A_MSRC_OVERHEAD                                  (660 )
#define DL1A_PRERANGE_OVERHEAD                              (660 )
#define DL1A_FINALlRANGE_OVERHEAD                           (550 )

typedef enum
{
    DL1A_VCSEL_PERIOD_PER_RANGE,
    DL1A_VCSEL_PERIOD_FINAL_RANGE,
}dl1a_vcsel_period_type_enum;

typedef struct
{
    uint8 tcc;
    uint8 msrc;
    uint8 dss;
    uint8 pre_range;
    uint8 final_range;
}dl1a_sequence_enables_step_struct;

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
}dl1a_sequence_timeout_step_struct;

extern uint8 dl1a_finsh_flag;
extern uint16 dl1a_distance_mm;

void   dl1a_get_distance (void);

uint8  dl1a_init         (void);

#endif

