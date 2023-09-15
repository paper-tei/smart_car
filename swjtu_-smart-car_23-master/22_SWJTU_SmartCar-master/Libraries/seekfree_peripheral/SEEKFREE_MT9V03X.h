/*********************************************************************************************************************
 * COPYRIGHT NOTICE
 * Copyright (c) 2020,��ɿƼ�
 * All rights reserved.
 * ��������QQȺ����Ⱥ��824575535
 *
 * �����������ݰ�Ȩ������ɿƼ����У�δ����������������ҵ��;��
 * ��ӭ��λʹ�ò������������޸�����ʱ���뱣����ɿƼ��İ�Ȩ������
 *
 * @file       		�����
 * @company	   		�ɶ���ɿƼ����޹�˾
 * @author     		��ɿƼ�(QQ3184284598)
 * @version    		�鿴doc��version�ļ� �汾˵��
 * @Software 		ADS v1.2.2
 * @Target core		TC364DP
 * @Taobao   		https://seekfree.taobao.com/
 * @date       		2020-11-23
 * @note		
					���߶��壺
					------------------------------------ 
					ģ��ܽ�            			��Ƭ���ܽ�
					SDA(51��RX)         		�鿴SEEKFREE_MT9V03X.h�ļ��е�MT9V03X_COF_UART_TX�궨��
					SCL(51��TX)         		�鿴SEEKFREE_MT9V03X.h�ļ��е�MT9V03X_COF_UART_RX�궨��
					���ж�(VSY)         		�鿴SEEKFREE_MT9V03X.h�ļ��е�MT9V03X_VSYNC_PIN�궨��
					���ж�(HREF)				����û��ʹ�ã���˲�����
					�����ж�(PCLK)      		�鿴SEEKFREE_MT9V03X.h�ļ��е�MT9V03X_PCLK_PIN�궨��
					���ݿ�(D0-D7)			�鿴SEEKFREE_MT9V03X.h�ļ��е�MT9V03X_DATA_PIN�궨��
					------------------------------------ 
	
					Ĭ�Ϸֱ�����           			188*120
					Ĭ��FPS                 50֡
 ********************************************************************************************************************/



#ifndef _SEEKFREE_MT9V03X_h
#define _SEEKFREE_MT9V03X_h

#include "common.h"
#include "IfxDma_cfg.h"
#include "zf_uart.h"

//��������ͷ����
#define MT9V03X_W               188             	//ͼ����� 	��Χ1-188
#define MT9V03X_H               120           	//ͼ��߶�	��Χ1-120



//--------------------------------------------------------------------------------------------------
//��������
//--------------------------------------------------------------------------------------------------
#define MT9V03X_COF_UART        UART_1         		//��������ͷ��ʹ�õ��Ĵ���
#define MT9V03X_COF_UART_TX     UART1_TX_P02_2
#define MT9V03X_COF_UART_RX     UART1_RX_P02_3

#define MT9V03X_VSYNC_PIN       ERU_CH3_REQ6_P02_0  //���ж�����	��ѡ��Χ�ο�ERU_PIN_enumö�� ��������������ѡ��Ϊͬһ��ͨ���������ǹ����жϵ�ͨ��
													//���糡�ж�ѡ��ERU_CH3 ��ô�������žͲ���ѡ��ERU_CH7����Ϊ3��7���ж��ǹ��õġ�

#define MT9V03X_DATA_PIN        P00_0  				//����D0��������  ����D0����ΪP00_0 ��ôD1��ʹ�õ�������ΪP00_1����������
													//�����ò���P00_0��P02_0��P15_0����������Ϊ��������

#define MT9V03X_PCLK_PIN        ERU_CH2_REQ14_P02_1 //��������ʱ������ ��ѡ��Χ�ο�ERU_PIN_enumö�� �����볡�ж�����ѡ��Ϊͬһ��ͨ���������ǹ����жϵ�ͨ��
													//���糡�ж�ѡ��ERU_CH3 ��ô�������žͲ���ѡ��ERU_CH7����Ϊ3��7���ж��ǹ��õġ�

#define MT9V03X_DMA_CH			IfxDma_ChannelId_5	//����ʹ�õ�DMAͨ�� 0-47��ѡ  ͨ����Խ�����ȼ�Խ��

//����ͷ����ö��
typedef enum
{
    INIT = 0,               //����ͷ��ʼ������
    AUTO_EXP,               //�Զ��ع�����
    EXP_TIME,               //�ع�ʱ������
    FPS,                    //����ͷ֡������
    SET_COL,                //ͼ��������
    SET_ROW,                //ͼ��������
    LR_OFFSET,              //ͼ������ƫ������
    UD_OFFSET,              //ͼ������ƫ������
    GAIN,                   //ͼ��ƫ������
    CONFIG_FINISH,          //������λ����Ҫ����ռλ����
    
    COLOR_GET_WHO_AM_I = 0xEF,
    SET_EXP_TIME = 0XF0,    //���������ع�ʱ������
    GET_STATUS,             //��ȡ����ͷ��������
    GET_VERSION,            //�̼��汾������
	
    SET_ADDR = 0XFE,        //�Ĵ�����ַ����
    SET_DATA                //�Ĵ�����������
}CMD;
     
     
     
extern uint8    receive[3];         //�����ڽ���������Ϣʱ����  ��ֹ�û�ʹ�øñ���
extern uint8    receive_num;        //�����ڽ���������Ϣʱ����  ��ֹ�û�ʹ�øñ���
extern vuint8   uart_receive_flag;  //�����ڽ���������Ϣʱ����  ��ֹ�û�ʹ�øñ���


extern uint8    mt9v03x_finish_flag;//һ��ͼ��ɼ���ɱ�־λ
extern uint8    mt9v03x_image[MT9V03X_H][MT9V03X_W];

void mt9v03x_uart_callback(void);
void set_config(UARTN_enum uartn, int16 buff[CONFIG_FINISH-1][2]);
void get_config(UARTN_enum uartn, int16 buff[CONFIG_FINISH-1][2]);
uint16 get_version(UARTN_enum uartn);
uint16 set_exposure_time(UARTN_enum uartn, uint16 light);

void mt9v03x_init(void);
void mt9v03x_vsync(void);
void mt9v03x_dma(void);
void seekfree_sendimg_03x(UARTN_enum uartn, uint8 *image, uint16 width, uint16 height);
void seekfree_sendimg_03x_usb_cdc(uint8 *image, uint16 width, uint16 height);



#endif

