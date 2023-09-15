/*
 * motor.c
 *
 *  Created on: 2022年11月18日
 *      Author: paper
 */
#include "motor.h"

int32 bldc_b_speed = 0;
int32 bldc_f_speed = 0;
float bdc_speed = 0;//给一个速度700
float bdc_speed_max = 0;
void SET_BLDC_F_PWM(int32 ERROR) {
    if (ERROR > PWM_DUTY_MAX)ERROR = PWM_DUTY_MAX;
    else if (ERROR < -PWM_DUTY_MAX)ERROR = -PWM_DUTY_MAX;
    if (ERROR >= 0) {
        pwm_set_duty(BLDC_F_PWM_PIN, PWM_DUTY_MAX - ERROR);
        gpio_set_level(BLDC_F_DIR_PIN, 0);
    }
    else {
        pwm_set_duty(BLDC_F_PWM_PIN, PWM_DUTY_MAX + ERROR);
        gpio_set_level(BLDC_F_DIR_PIN, 1);
    }
}

void SET_BLDC_B_PWM(int32 ERROR) {
    if (ERROR > PWM_DUTY_MAX)ERROR = PWM_DUTY_MAX;
    else if (ERROR < -PWM_DUTY_MAX)ERROR = -PWM_DUTY_MAX;
    if (ERROR >= 0) {
        pwm_set_duty(BLDC_B_PWM_PIN, PWM_DUTY_MAX - ERROR);
        gpio_set_level(BLDC_B_DIR_PIN, 0);
    }
    else {
        pwm_set_duty(BLDC_B_PWM_PIN, PWM_DUTY_MAX + ERROR);
        gpio_set_level(BLDC_B_DIR_PIN, 1);
    }
}


void SET_BLDC_B_SPEED(int32 ERROR) {
    float pwm = Positional_PID(&bldc_b_wheel, (float)ERROR, (float)encoder_bldc_b, 3100);
    if (pwm > 10000)pwm = 10000;
    else if (pwm < -10000)pwm = -10000;
    if (ERROR == 0 && (encoder_bldc_b > -6 && encoder_bldc_b < 6))pwm = 0;
    SET_BLDC_B_PWM((int)pwm);
}

void SET_BLDC_F_SPEED(int32 ERROR) {
    float pwm = Positional_PID(&bldc_f_wheel, (float)ERROR, (float)-encoder_bldc_f, 3100);
    if (pwm > 10000)pwm = 10000;
    else if (pwm < -10000)pwm = -10000;
    if (ERROR == 0 && (encoder_bldc_f > -6 && encoder_bldc_f < 6))pwm = 0;

    SET_BLDC_F_PWM((int)pwm);
}


void SET_BDC_PWM(int32 ERROR) {
    if (ERROR >= 4000)ERROR = 4000;
    else if (ERROR <= -4000)ERROR = -4000;

    if (ERROR >= 0) {
        pwm_set_duty(BDC_PWM_PIN, ERROR);
        pwm_set_duty(BDC_DIR_PIN, 0);
    }
    else {
        pwm_set_duty(BDC_PWM_PIN, 0);
        pwm_set_duty(BDC_DIR_PIN, -ERROR);
    }
}


// int32 ERROR_old = 0;
//int low_pas = 0.85;
void SET_SERVO_PWM(int32 ERROR) {

    //    if (ERROR >= 300)ERROR = 300;
    //    else if (ERROR <= 250)ERROR = 250;
    //    ERROR = ERROR * low_pas + ERROR_old * (1 - low_pas);
    pwm_set_duty(SERVO_A_PWM_PIN, ERROR);
    pwm_set_duty(SERVO_B_PWM_PIN, ERROR);
    //    ERROR_old = ERROR;
}
void STOP_BLDC(uint8 STOP) {
    if (STOP) {
        gpio_set_level(BLDC_F_STOP_PIN, 0);
        gpio_set_level(BLDC_B_STOP_PIN, 0);
    }
}
float error_prouette = 0;



// // 可尝试对抗积分饱和，未测试
// void yaw_pirouette (float yaw_agree)//#转的角度，左负右正
// {


//     float error_prouette_1 = 0;
//     yaw_agree = - yaw_agree + eulerAngle_yaw;
//     {

//         if (yaw_agree > 180.0f) {
//             yaw_agree -= 360.0f;
//         } else if (yaw_agree < -180.0f) {
//             yaw_agree += 360.0f;
//         }

//         error_prouette_1 = -(yaw_agree - eulerAngle_yaw)

//         if (error_prouette_1 > 180.0f) {
//             error_prouette_1 -= 360.0f;
//         } else if (error_prouette_1 < -180.0f) {
//             error_prouette_1 += 360.0f;
//         }
// // 可尝试对抗积分饱和，未测试
//         //   // 每次旋转最大角度为30度
//         //  float max_rotate_angle = 30.0f;
//         //  float rotate_angle;
//         //  // 计算本次旋转的角度
//         //     if (yaw_agree > 0) {
//         //         rotate_angle = (yaw_agree < max_rotate_angle) ? yaw_agree : max_rotate_angle;
//         //     } else {
//         //     rotate_angle = (yaw_agree > -max_rotate_angle) ? yaw_agree : -max_rotate_angle;
//         //     error_prouette = rotate_angle;

//      error_prouette = error_prouette_1;////正的向右转，负的向左

//     }while (error_prouette > 5);

// }


























