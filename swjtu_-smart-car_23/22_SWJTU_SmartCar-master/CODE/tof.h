/*
 * tof.h
 *
 *  Created on: 2022Äê4ÔÂ2ÈÕ
 *      Author: 24916
 */

#ifndef CODE_TOF_H_
#define CODE_TOF_H_
#include "headfile.h"
void Distance_measurement_Init(void);
void Distance_measurement(void);
void Distance_read(void);
uint32 ModbusCRC(uint32 *ptr,uint32 ucLen);//CRC??
extern int distance ;
extern unsigned char distance_finish;
extern uint32 distance_buff[200];
#endif /* CODE_TOF_H_ */
