/*
 * time.h
 *
 *  Created on: 2022年1月6日
 *      Author: paper
 */

#ifndef CODE_TIME_H_
#define CODE_TIME_H_

#include "headfile.h"

extern unsigned char   int_OK;//初始化成功标志位
//核心关键的标志位结构体定义
typedef struct
{        
      unsigned char T_2ms;             
      unsigned char T_6ms ;             
      unsigned char T_10ms ;             
      unsigned char T_20ms ;  
      unsigned char T_1s ;            
       

}Body;
extern Body Flag;
extern int test;
extern int start_count;
/*********函数声明**************/
void Fuse_result(void)  ;
//核心关键的标志位结构体初始化
void Flag_Init(void);
extern float actual_val_left;
  extern      float actual_val_right ;

extern float pid_speed_target_val_left;
extern float pid_speed_target_val_right;


#endif /* CODE_TIME_H_ */
