/*********************************************************************************************************************
* TC264 Opensourec Library ����TC264 ��Դ�⣩��һ�����ڹٷ� SDK �ӿڵĵ�������Դ��
* Copyright (c) 2022 SEEKFREE ��ɿƼ�
*
* ���ļ��� TC264 ��Դ���һ����
*
* TC264 ��Դ�� ���������
* �����Ը���������������ᷢ���� GPL��GNU General Public License���� GNUͨ�ù�������֤��������
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
* ����Դ��ʹ�� GPL3.0 ��Դ����֤Э�� ������������Ϊ���İ汾
* ��������Ӣ�İ��� libraries/doc �ļ����µ� GPL3_permission_statement.txt �ļ���
* ����֤������ libraries �ļ����� �����ļ����µ� LICENSE �ļ�
* ��ӭ��λʹ�ò����������� ���޸�����ʱ���뱣����ɿƼ��İ�Ȩ����������������
*
* �ļ�����          zf_device_imu963ra
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
/********************************************************************************************************************
* ���߶��壺
*                  ------------------------------------
*                  ģ��ܽ�             ��Ƭ���ܽ�
*                  // Ӳ�� SPI ����
*                  SCL/SPC            �鿴 zf_device_imu963ra.h �� IMU963RA_SPC_PIN �궨��
*                  SDA/DSI            �鿴 zf_device_imu963ra.h �� IMU963RA_SDI_PIN �궨��
*                  SA0/SDO            �鿴 zf_device_imu963ra.h �� IMU963RA_SDO_PIN �궨��
*                  CS                 �鿴 zf_device_imu963ra.h �� IMU963RA_CS_PIN  �궨��
*                  VCC                3.3V��Դ
*                  GND                ��Դ��
*                  ������������
*
*                  // ���� IIC ����
*                  SCL/SPC            �鿴 zf_device_imu963ra.h �� IMU963RA_SCL_PIN �궨��
*                  SDA/DSI            �鿴 zf_device_imu963ra.h �� IMU963RA_SDA_PIN �궨��
*                  VCC                3.3V��Դ
*                  GND                ��Դ��
*                  ������������
*                  ------------------------------------
********************************************************************************************************************/

#ifndef _zf_device_imu963ra_h_
#define _zf_device_imu963ra_h_

#include "zf_common_typedef.h"

#define IMU963RA_USE_SOFT_IIC                       (0)                         // Ĭ��ʹ��Ӳ�� SPI ��ʽ����
#if IMU963RA_USE_SOFT_IIC                                                       // ������ ��ɫ�����Ĳ�����ȷ�� ��ɫ�ҵľ���û���õ�
//====================================================���� IIC ����====================================================
#define IMU963RA_SOFT_IIC_DELAY                     (59 )                       // ���� IIC ��ʱ����ʱ���� ��ֵԽС IIC ͨ������Խ��
#define IMU963RA_SCL_PIN                            (P20_11)                    // ���� IIC SCL ���� ���� IMU963RA �� SCL ����
#define IMU963RA_SDA_PIN                            (P20_14)                    // ���� IIC SDA ���� ���� IMU963RA �� SDA ����
//====================================================���� IIC ����====================================================
#else
//====================================================Ӳ�� SPI ����====================================================
#define IMU963RA_SPI_SPEED                          (10 * 1000 * 1000)          // Ӳ�� SPI ����
#define IMU963RA_SPI                                (SPI_0           )          // Ӳ�� SPI ��
#define IMU963RA_SPC_PIN                            (SPI0_SCLK_P20_11)          // Ӳ�� SPI SCK ����
#define IMU963RA_SDI_PIN                            (SPI0_MOSI_P20_14)          // Ӳ�� SPI MOSI ����
#define IMU963RA_SDO_PIN                            (SPI0_MISO_P20_12)          // Ӳ�� SPI MISO ����
//====================================================Ӳ�� SPI ����====================================================
#endif

#define IMU963RA_CS_PIN                             (P20_13)                    // CS Ƭѡ����
#define IMU963RA_CS(x)                              (x? (gpio_high(IMU963RA_CS_PIN)): (gpio_low(IMU963RA_CS_PIN)))

#define IMU963RA_TIMEOUT_COUNT                      (0x00FF)                    // IMU963RA ��ʱ����

//================================================���� IMU963RA �ڲ���ַ================================================
#define IMU963RA_DEV_ADDR                           (0x6B)                      // SA0�ӵأ�0x6A SA0������0x6B ģ��Ĭ������
#define IMU963RA_SPI_W                              (0x00)
#define IMU963RA_SPI_R                              (0x80)

#define IMU963RA_FUNC_CFG_ACCESS                    (0x01)
#define IMU963RA_INT1_CTRL                          (0x0D)
#define IMU963RA_WHO_AM_I                           (0x0F)
#define IMU963RA_CTRL1_XL                           (0x10)
#define IMU963RA_ACC_SAMPLE                         (0x3C)                      // ���ٶȼ�����
#define IMU963RA_CTRL2_G                            (0x11)
#define IMU963RA_GYRO_SAMPLE                        (0x5C)                      // ����������
#define IMU963RA_CTRL3_C                            (0x12)
#define IMU963RA_CTRL4_C                            (0x13)
#define IMU963RA_CTRL5_C                            (0x14)
#define IMU963RA_CTRL6_C                            (0x15)
#define IMU963RA_CTRL7_G                            (0x16)
#define IMU963RA_CTRL9_XL                           (0x18)
#define IMU963RA_OUTX_L_G                           (0x22)
#define IMU963RA_OUTX_L_A                           (0x28)

//������������ؼĴ��� ��Ҫ��FUNC_CFG_ACCESS��SHUB_REG_ACCESSλ����Ϊ1������ȷ����
#define IMU963RA_SENSOR_HUB_1                       (0x02)
#define IMU963RA_MASTER_CONFIG                      (0x14)
#define IMU963RA_SLV0_ADD                           (0x15)
#define IMU963RA_SLV0_SUBADD                        (0x16)
#define IMU963RA_SLV0_CONFIG                        (0x17)
#define IMU963RA_DATAWRITE_SLV0                     (0x21)
#define IMU963RA_STATUS_MASTER                      (0x22)

#define IMU963RA_MAG_ADDR                           (0x0D)                      // 7λIIC��ַ
#define IMU963RA_MAG_OUTX_L                         (0x00)
#define IMU963RA_MAG_CONTROL1                       (0x09)
#define IMU963RA_MAG_SAMPLE                         (0x19)                      // �شż�����
#define IMU963RA_MAG_CONTROL2                       (0x0A)
#define IMU963RA_MAG_FBR                            (0x0B)
#define IMU963RA_MAG_CHIP_ID                        (0x0D)
//================================================���� IMU963RA �ڲ���ַ================================================

//===============================================���� IMU963RA ���ݴ洢����===============================================
extern int16 imu963ra_acc_x,  imu963ra_acc_y,  imu963ra_acc_z;          // ��������������
extern int16 imu963ra_gyro_x, imu963ra_gyro_y, imu963ra_gyro_z;         // ������ٶȼ�����
extern int16 imu963ra_mag_x,  imu963ra_mag_y,  imu963ra_mag_z;          // ����شż�����
//===============================================���� IMU963RA ���ݴ洢����===============================================

//==================================================IMU963RA ��������==================================================
void  imu963ra_get_acc              (void);                                 // ��ȡ IMU963RA ���ٶȼ�����
void  imu963ra_get_gyro             (void);                                 // ��ȡ IMU963RA ����������
void  imu963ra_get_mag              (void);                                 // ��ȡ IMU963RA ����������
float imu963ra_acc_transition       (int16 acc_value);                      // ��   IMU963RA ���ٶȼ�����ת��Ϊʵ����������
float imu963ra_gyro_transition      (int16 gyro_value);                     // ��   IMU963RA ����������ת��Ϊʵ����������
float imu963ra_mag_transition       (int16 mag_value);                      // ��   IMU963RA �شż�����ת��Ϊʵ����������
uint8 imu963ra_init                 (void);                                 // ��ʼ�� IMU963RA
//==================================================IMU963RA ��������==================================================


#endif