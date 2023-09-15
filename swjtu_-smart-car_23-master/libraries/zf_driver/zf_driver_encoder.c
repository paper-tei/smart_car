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
* �ļ�����          zf_driver_encoder
* ��˾����          �ɶ���ɿƼ����޹�˾
* �汾��Ϣ          �鿴 libraries/doc �ļ����� version �ļ� �汾˵��
* ��������          ADS v1.9.4
* ����ƽ̨          TC264D
* ��������          https://seekfree.taobao.com/
*
* �޸ļ�¼
* ����              ����                ��ע
* 2022-09-15       pudding            first version
********************************************************************************************************************/

#include "IfxGpt12_IncrEnc.h"
#include "zf_common_debug.h"
#include "zf_driver_encoder.h"

//-------------------------------------------------------------------------------------------------------------------
// �������     ��������ַ���� �ڲ�����
// ����˵��     gptn            ѡ����ʹ�õ�GPT12��ʱ��
// ����˵��     count_pin       ���ü�������
// ����˵��     dir_pin         ���ü�����������
// ���ز���     void
// ʹ��ʾ��     encoder_mapping_set(gptn, ch1_pin, ch2_pin);
// ��ע��Ϣ
//-------------------------------------------------------------------------------------------------------------------
static void encoder_mapping_set (encoder_index_enum encoder_n, encoder_channel1_enum ch1_pin, encoder_channel2_enum ch2_pin)
{
    IfxGpt12_TxIn_In  *ch1;
    IfxGpt12_TxEud_In *ch2;

    switch(encoder_n)
    {
        case TIM2_ENCODER:
        {
            if      (TIM2_ENCODER_CH1_P00_7  == ch1_pin)    ch1   = &IfxGpt120_T2INA_P00_7_IN;
            else if (TIM2_ENCODER_CH1_P33_7  == ch1_pin)    ch1   = &IfxGpt120_T2INB_P33_7_IN;
            else    zf_assert(FALSE);

            if      (TIM2_ENCODER_CH2_P00_8  == ch2_pin)    ch2   = &IfxGpt120_T2EUDA_P00_8_IN;
            else if (TIM2_ENCODER_CH2_P33_6  == ch2_pin)    ch2   = &IfxGpt120_T2EUDB_P33_6_IN;
            else    zf_assert(FALSE);
        }break;

        case TIM3_ENCODER:
        {
            if      (TIM3_ENCODER_CH1_P02_6  == ch1_pin)    ch1   = &IfxGpt120_T3INA_P02_6_IN;
            else    zf_assert(FALSE);

            if      (TIM3_ENCODER_CH2_P02_7  == ch2_pin)    ch2   = &IfxGpt120_T3EUDA_P02_7_IN;
            else    zf_assert(FALSE);
        }break;

        case TIM4_ENCODER:
        {
            if      (TIM4_ENCODER_CH1_P02_8  == ch1_pin)    ch1   = &IfxGpt120_T4INA_P02_8_IN;
            else    zf_assert(FALSE);

            if      (TIM4_ENCODER_CH2_P00_9  == ch2_pin)    ch2   = &IfxGpt120_T4EUDA_P00_9_IN;
            else if (TIM4_ENCODER_CH2_P33_5  == ch2_pin)    ch2   = &IfxGpt120_T4EUDB_P33_5_IN;
            else    zf_assert(FALSE);
        }break;

        case TIM5_ENCODER:
        {
            if      (TIM5_ENCODER_CH1_P21_7  == ch1_pin)    ch1   = &IfxGpt120_T5INA_P21_7_IN;
            else if (TIM5_ENCODER_CH1_P10_3  == ch1_pin)    ch1   = &IfxGpt120_T5INB_P10_3_IN;
            else    zf_assert(FALSE);

            if      (TIM5_ENCODER_CH2_P21_6  == ch2_pin)    ch2   = &IfxGpt120_T5EUDA_P21_6_IN;
            else if (TIM5_ENCODER_CH2_P10_1  == ch2_pin)    ch2   = &IfxGpt120_T5EUDB_P10_1_IN;
            else    zf_assert(FALSE);
        }break;

        case TIM6_ENCODER:
        {
            if      (TIM6_ENCODER_CH1_P20_3  == ch1_pin)    ch1   = &IfxGpt120_T6INA_P20_3_IN;
            else if (TIM6_ENCODER_CH1_P10_2  == ch1_pin)    ch1   = &IfxGpt120_T6INB_P10_2_IN;
            else    zf_assert(FALSE);

            if      (TIM6_ENCODER_CH2_P20_0  == ch2_pin)    ch2   = &IfxGpt120_T6EUDA_P20_0_IN;
            else    zf_assert(FALSE);
        }break;
    }
#pragma warning 507

    IfxGpt12_initTxInPinWithPadLevel(ch1,  IfxPort_InputMode_pullUp, IfxPort_PadDriver_cmosAutomotiveSpeed1);
    IfxGpt12_initTxEudInPinWithPadLevel(ch2, IfxPort_InputMode_pullUp, IfxPort_PadDriver_cmosAutomotiveSpeed1);

#pragma warning default
}

//-------------------------------------------------------------------------------------------------------------------
//  �������      ���������ݲɼ�
//  ����˵��      gptn            ѡ����ʹ�õĶ�ʱ��
//  ���ز���      void
//  ʹ��ʾ��      int16 speed; speed = encoder_get_count(TIM2_ENCODER); // ʹ��T2��ʱ��
//  ��ע��Ϣ
//-------------------------------------------------------------------------------------------------------------------
int16 encoder_get_count (encoder_index_enum encoder_n)
{
    int16 encoder_data = 0;
    switch(encoder_n)
    {
        case TIM2_ENCODER: encoder_data = (int16)IfxGpt12_T2_getTimerValue(&MODULE_GPT120); break;
        case TIM3_ENCODER: encoder_data = (int16)IfxGpt12_T3_getTimerValue(&MODULE_GPT120); break;
        case TIM4_ENCODER: encoder_data = (int16)IfxGpt12_T4_getTimerValue(&MODULE_GPT120); break;
        case TIM5_ENCODER: encoder_data = (int16)IfxGpt12_T5_getTimerValue(&MODULE_GPT120); break;
        case TIM6_ENCODER: encoder_data = (int16)IfxGpt12_T6_getTimerValue(&MODULE_GPT120); break;
        default: encoder_data = 0;
    }
    return encoder_data;
}
//-------------------------------------------------------------------------------------------------------------------
//  �������      �������������
//  ����˵��      gptn            ѡ����ʹ�õĶ�ʱ��
//  ���ز���      void
//  ʹ��ʾ��      encoder_clear_count(TIM2_ENCODER);// ʹ��T2��ʱ��
//  ��ע��Ϣ
//-------------------------------------------------------------------------------------------------------------------
void encoder_clear_count (encoder_index_enum encoder_n)
{
    switch(encoder_n)
    {
        case TIM2_ENCODER: IfxGpt12_T2_setTimerValue(&MODULE_GPT120, 0); break;
        case TIM3_ENCODER: IfxGpt12_T3_setTimerValue(&MODULE_GPT120, 0); break;
        case TIM4_ENCODER: IfxGpt12_T4_setTimerValue(&MODULE_GPT120, 0); break;
        case TIM5_ENCODER: IfxGpt12_T5_setTimerValue(&MODULE_GPT120, 0); break;
        case TIM6_ENCODER: IfxGpt12_T6_setTimerValue(&MODULE_GPT120, 0); break;
    }
}

//-------------------------------------------------------------------------------------------------------------------
//  �������      �����������ɼ���ʼ��
//  ����˵��      encoder_n       ѡ����ʹ�õ�GPT12��ʱ��
//  ����˵��      ch1_pin         ENCODER ͨ�� 1
//  ����˵��      ch2_pin         ENCODER ͨ�� 2
//  ���ز���      void
//  ʹ��ʾ��      encoder_quad_init(TIM2_ENCODER, TIM2_ENCODER_CH1_P00_7, TIM2_ENCODER_CH2_P00_8);// ʹ��T2��ʱ��   P00_7����ΪAͨ��    P00_8����ΪBͨ��
//  ��ע��Ϣ
//-------------------------------------------------------------------------------------------------------------------
void encoder_quad_init (encoder_index_enum encoder_n, encoder_channel1_enum count_pin, encoder_channel2_enum dir_pin)
{
    IfxGpt12_enableModule(&MODULE_GPT120);
    IfxGpt12_setGpt1BlockPrescaler(&MODULE_GPT120, IfxGpt12_Gpt1BlockPrescaler_4);
    IfxGpt12_setGpt2BlockPrescaler(&MODULE_GPT120, IfxGpt12_Gpt2BlockPrescaler_4);
    encoder_mapping_set(encoder_n, count_pin, dir_pin);

    switch(encoder_n)
    {
        case TIM2_ENCODER:
        {
            IfxGpt12_T2_setCounterInputMode(&MODULE_GPT120, IfxGpt12_CounterInputMode_risingEdgeTxIN);
            IfxGpt12_T2_setDirectionSource (&MODULE_GPT120, IfxGpt12_TimerDirectionSource_external);
            IfxGpt12_T2_setMode            (&MODULE_GPT120, IfxGpt12_Mode_counter);
            IfxGpt12_T2_run                (&MODULE_GPT120, IfxGpt12_TimerRun_start);
        }break;

        case TIM3_ENCODER:
        {
            IfxGpt12_T3_setCounterInputMode(&MODULE_GPT120, IfxGpt12_CounterInputMode_risingEdgeTxIN);
            IfxGpt12_T3_setDirectionSource (&MODULE_GPT120, IfxGpt12_TimerDirectionSource_external);
            IfxGpt12_T3_setMode            (&MODULE_GPT120, IfxGpt12_Mode_counter);
            IfxGpt12_T3_run                (&MODULE_GPT120, IfxGpt12_TimerRun_start);
        }break;

        case TIM4_ENCODER:
        {
            IfxGpt12_T4_setCounterInputMode(&MODULE_GPT120, IfxGpt12_CounterInputMode_risingEdgeTxIN);
            IfxGpt12_T4_setDirectionSource (&MODULE_GPT120, IfxGpt12_TimerDirectionSource_external);
            IfxGpt12_T4_setMode            (&MODULE_GPT120, IfxGpt12_Mode_counter);
            IfxGpt12_T4_run                (&MODULE_GPT120, IfxGpt12_TimerRun_start);
        }break;

        case TIM5_ENCODER:
        {
            IfxGpt12_T5_setCounterInputMode(&MODULE_GPT120, IfxGpt12_CounterInputMode_risingEdgeTxIN);
            IfxGpt12_T5_setDirectionSource (&MODULE_GPT120, IfxGpt12_TimerDirectionSource_external);
            IfxGpt12_T5_setMode            (&MODULE_GPT120, IfxGpt12_Mode_counter);
            IfxGpt12_T5_run                (&MODULE_GPT120, IfxGpt12_TimerRun_start);
        }break;

        case TIM6_ENCODER:
        {
            IfxGpt12_T6_setCounterInputMode(&MODULE_GPT120, IfxGpt12_CounterInputMode_risingEdgeTxIN);
            IfxGpt12_T6_setDirectionSource (&MODULE_GPT120, IfxGpt12_TimerDirectionSource_external);
            IfxGpt12_T6_setMode            (&MODULE_GPT120, IfxGpt12_Mode_counter);
            IfxGpt12_T6_run                (&MODULE_GPT120, IfxGpt12_TimerRun_start);
        }break;
    }
}

//-------------------------------------------------------------------------------------------------------------------
//  �������      ����������ɼ���ʼ��
//  ����˵��      encoder_n       ѡ����ʹ�õ�GPT12��ʱ��
//  ����˵��      ch1_pin         ���ü�������
//  ����˵��      ch2_pin         ���÷�������
//  ���ز���      void
//  ʹ��ʾ��      encoder_quad_init(TIM2_ENCODER, TIM2_ENCODER_CH1_P00_7, TIM2_ENCODER_CH2_P00_8);// ʹ��T2��ʱ��   P00_7���Ž��м���    ��������ʹ��P00_8����
//  ��ע��Ϣ      Ӣ����ϵ�е�Ƭ���������������ͷ�����������˴��������ӿڷ����û�ʹ��
//-------------------------------------------------------------------------------------------------------------------
void encoder_dir_init (encoder_index_enum encoder_n, encoder_channel1_enum ch1_pin, encoder_channel2_enum ch2_pin)
{
    encoder_quad_init(encoder_n, ch1_pin, ch2_pin);
}

