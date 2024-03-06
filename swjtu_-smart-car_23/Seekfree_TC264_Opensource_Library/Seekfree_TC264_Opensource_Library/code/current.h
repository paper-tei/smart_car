#ifndef CODE_CURRENT_H_
#define CODE_CURRENT_H_
#include "zf_common_headfile.h"
void BLDC_Current(void);
void BLDC_Current_Init(void);
extern int32 bldc_f_current_value;
extern int32 bldc_b_current_value;
extern int32 bldc_b_current_value_error, bldc_f_current_value_error;
#endif