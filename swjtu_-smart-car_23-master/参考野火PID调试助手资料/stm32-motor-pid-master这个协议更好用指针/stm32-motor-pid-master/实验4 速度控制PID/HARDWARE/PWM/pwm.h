#ifndef _PWM_H
#define _PWM_H
#include "sys.h"

void TIMx_pwm_init(u32 arr,u32 psc);
void set_pwm(int pwm);
#endif
