/*
 * key.h
 *
 *  Created on: 2022��1��6��
 *      Author: paper
 */

#ifndef CODE_KEY_H_
#define CODE_KEY_H_
#include "zf_common_headfile.h"

void Key_scan(void);
extern uint8 sw1_status, key1_status;
extern uint8 sw2_status;
extern uint8 key1_flag;
extern uint8 key2_flag;
extern uint8 key3_flag;
extern uint8 key4_flag;
extern uint8 go_flag, key_val;
#endif /* CODE_KEY_H_ */
