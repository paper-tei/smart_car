/*
 * motor.h
 *
 *  Created on: 2022�?11�?18�?
 *      Author: paper
 */
#ifndef CODE_MOTOR_H_
#define CODE_MOTOR_H_
#include "zf_common_headfile.h"
extern int32 bldc_b_speed, bldc_f_speed;
extern float bdc_speed, bdc_speed_max;
extern float error_prouette;
void SET_BLDC_B_PWM(int32 ERROR);
void SET_BLDC_F_PWM(int32 ERROR);
void STOP_BLDC(uint8 STOP);
void SET_BLDC_B_SPEED(int32 ERROR);
void SET_BLDC_F_SPEED(int32 ERROR);
void SET_BDC_PWM(int32 ERROR);
void SET_SERVO_PWM(int32 ERROR);
void yaw_pirouette(float yaw_agree);

#endif /* CODE_MOTOR_H_ */
