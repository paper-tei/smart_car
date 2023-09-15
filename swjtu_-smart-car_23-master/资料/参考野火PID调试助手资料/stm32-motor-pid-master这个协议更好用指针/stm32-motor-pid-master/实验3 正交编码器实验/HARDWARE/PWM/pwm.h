#ifndef _PWM_H
#define _PWM_H
#include "sys.h"

void TIM3_PWM_Init(u32 arr,u32 psc);
void set_pwm(int pwm);
#endif
