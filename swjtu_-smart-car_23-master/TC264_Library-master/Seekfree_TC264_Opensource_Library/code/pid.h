/*
 * pid.h
 *
 *  Created on: 2022年1月6日
 *      Author: paper
 */

#ifndef CODE_PID_H_
#define CODE_PID_H_
#include "zf_common_headfile.h"

typedef struct {
    float target_val;             //目标值，相当于SetValue
    float Kp;                       //比例系数Proportional
    float Ki;                       //积分系数Integral
    float Kd;                       //微分系数Derivative
    float Ek;                       //当前误差
    float Ek1;                      //前一次误差 e(k-1)
    float Ek2;                      //再前一次误差 e(k-2)
    float OUT;                      //PID输出
} PID_IncTypeDef;
#define camera  0
#define electromagnetic 1
#define Inertial_navigation 2
void PID_Inc_Init(PID_IncTypeDef* sptr, float kp, float ki, float kd);
float Positional_PID(PID_IncTypeDef* PID, float SetValue, float ActualValue, float Max_I);
void PID_param_init(void);

extern PID_IncTypeDef roll_angle_wheel, roll_anglespeed_wheel, roll_speed_wheel;
extern PID_IncTypeDef bldc_b_wheel, bldc_f_wheel, bdc_speed_wheel, roll_bdc_banlance_wheel,
bldc_yaw_anglespeed_wheel, bldc_yaw_angle_wheel, bldc_yaw_speed_wheel, bldc_lq_yaw_wheel, bldc_yaw_angle_wheel_Straight;
extern PID_IncTypeDef adc_pid, ROLL_Dynamic_zero_wheel;
extern float pidout_1b, pidout_2b, pidout_3b, pidout_4b, pidout_5b, pidout_6b,
pidout_1f, pidout_2f, pidout_3f, pidout_4f, pidout_5f, pidout_6f;
extern uint8 flag_ELECTROMAGNETIC_CAMERA;//摄像头与纯电磁选择标志位

float get_pid_target(PID_IncTypeDef* pid);
void set_pid_target(PID_IncTypeDef* pid, float temp_val);
void set_p_i_d(PID_IncTypeDef* pid, float p, float i, float d);
void PID_Change(void);
void PID_clear(PID_IncTypeDef* sptr);
extern float ring_speed;
extern float straight_speed;
extern float turn_speed;


#endif /* CODE_PID_H_ */
