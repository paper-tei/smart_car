#ifndef _TIMER_H
#define _TIMER_H
#include "sys.h"

void TIMx_calcPID_init(u16 arr,u16 psc);
void TIMx_calcPID_start(void);
void TIMx_calcPID_stop(void);
#endif
