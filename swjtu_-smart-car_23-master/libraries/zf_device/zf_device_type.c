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
* �ļ�����          zf_device_type
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

#include "zf_device_type.h"

static void type_default_callback(void);

camera_type_enum    camera_type                     = NO_CAMERE;                            // ����ͷ���ͱ���
callback_function   camera_dma_handler              = type_default_callback;                // DMA����жϺ���ָ�룬���ݳ�ʼ��ʱ���õĺ���������ת
callback_function   camera_vsync_handler            = type_default_callback;                // ���жϺ���ָ�룬���ݳ�ʼ��ʱ���õĺ���������ת
callback_function   camera_uart_handler             = type_default_callback;                // ����ͨѶ�жϺ���ָ�룬���ݳ�ʼ��ʱ���õĺ���������ת

wireless_type_enum  wireless_type                   = NO_WIRELESS;
callback_function   wireless_module_uart_handler    = type_default_callback;                // ���ߴ��ڽ����жϺ���ָ�룬���ݳ�ʼ��ʱ���õĺ���������ת

//-------------------------------------------------------------------------------------------------------------------
// �������     Ĭ�ϻص�����
// ����˵��     void
// ���ز���     void
// ʹ��ʾ��     type_default_callback();
// ��ע��Ϣ     ������������� ��ֹ��û�г�ʼ���豸��ʱ���ܷ�
//-------------------------------------------------------------------------------------------------------------------
static void type_default_callback (void)
{

}
//-------------------------------------------------------------------------------------------------------------------
// �������     ��������ͷ����
// ����˵��     type_set        ѡ��������ͷ����
// ���ز���     void
// ʹ��ʾ��     set_camera_type(CAMERA_GRAYSCALE);
// ��ע��Ϣ     һ���ɸ�����ͷ��ʼ���ڲ�����
//-------------------------------------------------------------------------------------------------------------------
void set_camera_type (camera_type_enum type_set, callback_function vsync_callback, callback_function dma_callback, callback_function uart_callback)
{
    camera_type = type_set;
    if(vsync_callback == NULL)  camera_dma_handler      = type_default_callback;
    else                        camera_dma_handler      = dma_callback;
    if(dma_callback == NULL)    camera_vsync_handler    = type_default_callback;
    else                        camera_vsync_handler    = vsync_callback;
    if(uart_callback == NULL)   camera_uart_handler     = type_default_callback;
    else                        camera_uart_handler     = uart_callback;
}

//-------------------------------------------------------------------------------------------------------------------
// �������     ��������ģ������
// ����˵��     type_set        ѡ��������ģ������
// ���ز���     void
// ʹ��ʾ��     set_wireless_type(WIRELESS_UART);
// ��ע��Ϣ     һ���ɸ�����ͷ��ʼ���ڲ�����
//-------------------------------------------------------------------------------------------------------------------
void set_wireless_type (wireless_type_enum type_set, callback_function uart_callback)
{
    wireless_type = type_set;
    if(uart_callback == NULL)  wireless_module_uart_handler = type_default_callback;
    else                       wireless_module_uart_handler = uart_callback;
}

