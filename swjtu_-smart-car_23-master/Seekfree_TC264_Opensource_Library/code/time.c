/*
 * time.c
 *
 *  Created on: 2022年11月18日
 *      Author: paper
 */
#include "time.h"
#define DEBUG 0
float Mechanical_zero = -0.0f;
float LQ_YAW = 0;
uint8 flag_pose_1s = 0;
uint8 in_bound = 0;
uint8 time_stop = 0;
uint32 time_1s = 0;
uint8 in_bound_flag = 0;
Timer_0 Flag;
uint8 rst_flag = 0, rst_count = 0;
float servo_pwm = 625;
/*
根据机械传动公式，假设读取编码器脉冲个数的频率为500Hz，因此将可以计算出由编码器脉冲数转换为实际运行速度（米每秒）的比例系数
P u l s e 2 M P S
原文链接：https://blog.csdn.net/xht2403267701/article/details/128563878
*/
float Pulse1MPs = 1 / 512 * 32 / 68 * 64 * 0.001 * PI * 50;

void Fuse_result(void) {
    // 2ms控制
    if (Flag.T_2ms == 1) {
        //编码器电流获取
        //BLDC_Current();
        //姿态解算
        ICM_getEulerianAngles();
        //坐标计算
        Inertial_Navigation_Update();
        //参数切换
        PID_Change();
        //姿态异常急停
        if (go_flag == 1 && (eulerAngle_roll > 23 || eulerAngle_roll < -23 || eulerAngle_pitch < -23 || eulerAngle_pitch > 23)) {
            // Buzzer(1);
            STOP_BLDC(1);
            SET_BDC_PWM(0);
            SET_BLDC_B_PWM(0);
            SET_BLDC_F_PWM(0);
            //zf_log(0, "Emergency stop");
            Buzzer(0);
            Emergency_flag = 1;
            goto Emergency;
        }
        else if (go_flag == 1) {
            //左右平衡角速度环
            pidout_3f = ROLL_ANGLESPEED_LOOP(pidout_2f);
            pidout_3b = pidout_3f;
            //YAW轴平衡角速度环
            pidout_6f = BLDC_YAW_ANGLESPEED_LOOP(pidout_5f);
            pidout_6b = pidout_6f;
            //  if ( Track_Type == Wobble){      //角速度变大则保持稳定
            //     Buzzer(1);
            // SET_BLDC_F_PWM((int)(-pidout_3b));
            // SET_BLDC_B_PWM((int)(+pidout_3f));
            //  }
            //  else {
            if (rst_flag || encoder_bdc_total < 2000) {
                SET_BLDC_F_PWM((int)(-pidout_3b));
                SET_BLDC_B_PWM((int)(+pidout_3f));
            }
            else {
                SET_BLDC_F_PWM((int)pidout_6b - pidout_3b);
                SET_BLDC_B_PWM((int)pidout_6f + pidout_3f);
            }

            //  }
        }
        Flag.T_2ms = 0;
    }
    // 6ms控制
    if (Flag.T_6ms == 1) {


        Flag.T_6ms = 0;
    }
    // 10ms控制
    if (Flag.T_10ms == 1) {
        getadval();
        em_calculate();//电磁偏差值计算，出偏差值
        // Dynamic_zero_set =0 ;
        Dynamic_zero_set = Dynamic_zero_cale();//动态零点获取
        if (go_flag == 1) {
            //左右平衡角度环
            pidout_2f = ROLL_ANGLE_LOOP(pidout_1f);
            pidout_2b = pidout_2f;
            //yaw轴转向角度环
            pidout_5f = BLDC_YAW_ANGLE_LOOP(pidout_4f);
            pidout_5b = pidout_5f;
        }
        Flag.T_10ms = 0;
    }
    // 20ms控制  左右平衡飞轮速度环 按键扫描 编码器获取
    if (Flag.T_20ms == 1) {
        //编码器角度获取
        encoder_get();
        Key_scan();
        if (go_flag == 1) {
            //左右平衡速度环
            pidout_1f = ROLL_SPEED_LOOP(0);
            pidout_1b = pidout_1f;
            //yaw轴转向速度环
            pidout_4f = BLDC_YAW_SPEED_LOOP(0);
            pidout_4b = pidout_4f;
            //前后平衡角度环
            float Balance_Pwm = BDC_BANLANCE_LOOP(eulerAngle_pitch, -icm_data_gyro_x);
            //前后平衡速度环
            float Velocity_Pwm = BDC_SPEED_LOOP((float)encoder_bdc, (float)-bdc_speed);
            float PWM_out = Balance_Pwm + Velocity_Pwm;
            SET_BDC_PWM(PWM_out);
        }
        //摄像头云台
        //servo_pwm = eulerAngle_pitch * 5.5555 + 750;
        //servo_pwm = 755;
        // if (servo_pwm < 755)servo_pwm = 755;
        test1 = servo_pwm;
        SET_SERVO_PWM(servo_pwm);

        // flag_pose_1s = Attitude_stability_judgment(eulerAngle_roll, eulerAngle_pitch);
        Flag.T_20ms = 0;
    }
    if (Flag.T_100ms == 1) {
        //圆环识别重启计时
        if (ring.ring_state == RESTART) {
            ring.ring_time_count++;
            if (ring.ring_time_count > 15) {
                ring.ring_state = FINDING;
                ring.ring_time_count = 0;
            }
        }
        if (rst_flag) {
            rst_count++;
            if (rst_count > 5) {
                navigation.inertial_navigation_set_yaw = eulerAngle_yaw_total;
                rst_flag = 0;
                rst_count = 0;
            }
        }
        Flag.T_100ms = 0;
    }
    // 1000ms控制
    if (Flag.T_1s == 1) {
        debug.cross_dericetion = debug.data[0];
        AD_val_1_max = debug.data[1];
        AD_val_2_max = debug.data[2];
        AD_val_3_max = debug.data[3];
        AD_val_4_max = debug.data[4];
        AD_val_1_min = debug.data[5];
        AD_val_2_min = debug.data[6];
        AD_val_3_min = debug.data[7];
        AD_val_4_min = debug.data[8];
        ring_speed = debug.data[9];
        straight_speed = debug.data[10];
        turn_speed = debug.data[11];
        car_scan_mode = debug.data[12];
        break_mode = debug.data[13];
        out_home_dir = debug.data[14];
        gate_offset = debug.data[15];

        Flag.T_1s = 0;

    Emergency:;
    }
}