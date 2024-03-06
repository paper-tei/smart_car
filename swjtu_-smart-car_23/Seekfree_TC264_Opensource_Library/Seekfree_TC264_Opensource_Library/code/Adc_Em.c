/*
 * Adc_Em.h
 *
 *  Created on: 2023年3月16日
 *      Author: Cappi
 * Emil:2046203886@qq.com
 */



#include"Adc_Em.h"


uint16  max_v[4], w;
uint16  min_v[4];
uint16   AD_valu[4];
//???ADC????????1?71?1?771?1?71?1?7771?1?71?1?771?1?71?1?7777
uint16  AD_val_1_max;
uint16  AD_val_2_max;
uint16  AD_val_3_max;
uint16  AD_val_4_max;
uint16  AD_val_5_max;
uint16  AD_val_6_max;
uint16  AD_val_7_max;

//////////////////////////
//???ADC??????��?
uint16  AD_val_1_min;
uint16  AD_val_2_min;
uint16  AD_val_3_min;
uint16  AD_val_4_min;
uint16  AD_val_5_min;
uint16  AD_val_6_min;
uint16  AD_val_7_min;

//
uint16 AD1DD = 1504;
uint16 AD2DD = 1158;
uint16 AD3DD = 1158;
uint16 AD4DD = 2014;
uint16 AD5DD;
uint16 AD6DD;
uint16 AD7DD;

//??????adc????7??????
#define CHANNEL_NUMBER          (4)
 //???ADC??????1?71?1?771?1?71?1?7771?1?71?1?771?1?71?1?7777
uint16   AD_val_1;
uint16   AD_val_2;
uint16   AD_val_3;
uint16   AD_val_4;
COefficient coefficient_L;
Error Error_Em;
// uint8 channel_index = 0;
adc_channel_enum channel_list[CHANNEL_NUMBER] =
{
    ADC_CHANNEL1, ADC_CHANNEL2, ADC_CHANNEL3, ADC_CHANNEL4
};
//????????????ADC????
int   disgy_AD_val_1, disgy_AD_val_2, disgy_AD_val_3, disgy_AD_val_4, disgy_AD_val_5, disgy_AD_val_6, disgy_AD_val_7;
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define MINMAX(input, low, upper) MIN(MAX(input, low), upper)




float adc_angle_1 = 0;
float adc_angle_2 = 0;
// printf(
//     "ADC channel %d mean filter convert data is %d.\r\n",
//     channel_index + 1,
            //     adc_mean_filter_convert(channel_list[channel_index], 10));      // 循环输出 10 次均值滤波转换结果


//-------------------------------------------------------------------------------------------------------------------
// 函数简介     ADC 均值+中值滤波转换
// 参数说明     ch              选择 ADC 通道 (详见 zf_driver_adc.h 中枚举 adc_channel_enum 定义)
// 参数说明     count_median    中值滤波的基数
// 参数说明     count           均值滤波次数
// 返回参数     uint16          转换的 ADC 值
// 使用示例     adc_mean_filter_median_convert(channel_list[3], 3, 5);                       // 3个数中中值滤波 采集5次中值 然后返回平均值
// 备注信息
//-------------------------------------------------------------------------------------------------------------------
uint16 adc_mean_filter_median_convert(adc_channel_enum vadc_chn, uint8 count_median, uint8 count) {
    uint8 i, i_1;
    uint32 sum;
    uint16 max, min, median, actual = 0;
    zf_assert(count);// 断言次数不能为0


    sum = 0;
    for (i = 0; i < count; i++) {
        for (i_1 = 0; i_1 < count_median; i_1++) {
            actual = adc_convert(vadc_chn);
            if (i_1 == 0)
                median = max = min = actual;

            if (actual > max)
                max = actual;
            else if (actual < min)
                min = actual;
            else
                median = actual;

            if (i_1 == i_1 - 1) {
                max = 0;
                min = 0;
                actual = 0;
            }
        }
        sum += median;
        median = 0;
    }

    sum = sum / count;

    return (uint16)sum;
}



///////电磁组这就是归一化，做电磁的这个必须加，很有用，让你使用不同的赛道，可以直接复制，
////原理很简单车子扫一下就是采集一个电感的最大值，和最小值，然后跑的时候检测到的值除以（最大值减最小值）乘以100,
void Em_black_Init(void) {
    /*

    uint16  i, j;

    max_v[0] = max_v[1] = max_v[2] = max_v[3] = 1000;
    min_v[0] = min_v[1] = min_v[2] = min_v[3] = 5000;

    for (i = 0;i < 3600;i++) {
        AD_valu[0] = adc_mean_filter_convert(channel_list[0], 2);  //
        AD_valu[1] = adc_mean_filter_convert(channel_list[1], 2);  //
        AD_valu[2] = adc_mean_filter_convert(channel_list[2], 2);//
        AD_valu[3] = adc_mean_filter_convert(channel_list[3], 2); //


        for (j = 0;j < 4;j++) {
            if (AD_valu[j] > max_v[j]) {

                max_v[j] = AD_valu[j];

            }

            if (AD_valu[j] < min_v[j]) {
                min_v[j] = AD_valu[j];
            }
        }
        system_delay_ms(6);        //延时
    }

    for (j = 0;j < 4;j++) {
        if (AD_valu[j] > max_v[j]) {

            max_v[j] = AD_valu[j];

        }

        if (AD_valu[j] < min_v[j]) {
            min_v[j] = AD_valu[j];
        }
    }


    AD_val_1_max = max_v[0];
    AD_val_2_max = max_v[1];
    AD_val_3_max = max_v[2];
    AD_val_4_max = max_v[3];
    AD_val_1_min = min_v[0];
    AD_val_2_min = min_v[1];
    AD_val_3_min = min_v[2];
    AD_val_4_min = min_v[3];
    */
    AD_val_1_max = debug.data[1];
    AD_val_2_max = debug.data[2];
    AD_val_3_max = debug.data[3];
    AD_val_4_max = debug.data[4];
    AD_val_1_min = debug.data[5];
    AD_val_2_min = debug.data[6];
    AD_val_3_min = debug.data[7];
    AD_val_4_min = debug.data[8];

    AD1DD = AD_val_1_max - AD_val_1_min;
    AD2DD = AD_val_2_max - AD_val_2_min;
    AD3DD = AD_val_3_max - AD_val_3_min;
    AD4DD = AD_val_4_max - AD_val_4_min;



}
void Em_black_Scan(void) {


    uint16  i, j;

    max_v[0] = max_v[1] = max_v[2] = max_v[3] = 1000;
    min_v[0] = min_v[1] = min_v[2] = min_v[3] = 5000;

    for (i = 0;i < 3600;i++) {
        //Buzzer(1);
        AD_valu[0] = adc_mean_filter_convert(channel_list[0], 2);  //
        AD_valu[1] = adc_mean_filter_convert(channel_list[1], 2);  //
        AD_valu[2] = adc_mean_filter_convert(channel_list[2], 2);//
        AD_valu[3] = adc_mean_filter_convert(channel_list[3], 2); //


        for (j = 0;j < 4;j++) {
            if (AD_valu[j] > max_v[j]) {

                max_v[j] = AD_valu[j];

            }

            if (AD_valu[j] < min_v[j]) {
                min_v[j] = AD_valu[j];
            }
        }
        system_delay_ms(6);        //延时
        //Buzzer(0);
    }

    AD_val_1_max = max_v[0] * 2;
    AD_val_2_max = max_v[1] * 2;
    AD_val_3_max = max_v[2] * 2;
    AD_val_4_max = max_v[3] * 2;
    AD_val_1_min = min_v[0];
    AD_val_2_min = min_v[1];
    AD_val_3_min = min_v[2];
    AD_val_4_min = min_v[3];

    debug.data[1] = AD_val_1_max;
    debug.data[2] = AD_val_2_max;
    debug.data[3] = AD_val_3_max;
    debug.data[4] = AD_val_4_max;
    debug.data[5] = AD_val_1_min;
    debug.data[6] = AD_val_2_min;
    debug.data[7] = AD_val_3_min;
    debug.data[8] = AD_val_4_min;

    AD1DD = AD_val_1_max - AD_val_1_min;
    AD2DD = AD_val_2_max - AD_val_2_min;
    AD3DD = AD_val_3_max - AD_val_3_min;
    AD4DD = AD_val_4_max - AD_val_4_min;
    key_set_data();
}

//读取电感ADC
void getadval() {

    AD_val_1 = adc_mean_filter_median_convert(channel_list[0], 3, 2);      //电感电压
    AD_val_2 = adc_mean_filter_median_convert(channel_list[1], 3, 2);
    AD_val_3 = adc_mean_filter_median_convert(channel_list[2], 3, 2);
    AD_val_4 = adc_mean_filter_median_convert(channel_list[3], 3, 2);
    // AD_val_1 =  adc_mean_filter_convert(channel_list[0],  5);      //电感电压
    // AD_val_2 =  adc_mean_filter_convert(channel_list[1],  5);
    // AD_val_3 =  adc_mean_filter_convert(channel_list[2],  5);
    // AD_val_4 =  adc_mean_filter_convert(channel_list[3],  5);

  //限幅
    // if (AD_val_1 > AD_val_1_max)		AD_val_1 = AD_val_1_max;
    // if (AD_val_2 > AD_val_2_max)		AD_val_2 = AD_val_2_max;
    // if (AD_val_3 > AD_val_3_max)		AD_val_3 = AD_val_3_max;
    // if (AD_val_4 > AD_val_4_max)		AD_val_4 = AD_val_4_max;


    // if (AD_val_1 < AD_val_1_min)		AD_val_1 = AD_val_1_min;
    // if (AD_val_2 < AD_val_2_min)		AD_val_2 = AD_val_2_min;
    // if (AD_val_3 < AD_val_3_min)		AD_val_3 = AD_val_3_min;
    // if (AD_val_4 < AD_val_4_min)		AD_val_4 = AD_val_4_min;



    //我觉得1000的话可能更稳，后面试试效果
    disgy_AD_val_1 = (int)100 * (AD_val_1 - AD_val_1_min) / AD1DD;//左一
    disgy_AD_val_2 = (int)100 * (AD_val_2 - AD_val_2_min) / AD2DD;//左2
    disgy_AD_val_3 = (int)100 * (AD_val_3 - AD_val_3_min) / AD3DD;
    disgy_AD_val_4 = (int)100 * (AD_val_4 - AD_val_4_min) / AD4DD;

    // 测试
        //    ips200_show_float(200, 120, disgy_AD_val_1, 4, 4);
        //    ips200_show_float(200, 140,disgy_AD_val_2, 4, 4);
        //    ips200_show_float(200, 160, disgy_AD_val_3, 4, 4);
        //    ips200_show_float(200, 180, disgy_AD_val_4, 4, 4);




}


void em_calculate(void) {
    int adc_sum_1, adc_dec_1;
    adc_sum_1 = disgy_AD_val_1 + disgy_AD_val_4;
    adc_dec_1 = disgy_AD_val_4 - disgy_AD_val_1;
    Error_Em.adc_incline = (float)adc_dec_1 / (float)adc_sum_1;//差比和


    float adc_sum_2, adc_dec_2;
    adc_sum_2 = disgy_AD_val_2 + disgy_AD_val_3;
    adc_dec_2 = disgy_AD_val_3 - disgy_AD_val_2;
    Error_Em.adc_dierct = (float)adc_dec_2 / (float)adc_sum_2;

    Error_Em.adc_sum = coefficient_L.incline * Error_Em.adc_incline + coefficient_L.direct * Error_Em.adc_dierct;
}
