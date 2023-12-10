/*
 * pid.c
 *
 *  Created on: 2022年1月6日
 *      Author: paper
 */
#include "pid.h"
//pid参数
        float pid_speed_Kp_left = 0.12;
        float pid_speed_Ki_left = 0;
        float pid_speed_Kd_left = 0.00;

        float pid_speed_Kp_right =  0.12;
        float pid_speed_Ki_right = 0;
        float pid_speed_Kd_right = 0.00;

        float pid_speed_left_OUT  = 0;
        float pid_speed_right_OUT = 0;
void PID_Inc_Init(PID_IncTypeDef* sptr,float kp,float ki,float kd)
{
    sptr->Ek1 = 0;        //上次偏差值初始化
    sptr->Ek2 = 0;        //上上次偏差值初始化

    sptr->Kp = kp;                //比例常数
    sptr->Ki = ki;                //积分常数
    sptr->Kd = kd;                //微分常数
    sptr->OUT = 0;
}

/************************************************
函数名称 ： PID_Inc
功    能 ： PID增量(Increment)计算
参    数 ： SetValue ------ 设置值(期望值)
            ActualValue --- 实际值(反馈值)
            PID ----------- PID数据结构
返 回 值 ： PIDInc -------- 本次PID增量(+/-)
作    者 ： strongerHuang
*************************************************/
uint8 warning = 0;
float Differential_speed_1 = 0;
PID_IncTypeDef left_wheel;
PID_IncTypeDef right_wheel;
float PID_Inc(float SetValue, float ActualValue, PID_IncTypeDef *PID)
{
  float PIDInc;                                  //位置PID
  PID->Ek = SetValue - ActualValue;
  PID->Ek2 += PID->Ek;
    // /* 设定闭环死区 */
    // if( ( PID->Ek>-8) && ( PID->Ek<8 ) )
    // {
    //     PID->Ek = 0;
    //     PID->Ek2 = 0;
    //     PID->Ek1 = 0;
    // }
    
  PIDInc = (PID->Kp * PID->Ek) + (PID->Ki * PID->Ek2) + (PID->Kd * (PID->Ek - PID->Ek1));
  PID->Ek1 = PID->Ek; 
 
  return PIDInc;
}
void Init_pid_wheel(void)
{
    //设置PID参数信息
    PID_Inc_Init(&right_wheel,p,i,d);
    PID_Inc_Init(&left_wheel,p,i,d);
}

void pid_wheel(void)//右转为正
{
    float ei = 0.10;
    // float A = 0; //差速自分配因子
    // if(yawspeed > 0 )
    // {
    //      A = 1 - 0.2*yawspeed/(Speed_Left_set+Speed_Right_set);
    // }
    // else if(yawspeed < 0 )
    // {
    //      A = 1 + 0.2*yawspeed/(Speed_Left_set+Speed_Right_set);
    // }
   
    // if(start_count<50)
    // {
    //   A = 0;
    // }
    if(encoder_all_home>=1500 )
    {
        Speed_Left = 0;
        Speed_Right = 0;
        ei = 0;
    }
    else if(In_Home_flag==1)
    {
       Speed_Right = 10    *   109.0925 /5;
       Speed_Left =  0 ;
       ei = 0;
    }
    if(start_count<=50)
    {
        ei = 0;
    }
    if(yawspeed>=80)
    {
        yawspeed = 80;

    }
    else   if(yawspeed<=-80)
    {
        yawspeed = -80;

    }
    if(go)
    {
        left_wheel.Kp = p;
        left_wheel.Ki = i;
        left_wheel.Kd = d;
        right_wheel.Kp = p;
        right_wheel.Ki = i; 
        right_wheel.Kd = d;// //1.6   -75   (45.7 + 75*x) = (45.7 - 75*x) *1.6  0.14
        left_wheel.OUT += PID_Inc(Speed_Left  +  ei*yawspeed,encoder_left,&left_wheel);
        right_wheel.OUT += PID_Inc(Speed_Right - ei*yawspeed,encoder_right,&right_wheel);
       
        //  if(yawspeed > 0 )
        // {
        //     left_wheel.OUT += PID_Inc(Speed_Left+A*yawspeed,encoder_left,&left_wheel);
        //     right_wheel.OUT += PID_Inc(Speed_Right-(1-A)*yawspeed,encoder_right,&right_wheel);
        // }
        // else if(yawspeed < 0 )
        // {
        //     right_wheel.OUT += PID_Inc(Speed_Right+A*yawspeed,encoder_left,&left_wheel);
        //     left_wheel.OUT += PID_Inc(Speed_Left-(1-A)*yawspeed,encoder_right,&right_wheel);
        // }
        // else
        // {
        //     left_wheel.OUT += PID_Inc(Speed_Left,encoder_left,&left_wheel);
        //     right_wheel.OUT += PID_Inc(Speed_Right,encoder_right,&right_wheel);
        // }
        
        // left_wheel.OUT += PID_Inc(Speed_Left,encoder_left,&left_wheel);
        // right_wheel.OUT += PID_Inc(Speed_Right,encoder_right,&right_wheel);
    }
}
float pid_speed_err_last = 0;
float pid_speed_i = 0;
float pid_speed(float pid_speed_target_val,float actual_val,float pid_speed_Kp,float pid_speed_Ki,float pid_speed_Kd)//右转为正
{
    //死区
    float SPE_DEAD_ZONE = 0;
    //积分分离值
    float SPE_INTEGRAL_START_ERR =0;
    //积分限幅
    float SPE_INTEGRAL_MAX_VAL = 3000;
    float pid_speed_output_val = 0;
     /*计算目标值与实际值的误差*/
    float pid_speed_err = pid_speed_target_val - actual_val;
    pid_speed_i += pid_speed_err;
    /* 设定闭环死区 */
    // if( (pid_speed_err>-SPE_DEAD_ZONE) && (pid_speed_err<SPE_DEAD_ZONE ) )
    // {
    //     pid_speed_err = 0;
    //     pid_speed_i = 0;
    //     pid_speed_err_last = 0;
    // }
    /*积分分离，偏差较小时才进行积分*/
    if(pid_speed_err > -SPE_INTEGRAL_START_ERR && pid_speed_err < SPE_INTEGRAL_START_ERR)
    {
        pid_speed_i += pid_speed_err;  
        /*积分范围限定，防止积分饱和*/
        if(pid_speed_i > SPE_INTEGRAL_MAX_VAL)
        {
            pid_speed_i = SPE_INTEGRAL_MAX_VAL;
        }
        else if(pid_speed_i < -SPE_INTEGRAL_MAX_VAL)
        {
            pid_speed_i = -SPE_INTEGRAL_MAX_VAL;
        }
    }   
    /*PID算法实现*/
    pid_speed_output_val = pid_speed_Kp * pid_speed_err +
                           pid_speed_Ki * pid_speed_i +
                           pid_speed_Kd *(pid_speed_err - pid_speed_err_last);
    /*误差传递*/
    pid_speed_err_last = pid_speed_err;
    /*返回当前实际值*/
    return pid_speed_output_val;
}
void Warning_STOP(void)//刹车报警
{
    go=0;
    while(1)
    {
    Buzzer(1);
    ips200_clear(RED);
    Buzzer(0);
    ips200_clear(BLUE);
    ips200_clear(RED);
    ips200_clear(BLUE);
    }
}
