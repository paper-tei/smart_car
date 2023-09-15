/*********************************************************************************************************************
* TC387 Opensourec Library ����TC387 ��Դ�⣩��һ�����ڹٷ� SDK �ӿڵĵ�������Դ��
* Copyright (c) 2022 SEEKFREE ��ɿƼ�
*
* ���ļ��� TC387 ��Դ���һ����
*
* TC387 ��Դ�� ���������
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
* �ļ�����          main
* ��˾����          �ɶ���ɿƼ����޹�˾
* �汾��Ϣ          �鿴 libraries/doc �ļ����� version �ļ� �汾˵��
* ��������          ADS v1.8.0
* ����ƽ̨          TC387QP
* ��������          https://seekfree.taobao.com/
*
* �޸ļ�¼
* ����              ����                ��ע
* 2022-11-04       pudding             first version
********************************************************************************************************************/
/*********************************************************************************************************************
* ���߶��壺
*                 ------------------------------------
*                 ����ת����           ��Ƭ��
*                 RX                  �鿴zf_device_bluetooth_ch9141.h�ļ��е�BLUETOOTH_CH9141_TX_PIN�궨��
*                 TX                  �鿴zf_device_bluetooth_ch9141.h�ļ��е�BLUETOOTH_CH9141_RX_PIN�궨��
*                 RTS                 �鿴zf_device_bluetooth_ch9141.h�ļ��е�BLUETOOTH_CH9141_RTS_PIN�궨��
*                 CTS                 ����
*                 CMD                 ���ջ�������
*                 ------------------------------------
*********************************************************************************************************************/

#ifndef _zf_device_bluetooth_ch9141_h_
#define _zf_device_bluetooth_ch9141_h_

#include "zf_common_typedef.h"
#include "zf_device_type.h"


//=================================================9141���� ��������====================================================
#define BLUETOOTH_CH9141_INDEX              (UART_2)                        // ����ģ���Ӧʹ�õĴ��ں�
#define BLUETOOTH_CH9141_BUAD_RATE          (115200)                        // ����ģ���Ӧʹ�õĴ��ڲ�����
#define BLUETOOTH_CH9141_TX_PIN             (UART2_RX_P10_6)                // ����ģ���Ӧģ��� TX Ҫ�ӵ���Ƭ���� RX
#define BLUETOOTH_CH9141_RX_PIN             (UART2_TX_P10_5)                // ����ģ���Ӧģ��� RX Ҫ�ӵ���Ƭ���� TX
#define BLUETOOTH_CH9141_RTS_PIN            (P10_2)                         // ����ģ���Ӧģ��� RTS ����
//=================================================9141���� ��������====================================================

#define BLUETOOTH_CH9141_BUFFER_SIZE        (64)
#define BLUETOOTH_CH9141_TIMEOUT_COUNT      (500)

//=================================================9141���� ��������====================================================
uint32  bluetooth_ch9141_send_byte          (const uint8 data);
uint32  bluetooth_ch9141_send_buff          (const uint8 *buff, uint32 len);
uint32  bluetooth_ch9141_send_string        (const char *str);
void    bluetooth_ch9141_send_image         (const uint8 *image_addr, uint32 image_size);

uint32  bluetooth_ch9141_read_buff          (uint8 *buff, uint32 len);

void    bluetooth_ch9141_uart_callback      (void);

uint8   bluetooth_ch9141_init               (void);
//=================================================9141���� ��������====================================================

#endif