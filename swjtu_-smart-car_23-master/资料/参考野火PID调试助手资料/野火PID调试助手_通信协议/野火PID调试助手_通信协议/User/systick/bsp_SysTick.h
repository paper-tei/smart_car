#ifndef __SYSTICK_H
#define __SYSTICK_H

#include "stm32f4xx.h"

/*简单任务管理*/
#define TASK_ENABLE 0
#define NumOfTask 3


void SysTick_Init(void);
void Delay_ms(__IO u32 nTime);
void TimingDelay_Decrement(void);

#endif /* __SYSTICK_H */
