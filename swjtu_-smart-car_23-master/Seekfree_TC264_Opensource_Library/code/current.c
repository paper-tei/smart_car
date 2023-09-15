/*
 * current.c
 *
 *  Created on: 2023年1月4日
 *      Author: paper
 */
#include "current.h"
int32 bldc_f_current_value = 0;
int32 bldc_b_current_value = 0;
int32 bldc_b_current_value_error = 0, bldc_f_current_value_error = 0;
void BLDC_Current(void) {
    bldc_b_current_value =
        adc_mean_filter_convert(A_B_PIN, 5) - bldc_b_current_value_error;
    bldc_f_current_value =
        adc_mean_filter_convert(A_F_PIN, 5) - bldc_f_current_value_error;
}

void BLDC_Current_Init(void) {
    bldc_b_current_value_error = adc_mean_filter_convert(A_B_PIN, 240);
    bldc_f_current_value_error = adc_mean_filter_convert(A_F_PIN, 240);
}