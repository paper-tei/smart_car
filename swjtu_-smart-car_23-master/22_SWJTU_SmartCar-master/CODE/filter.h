#ifndef CODE_FILTER_H_
#define CODE_FILTER_H_
#include "headfile.h"
typedef struct{

    float ACCEL_X;
    float ACCEL_Y;
    float ACCEL_Z;
    float GYRO_X;
    float GYRO_Y;
    float GYRO_Z;
    //角度
    float Pitch;
    float Yaw;
    float Roll;
    //角速度
    float PitchVelocity;
    float RollVelocity;
    float YawVelocity;
    float YawVelocity_offset;//偏航角速度偏置
}ICMDatatypedef;

extern ICMDatatypedef icmdata;
extern int ramp_pitch;


float encfilter(signed int encgetvalue);
extern  short  Roll,Pitch,Yaw;
float Kalman_Filter(float angle_m, float gyro_m);

void IMUupdate(float gx, float gy, float gz, float ax, float ay, float az);
float angle_calc(float angle_m, float gyro_m);
float angle_calc(float angle_m, float gyro_m);

#endif /* CODE_FILTER_H_ */
