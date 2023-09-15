/*
 * pid.c
 *
 *  Created on: 2022年11月18日
 *      Author: paper
 */
#include "pid.h"
#include "protocol.h"
 ///150000 -> 1m
#define my_abs(num) (((num)>0)?(num):-(num))

PID_IncTypeDef roll_angle_wheel;
PID_IncTypeDef roll_anglespeed_wheel;
PID_IncTypeDef roll_speed_wheel;
PID_IncTypeDef bldc_b_wheel, bldc_f_wheel;
PID_IncTypeDef bdc_speed_wheel;
PID_IncTypeDef adc_pid;
PID_IncTypeDef roll_bdc_banlance_wheel;
PID_IncTypeDef bldc_yaw_anglespeed_wheel;
PID_IncTypeDef bldc_yaw_angle_wheel;
PID_IncTypeDef bldc_yaw_angle_wheel_Straight;
PID_IncTypeDef bldc_yaw_speed_wheel;
PID_IncTypeDef bldc_lq_yaw_wheel;
PID_IncTypeDef ROLL_Dynamic_zero_wheel;

Car_mode car_mode;
float ring_speed = 0;
float straight_speed = 0;
float turn_speed = 0;


float pidout_1b = 0, pidout_2b = 0, pidout_3b = 0, pidout_4b = 0, pidout_5b = 0, pidout_6b = 0,
pidout_1f = 0, pidout_2f = 0, pidout_3f = 0, pidout_4f = 0, pidout_5f = 0, pidout_6f = 0;


void PID_Change(void) // 切换参数
{
    static uint32 time_2ms = 0;
    if (car_mode.last_mode != car_mode.now_mode) {
        //  Buzzer(1);
        PID_clear(&bldc_yaw_speed_wheel);
        PID_clear(&bldc_yaw_angle_wheel);
        PID_clear(&bldc_yaw_anglespeed_wheel);
        car_mode.last_mode = car_mode.now_mode;
        //Buzzer(0);
    }
    //参数更新
    navigation.inertial_navigation_cur_distance = encoder_bdc_total;
    navigation.cur_yaw = eulerAngle_yaw_total;
    if (car_mode.now_mode == CAMERA && cross.crossroad_state == FINDING) {
        BLDC_YAW_ANGLE_Value = image_Error; //摄像头循迹偏差
        switch (Track_Type)//摄像头循迹下->赛道类型改变速度
        {
        case Ring_in_out:  //进出圆环速度改变
            //              Buzzer(1);
            bdc_speed_max = ring_speed;//
            bldc_yaw_speed_wheel.Kp = -0.20f;
            bldc_yaw_angle_wheel.Kp = 0.08f; // 0.12f;
            bldc_yaw_angle_wheel.Kd = 0.02f;
            bldc_yaw_anglespeed_wheel.Kp = 3500;//7300
            break;
        case Ring_turn:
            bdc_speed_max = ring_speed;
            break;
        case Straight:  //直道
            Buzzer(0);
            bdc_speed_max = straight_speed;//
            bldc_yaw_speed_wheel.Kp = -0.30f;
            bldc_yaw_angle_wheel.Kp = 0.08f;//  0.12f;
            bldc_yaw_angle_wheel.Kd = 0.02f;
            bldc_yaw_anglespeed_wheel.Kp = 3500;//7300
            break;
        default://弯道
            bdc_speed_max = turn_speed;//直1688和1350
            // Buzzer(1);
            bldc_yaw_speed_wheel.Kp = -0.20f;
            bldc_yaw_angle_wheel.Kp = 0.08f;//  0.12f;
            bldc_yaw_angle_wheel.Kd = 0.02f;
            bldc_yaw_anglespeed_wheel.Kp = 3500;//7300
            break;
        }
    }
    else if (car_mode.now_mode == ELECTROMAGNETIC) {
        //正常电磁循迹
        coefficient_L.incline = 0.00;//电磁偏差加权系数
        coefficient_L.direct = 1.0;
        BLDC_YAW_ANGLE_Value = Error_Em.adc_sum; //电磁循迹偏差
        bdc_speed_max = 800;
        bldc_yaw_speed_wheel.Kp = -0.0021f;
        bldc_yaw_angle_wheel.Kp = 20.5;
        bldc_yaw_angle_wheel.Ki = 1.6;
        bldc_yaw_anglespeed_wheel.Kp = 8500;//7300

    }
    else if (car_mode.now_mode == INERTIAL_NAVIGATION) {

        if (roadblock.roadblock_state == BREAKER1) {
            if (my_abs(navigation.cur_yaw - navigation.inertial_navigation_set_yaw_up_total) > 3) {
                bdc_speed_max = 400;
                Buzzer(1);
            }
            else if ((my_abs(navigation.y_set - navigation.y_cur) < 500) && (my_abs(navigation.x_set - navigation.x_cur) < 500)) {
                Buzzer(0);
                bdc_speed_max = 500;
            }
            else if ((my_abs(navigation.y_set - navigation.y_cur) < 2500) && (my_abs(navigation.x_set - navigation.x_cur) < 2500)) {
                Buzzer(0);
                bdc_speed_max = 1200;
            }
            else {
                Buzzer(1);
                bdc_speed_max = 1800;
            }
        }


        // BLDC_YAW_ANGLE_Value = (navigation.cur_yaw - navigation.inertial_navigation_set_yaw); 
        BLDC_YAW_ANGLE_Value = -(navigation.cur_yaw - navigation.inertial_navigation_set_yaw);

        // // BLDC_YAW_ANGLE_Value = (navigation.cur_yaw - navigation.inertial_navigation_set_yaw); 
        // BLDC_YAW_ANGLE_Value = -(navigation.cur_yaw - navigation.inertial_navigation_set_yaw);
        bldc_yaw_speed_wheel.Kp = -0.00f;
        bldc_yaw_angle_wheel.Kp = 0.601f;
        bldc_yaw_anglespeed_wheel.Kp = 10000;

    }
    if (rst_flag) {
        bdc_speed_max = 300;
    }
    //斜坡函数
    if (encoder_bdc_total < 2000) {
        bdc_speed_max = 80;
    }
    if (speed_reversal_flag == 0) {
        if (bdc_speed < bdc_speed_max) {
            bdc_speed += 1.2f;
        }
        else if (bdc_speed > bdc_speed_max) {
            bdc_speed -= 1.9f;
        }
    }
    else {
        if (bdc_speed > -bdc_speed_max) {
            bdc_speed -= 1.2f;
        }
        else if (bdc_speed < -bdc_speed_max) {
            bdc_speed += 1.9f;
        }
    }

    if (car_mode.now_mode == INERTIAL_NAVIGATION && rst_flag == 0) {
        if (navigation.inertial_navigation_set_yaw < navigation.inertial_navigation_set_yaw_up_total) {
            navigation.inertial_navigation_set_yaw += 0.04f;
        }
        else if (navigation.inertial_navigation_set_yaw > navigation.inertial_navigation_set_yaw_up_total) {
            navigation.inertial_navigation_set_yaw -= 0.04f;
        }

    }
    if (Dynamic_zero < Dynamic_zero_set)//10ms周期
        Dynamic_zero += 0.0040f;
    else if (Dynamic_zero > Dynamic_zero_set)
        Dynamic_zero -= 0.0040f;
}
void PID_Inc_Init(PID_IncTypeDef* sptr, float kp, float ki, float kd) {
    sptr->Ek1 = 0;        //上次偏差值初始化
    sptr->Ek2 = 0;        //上上次偏差值初始化
    sptr->Kp = kp;                //比例常数
    sptr->Ki = ki;                //积分常数
    sptr->Kd = kd;                //微分常数
    sptr->OUT = 0;
}



float Positional_PID(PID_IncTypeDef* PID, float SetValue, float ActualValue, float Max_I) {
    // 野火上位机配置理想值用的一段
    // if (memcmp(&PID, &roll_speed_wheel, sizeof(PID_IncTypeDef)) != 0)
    //     PID->target_val = SetValue;
    float PIDInc;
    PID->Ek = SetValue - ActualValue;
    PID->Ek2 += PID->Ek;

    if (PID->Ek2 > Max_I)  PID->Ek2 = Max_I;
    if (PID->Ek2 < -Max_I) PID->Ek2 = -Max_I;

    PIDInc = (PID->Kp * PID->Ek) +
        (PID->Ki * PID->Ek2) +
        (PID->Kd * (PID->Ek - PID->Ek1));
    PID->Ek1 = PID->Ek;
    return PIDInc;
}


//pid输出清零函数
void PID_clear(PID_IncTypeDef* sptr) {
    sptr->Ek1 = 0;        //上次偏差值初始化
    sptr->Ek2 = 0;        //上上次偏差值初始化
    //    sptr->OUT = 0;
}

/**
  * @brief  PID参数初始化
  *	@note 	无
  * @retval 无
  */
void PID_param_init(void) {
    // 初始化平衡环角度环

    // 初始化平衡环角度环 五月前参数
//    PID_Inc_Init(&roll_speed_wheel, -0.025, 0.0000, 0.0000f);//0.0025
//    PID_Inc_Init(&roll_angle_wheel, 1.1f, 0, 1.5f);    //
//    PID_Inc_Init(&roll_anglespeed_wheel, 10000, 0, 50);  //

    // 初始化平衡环角度环 乐师参数
//    PID_Inc_Init(&roll_speed_wheel, -0.035f, 0.0000, 0.0032f);
//    PID_Inc_Init(&roll_angle_wheel,   1.3f, 0, 0.980f);    // 8   1.5   0   8
//    PID_Inc_Init(&roll_anglespeed_wheel, 20000, 0, 200);  // 10000
//参数7.12
    if (car_scan_mode == 1) {///纯电磁参数
        PID_Inc_Init(&bdc_speed_wheel, 2.9, 2.9 / 800, 0);
        PID_Inc_Init(&roll_bdc_banlance_wheel, 350.0f, 0.0f, 1800.0f);

        PID_Inc_Init(&roll_speed_wheel, -0.016, 0.00, 0.00f);//注意roll_speed_wheel.kp<0
        PID_Inc_Init(&roll_angle_wheel, 1.85f, 0, 3.5f);
        PID_Inc_Init(&roll_anglespeed_wheel, 21000, 0, 12000);  // 10000
    }
    else {
        PID_Inc_Init(&roll_speed_wheel, -0.014, 0.00, 0.00f);//注意roll_speed_wheel.kp<0
        PID_Inc_Init(&roll_angle_wheel, 1.65f, 0, 4.1f);
        PID_Inc_Init(&roll_anglespeed_wheel, 18000, 0, 14000);  // 10000

        PID_Inc_Init(&bdc_speed_wheel, 3.1f, 3.1f / 800, 0);
        PID_Inc_Init(&roll_bdc_banlance_wheel, 320.0f, 0.0f, 1800.0f);
    }


    PID_Inc_Init(&bldc_yaw_speed_wheel, -0.004f, 0.0f, 0.0f);
    PID_Inc_Init(&bldc_yaw_angle_wheel, 65.0f, 0.00f, 0.0f);
    PID_Inc_Init(&bldc_yaw_anglespeed_wheel, 5000.0f, 0.0f, 0.0f);


    //     PID_Inc_Init(&bldc_lq_yaw_wheel, -1800.00f, -0.0f, -0.0f); //1200 10

    //初始化adc电磁pid
    PID_Inc_Init(&adc_pid, 180.0f, 0.0f, 10.0f);

#if PID_ASSISTANT_EN
    {
        float pid_temp[3] = { 0 };

        //        /*给通道1发送roll_speed_wheel--PID值*/
        //        pid_temp[0] = roll_anglespeed_wheel.Kp;
        //        pid_temp[1] = roll_anglespeed_wheel.Ki;
        //        pid_temp[2] = roll_anglespeed_wheel.Kd;
        //        set_computer_value(SEND_P_I_D_CMD, CURVES_CH1, &pid_temp, 3);
        //
        //        /*给通道2发送roll_angle_wheel--PID值*/
        //        pid_temp[0] = roll_angle_wheel.Kp;
        //        pid_temp[1] = roll_angle_wheel.Ki;
        //        pid_temp[2] = roll_angle_wheel.Kd;
        //        set_computer_value(SEND_P_I_D_CMD, CURVES_CH2, &pid_temp, 3);

                /*给通道1发送roll_speed_wheel--PID值*/
        pid_temp[0] = bldc_yaw_speed_wheel.Kp;
        pid_temp[1] = bldc_yaw_speed_wheel.Ki;
        pid_temp[2] = bldc_yaw_speed_wheel.Kd;
        set_computer_value(SEND_P_I_D_CMD, CURVES_CH1, &pid_temp, 3);

        /*给通道2发送roll_angle_wheel--PID值*/
        pid_temp[0] = bldc_yaw_angle_wheel.Kp;
        pid_temp[1] = bldc_yaw_angle_wheel.Ki;
        pid_temp[2] = bldc_yaw_angle_wheel.Kd;
        set_computer_value(SEND_P_I_D_CMD, CURVES_CH2, &pid_temp, 3);


        /*给通道3发送roll_speed_wheel--PID值*/
        pid_temp[0] = roll_speed_wheel.Kp;
        pid_temp[1] = roll_speed_wheel.Ki;
        pid_temp[2] = roll_speed_wheel.Kd;
        set_computer_value(SEND_P_I_D_CMD, CURVES_CH3, &pid_temp, 3);

        /*给通道4发送roll_bdc_banlance_wheel--PID值*/
        pid_temp[0] = roll_bdc_banlance_wheel.Kp;
        pid_temp[1] = roll_bdc_banlance_wheel.Ki;
        pid_temp[2] = roll_bdc_banlance_wheel.Kd;
        set_computer_value(SEND_P_I_D_CMD, CURVES_CH4, &pid_temp, 3);

        /*给通道5发送roll_bdc_banlance_wheel--PID值*/
        pid_temp[0] = bdc_speed_wheel.Kp;
        pid_temp[1] = bdc_speed_wheel.Ki;
        pid_temp[2] = bdc_speed_wheel.Kd;
        set_computer_value(SEND_P_I_D_CMD, CURVES_CH5, &pid_temp, 3);
    }
#endif
}


/**
  * @brief  设置目标值
  * @param  val		目标值
  *	@note 	无
  * @retval 无
  */
void set_pid_target(PID_IncTypeDef* pid, float temp_val) {
    pid->target_val = temp_val;    // 设置当前的目标值
}

/**
  * @brief  获取目标值
  * @param  无
  *	@note 	无
  * @retval 目标值
  */
float get_pid_target(PID_IncTypeDef* pid) {
    return pid->target_val;    // 获取当前的目标值
}


/**
    * @brief  设置比例、积分、微分系数
    * @param  p：比例系数 P
    * @param  i：积分系数 i
    * @param  d：微分系数 d
    *	@note 	无
    * @retval 无
    */
void set_p_i_d(PID_IncTypeDef* pid, float p, float i, float d) {
    pid->Kp = p;    // 设置比例系数 P
    pid->Ki = i;    // 设置积分系数 I
    pid->Kd = d;    // 设置微分系数 D
}



