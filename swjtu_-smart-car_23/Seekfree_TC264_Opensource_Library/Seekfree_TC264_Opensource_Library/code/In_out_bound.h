/*
 * In_out_bound.h
 *
 *  Created on: 2022��2��18��
 *      Author: paper
 */

#ifndef CODE_IN_OUT_BOUND_H_
#define CODE_IN_OUT_BOUND_H_
#include "zf_common_headfile.h"
void scratch_line(void);
void Inertial_Navigation_Update(void);
void Inertial_Navigation_Start(void);
void out_home(void);
void in_home(void);
extern int scratch_cnt, in_home_hang;
extern uint8 hall_detection_flag;
extern uint8 Zebra_flag;
extern uint8 speed_reversal_flag;
extern uint8 out_home_flag;
extern uint8 in_home_flag;



//===================下面是纯电磁出入库所用=====================//
void Exit_Garage_handler(void);
void Enter_Garage_handler(void);
float Distances_Calculate(float x1, float y1, float x2, float y2);
typedef struct {
    float Exit_Garage_distances;//出车库直行距离
    float Exit_Garage_set_yaw;   //出车库后转角

    unsigned char Exit_Garage_direction;//出库转向方向
    unsigned char Exit_Garage_state;//出库当前状态
    unsigned char Exit_Garage_state_last;//出库上一状态

    float Enter_Garge_distances;//看见斑马线直行距离
    float Enter_Garge_set_yaw;  //进车库转角
    unsigned char Enter_Garage_direction;//进库转向方向
    unsigned char Enter_Garage_state;//进库当前状态

}Garage;//车库

enum {

    Exit_Garage_straight,
    Exit_Garage_around,
    Exit_Garage_leave,

    Enter_Garage_flag,
    Enter_Garge_straight,
    Enter_Garage_around,
    Enter_Garge_strgight_low,//短走第二段直路
    Enter_Garage_leave
};

extern Garage  Exit_Garage;
extern Garage Enter_Garge;


#endif /* CODE_IN_OUT_BOUND_H_ */
