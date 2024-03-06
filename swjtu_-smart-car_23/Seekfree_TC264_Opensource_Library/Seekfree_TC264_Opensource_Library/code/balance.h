/*
 * balance.h
 *
 *  Created on: 2022年12月16日
 *      Author: paper
 */
#ifndef CODE_BALANCE_H_
#define CODE_BALANCE_H_
#include "zf_common_headfile.h"

float ROLL_ANGLE_LOOP(float Set_Pitch);
float ROLL_ANGLESPEED_LOOP(float value);
float ROLL_SPEED_LOOP(float value);
float BDC_BANLANCE_LOOP(float Angle, float Gyro);
float BDC_SPEED_LOOP(float encoder, float speed);
float BLDC_DIRECTION_LOOP(float direction);
float BLDC_YAW_ANGLESPEED_LOOP(float value);
float BLDC_YAW_ANGLE_LOOP(float value);
float BLDC_YAW_SPEED_LOOP(float value);
float BLDC_LQ_YAW_LOOP(float angle, float angle_zero, float gyro);
float limit(float val, float lower, float upper);
extern float Balance_Kp, Tracking_Path;
extern float test_setvalue_acc, test_setvalue, BLDC_YAW_ANGLE_Value;
extern float Dynamic_zero;
extern float Dynamic_zero_set, ZHONGZHI_roll, ZHONGZHI_PITCH;
uint8 Attitude_stability_judgment(float roll, float pitch);
float Dynamic_zero_cale(void);
#endif
