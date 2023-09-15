/*
 * motor.c
 *
 *  Created on: 2022Äê1ÔÂ6ÈÕ
 *      Author: paper
 */
#include "motor.h"
float speed_Low  =  0;
float speed_Straight = 3.0                             *   109.0925 /5;;
float speed_Turn = 0;
unsigned char go = 0;
void Motor_control_left(int32 speed)
{
if(go)
    {
               if(0<=speed)
       {
           pwm_duty(MOTOR1_PWM, speed);
           pwm_duty(MOTOR2_PWM, 0);
       }
       else
       {
           pwm_duty(MOTOR1_PWM, 0);
           pwm_duty(MOTOR2_PWM, speed);
       }
    }
    else
        {
           pwm_duty(MOTOR1_PWM, 0);
           pwm_duty(MOTOR2_PWM, 0);
       }
    
}
void Motor_control_right(int32 speed)
{
    if(go)
    {
       if(0<=speed)
       {
           pwm_duty(MOTOR3_PWM, speed);
           pwm_duty(MOTOR4_PWM, 0);
       }
       else
       {
           pwm_duty(MOTOR3_PWM, 0);
           pwm_duty(MOTOR4_PWM, speed);
       }


    }
    else
    {
           pwm_duty(MOTOR3_PWM, 0);
           pwm_duty(MOTOR4_PWM, 0);
       }
}
void control_speed(void)
{
                            
    if( back_loop_flag == 1 )
    {
        //158861
        Speed_Left  = 2.2                           *   109.0925 /5;
        Speed_Right =  2.2                              *   109.0925 /5;
        // Speed_Left  = 3.4                             *   109.0925 /5;
        // Speed_Right = 3.4                            *   109.0925 /5;
    }
    
    else if(Number_of_san_cha==1&&number_of_ring==0&&number_of_back_loop==1)
    {
        Speed_Left  = 2.1                              *   109.0925 /5;
        Speed_Right = 2.1                             *   109.0925 /5;
    }
     else if(Number_of_san_cha==1&&number_of_ring==1&&number_of_back_loop==1)
    {
        Speed_Left  = 2.4                              *   109.0925 /5;
        Speed_Right = 2.4                             *   109.0925 /5;
    }
     else if(Number_of_san_cha==1&&number_of_ring==1&&number_of_back_loop==2)
    {
        Speed_Left  = 2.2                              *   109.0925 /5;
        Speed_Right = 2.2                             *   109.0925 /5;
    }
    else if(Number_of_san_cha==1&&number_of_ring==2&&number_of_back_loop==2)
    {
        Speed_Left  = 2.2                              *   109.0925 /5;
        Speed_Right = 2.2                             *   109.0925 /5;
    }
    else if(Number_of_san_cha==2&&number_of_ring==2&&number_of_back_loop==2)
    {
        Speed_Left  = 2.3                              *   109.0925 /5;
        Speed_Right = 2.3                             *   109.0925 /5;
    }
    
    else if(!In_Home_flag)
    {
        Speed_Left  =  Speed_Left_set;
        Speed_Right = Speed_Left_set;
    }  
}

