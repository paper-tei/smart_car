
#ifndef BACK_LOOP_H_
#define BACK_LOOP_H_
#include "headfile.h"

void back_loop_find(void);
void back_loop_chu(void);
extern unsigned char back_loop_find_L ;
extern unsigned char back_loop_lose_L ;
extern unsigned char back_loop_find_R ;
extern unsigned char back_loop_lose_R ;
extern unsigned char back_loop_flag ,icm_back_loop_flag;
extern int icm_back_loop,number_of_back_loop;
extern unsigned char first_way;//0 ×ó 1 ÓÒ
#endif
