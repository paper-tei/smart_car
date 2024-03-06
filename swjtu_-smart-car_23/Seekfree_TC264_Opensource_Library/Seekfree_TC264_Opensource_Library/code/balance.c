/*
 * balance.c
 *
 *  Created on: 2022年11月18日
 *      Author: paper
 */
#include "balance.h"
float test_setvalue_acc = 0;
float test_setvalue = 0;
float eulerAngle_roll_max, eulerAngle_roll_min, eulerAngle_pitch_max, eulerAngle_pitch_min = 0;
float Tracking_Path = 0, BLDC_YAW_ANGLE_Value = 0; // 0 摄像头 1电磁
float Dynamic_zero = 0.0f;
float Dynamic_zero_set = 0.0f;
//左右平衡
float ROLL_ANGLESPEED_LOOP(float value) {
    float SetValue = value;
    float ActualValue = -icm_data_gyro_y;
    float out = Positional_PID(&roll_anglespeed_wheel, SetValue, ActualValue, 10);
    return out;
}

float ZHONGZHI_roll = -1.6f;//-1.8   -4.容易左拐(右拐饱和)   -0.5f容易右拐
float Coefficient = 5.25f;//相当于是给压弯角度限幅,
float ROLL_ANGLE_LOOP(float value) {
    float SetValue = value;
    //动态零点
    float ActualValue = eulerAngle_roll + ZHONGZHI_roll - Coefficient * Dynamic_zero;//
    //float ActualValue = eulerAngle_roll + ZHONGZHI_roll;
    float out = Positional_PID(&roll_angle_wheel, SetValue, ActualValue, 10);

    return out;
}


float ROLL_SPEED_LOOP(float value) {
    float SetValue = value;
    float ActualValue = (encoder_bldc_f + encoder_bldc_b) / 2;
    float out = Positional_PID(&roll_speed_wheel, SetValue, ActualValue, 10);
    return out;
}

//前后平衡
float ZHONGZHI_PITCH = 2.15f;
float BDC_BANLANCE_LOOP(float Angle, float Gyro) {
    float Bias;
    float balance;
    Bias = Angle + ZHONGZHI_PITCH;// - Coefficient * Dynamic_zero / 4;
    balance = roll_bdc_banlance_wheel.Kp * Bias + roll_bdc_banlance_wheel.Kd * Gyro;
    return balance;
}
float BDC_SPEED_LOOP(float encoder, float speed) {
    float SetValue = speed;
    float ActualValue = -encoder;
    float out = Positional_PID(&bdc_speed_wheel, SetValue, ActualValue, 500);
    return out;
}


//转向环
//角速度
float BLDC_YAW_ANGLESPEED_LOOP(float value) {
    float SetValue = value;
    float ActualValue = icm_data_gyro_z;
    float out = Positional_PID(&bldc_yaw_anglespeed_wheel, SetValue, ActualValue, 60);
    return out;
}

//角度
float BLDC_YAW_ANGLE_LOOP(float value) {
    float SetValue = value;
    float ActualValue = 0;
    float out = Positional_PID(&bldc_yaw_angle_wheel, SetValue, ActualValue, 40);//adc_error_2  -1到1；
    return out;
}
//速度
float BLDC_YAW_SPEED_LOOP(float value) {
    float SetValue = value;
    float ActualValue = encoder_bldc_f - encoder_bldc_b;

    float out = Positional_PID(&bldc_yaw_speed_wheel, SetValue, ActualValue, 2000);
    return out;
}


float limit(float val, float lower, float upper) {
    return val < lower ? lower : (val > upper ? upper : val);
}


//姿态是否稳定判断函数
//0稳定，1不稳定
uint8 Attitude_stability_judgment(float roll, float pitch) {
    uint8 flag_pose = 0;//0表示稳定，1表示不稳定，车体姿态标志位
    if (eulerAngle_roll_min > roll)
        eulerAngle_roll_min = roll;
    else if (eulerAngle_roll_max < roll)
        eulerAngle_roll_max = roll;
    if (eulerAngle_pitch_min > pitch)
        eulerAngle_pitch_min = pitch;
    else if (eulerAngle_pitch_max < pitch)
        eulerAngle_pitch_max = roll;
    //姿态不稳定，输出标志位，取消转向环
    if ((eulerAngle_pitch_max - eulerAngle_pitch_min) > 14 || (eulerAngle_roll_max - eulerAngle_roll_min) > 4.2) {
        flag_pose = 1;//不稳定
    }
    else if ((eulerAngle_pitch_max - eulerAngle_pitch_min) < 9 && (eulerAngle_roll_max - eulerAngle_roll_min) < 3)
        flag_pose = 0;//稳定
    else if ((eulerAngle_pitch_max - eulerAngle_pitch_min) > 14 || (eulerAngle_roll_max - eulerAngle_roll_min) > 5)
        flag_pose = 2;//极度不稳定
    eulerAngle_pitch_max = -ZHONGZHI_PITCH;
    eulerAngle_pitch_min = -ZHONGZHI_PITCH;
    eulerAngle_roll_max = -ZHONGZHI_roll + Coefficient * Dynamic_zero;
    eulerAngle_roll_min = -ZHONGZHI_roll + Coefficient * Dynamic_zero;
    return flag_pose;
}

//动态零点计算
float Dynamic_zero_cale(void) {
    float zero_set = 0;
    float image_Error1 = limit(image_Error, -30, 30);//限幅
    if (car_mode.now_mode == CAMERA)//摄像头弯道循迹时候
    {
        //Buzzer(1);
        //float encoder_bdc1 = encoder_bdc * 1;
        float encoder_bdc1 = (encoder_bdc) * 1;
        zero_set = KalmanFilter(&Zero_Kalman, atan((0.000001 * encoder_bdc1 * encoder_bdc1) * (-image_Error1) * 0.0125f));
    }
    else {
        // Buzzer(0);
        zero_set = KalmanFilter(&Zero_Kalman, 0);
    }
    //zero_set = limit(zero_set, -0.8f, 0.8f);
    return zero_set;
}






















