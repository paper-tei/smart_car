/*
 * init.h
 *
 *  Created on: 2022年12月16日
 *      Author: paper
 */
#ifndef CODE_IMU_H_
#define CODE_IMU_H_
#include "zf_common_headfile.h"

extern float GyroOffset_Xdata;
extern float GyroOffset_Ydata;
extern float GyroOffset_Zdata;
extern float GyroOffset_Xdata, icm_data_acc_x, icm_data_gyro_x;
extern float GyroOffset_Ydata, icm_data_acc_y, icm_data_gyro_y;
extern float GyroOffset_Zdata, icm_data_acc_z, icm_data_gyro_z;

void gyroOffset_init(void);
void ICM_getEulerianAngles(void);
extern float eulerAngle_yaw, eulerAngle_pitch, eulerAngle_roll, eulerAngle_yaw_total, test,test1,test2,test3;
#endif /* CODE_IMU_H_ */
