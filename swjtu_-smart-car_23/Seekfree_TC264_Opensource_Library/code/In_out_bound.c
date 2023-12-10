/*
 * In_out_bound.c
 *
 *  Created on: 2022年2月18日
 *      Author: paper
 */
#include "In_out_bound.h"
#define my_abs(num) (((num)>0)?(num):-(num))


int scratch_cnt = 0;
uint8 Zebra_flag = 0;
#define white 255
#define black 0
uint8 hall_detection_flag = 0;
int in_home_hang = 0;
uint8 speed_reversal_flag = 0;
void scratch_line(void) {
    scratch_cnt = 0;
    for (uint8 hang = MT9V03X_H - 40;hang < MT9V03X_H;hang++) {
        for (uint8 lie = leftline[hang];lie < rightline[hang];lie++) {
            if (Threshold_Image[hang][lie] == white && Threshold_Image[hang][lie + 1] == black)
                scratch_cnt++;
        }
        if (scratch_cnt >= 130) {
            Zebra_flag = 1;
            break;
        }
    }
}


void Inertial_Navigation_Start(void) {
    //设置当前坐标,抓取当前坐标系为基准坐标系
    navigation.start_yaw = eulerAngle_yaw_total;
    navigation.x_cur = 0;
    navigation.y_cur = 0;
    navigation.x_start = 0 * 1; //mm
    navigation.y_start = 0 * 1; //mm
    navigation.x_set = 0 * 1;   //mm
    navigation.y_set = 0 * 1;   //mm
    // car_mode.now_mode = INERTIAL_NAVIGATION;
    // roadblock.roadblock_state = BREAKER1;
}
//惯导坐标系刷新
void Inertial_Navigation_Update(void) { //周期2ms 一秒钟500次 encoder_bdc/150*50 mm/s
    navigation.cur_yaw = eulerAngle_yaw_total - navigation.start_yaw;
    navigation.inertial_navigation_cur_distance = encoder_bdc_total;
    navigation.x_cur = navigation.x_cur + sin(navigation.cur_yaw / 180 * 3.14159f) * (encoder_bdc / 150 * 50 * 0.002);
    navigation.y_cur = navigation.y_cur + cos(navigation.cur_yaw / 180 * 3.14159f) * (encoder_bdc / 150 * 50 * 0.002);
    //计算当前坐标与目标坐标的角度
    //偏航角改变值为inertial_navigation_set_yaw_up_total
    if (car_mode.now_mode == INERTIAL_NAVIGATION) {
        if (roadblock.roadblock_state == BREAKER1 || roadblock.roadblock_state == BREAKER3 || roadblock.roadblock_state == BREAKER2 || roadblock.roadblock_state == BREAKER4) {
            //注意规定坐标系下速度反转
            if (navigation.y_set - navigation.y_cur < 0 && navigation.x_set - navigation.x_cur > 0) {
                navigation.inertial_navigation_set_yaw_update = atan((navigation.x_set - navigation.x_cur) / (navigation.y_set - navigation.y_cur)) / 3.14159f * 180 + 180;
            }
            else if (navigation.y_set - navigation.y_cur < 0 && navigation.x_set - navigation.x_cur < 0) {
                navigation.inertial_navigation_set_yaw_update = atan((navigation.x_set - navigation.x_cur) / (navigation.y_set - navigation.y_cur)) / 3.14159f * 180 - 180;
            }
            else {
                navigation.inertial_navigation_set_yaw_update = atan((navigation.x_set - navigation.x_cur) / (navigation.y_set - navigation.y_cur)) / 3.14159f * 180;
            }

            float i = navigation.inertial_navigation_set_yaw_update - navigation.inertial_navigation_set_yaw_old;
            while (i > 180) {
                i = i - 360;
                if (i < 180) {
                    break;
                }
            }
            while (i < -180) {
                i = i + 360;
                if (i > -180) {
                    break;
                }
            }

            if (roadblock.roadblock_state == BREAKER2) {
                i = 0;
            }
            navigation.inertial_navigation_set_yaw_up_total += i;


            if (navigation.inertial_navigation_set_yaw_up_total - navigation.cur_yaw > 360) {
                while (1) {
                    navigation.inertial_navigation_set_yaw_up_total -= 360;
                    if (navigation.inertial_navigation_set_yaw_up_total - navigation.cur_yaw < 360) {
                        break;
                    }
                }
            }
            else if (navigation.inertial_navigation_set_yaw_up_total - navigation.cur_yaw < -360) {
                while (1) {
                    navigation.inertial_navigation_set_yaw_up_total += 360;
                    if (navigation.inertial_navigation_set_yaw_up_total - navigation.cur_yaw > -360) {
                        break;
                    }
                }
            }
            if (navigation.inertial_navigation_set_yaw_up_total - navigation.cur_yaw < 90 && navigation.inertial_navigation_set_yaw_up_total - navigation.cur_yaw > -90) {
                speed_reversal_flag = 0;
            }
            if (navigation.inertial_navigation_set_yaw_up_total - navigation.cur_yaw > 90) {
                while (1) {
                    //
                    navigation.inertial_navigation_set_yaw_up_total -= 180;
                    if (navigation.inertial_navigation_set_yaw_up_total - navigation.cur_yaw < 90) {
                        speed_reversal_flag = 1;
                        break;
                    }
                }
            }
            if (navigation.inertial_navigation_set_yaw_up_total - navigation.cur_yaw < -90) {
                while (1) {
                    navigation.inertial_navigation_set_yaw_up_total += 180;
                    if (navigation.inertial_navigation_set_yaw_up_total - navigation.cur_yaw > -90) {
                        speed_reversal_flag = 1;
                        break;
                    }
                }
            }
            navigation.inertial_navigation_set_yaw_old = navigation.inertial_navigation_set_yaw_up_total;
        }
    }
}
int out_home_distance1 = 25000;
uint8 out_home_flag = 0;
uint8 in_home_flag = 0;
void out_home(void) {
    if (go_flag == 1) {
        if (out_home_dir == 0) {
            out_home_flag = 3;
        }
        if (out_home_flag == 0) {
            car_mode.now_mode = INERTIAL_NAVIGATION;
            navigation.inertial_navigation_set_yaw = eulerAngle_yaw_total;
            navigation.inertial_navigation_set_distance = encoder_bdc_total + out_home_distance1;
            navigation.inertial_navigation_set_yaw_up_total = eulerAngle_yaw_total;
            out_home_flag = 1;
            bdc_speed_max = 600;
        }
        else if (out_home_flag == 1) {
            if ((navigation.inertial_navigation_set_distance - navigation.inertial_navigation_cur_distance) < 100) {
                out_home_flag = 2;
                if (out_home_dir == 1)//左转
                navigation.inertial_navigation_set_yaw_up_total = eulerAngle_yaw_total - 90;
                elseif (out_home_dir == 2)//右转
                navigation.inertial_navigation_set_yaw_up_total = eulerAngle_yaw_total + 90;
            }
        }
        else if (out_home_flag == 2) {
            if (my_abs(navigation.cur_yaw - navigation.inertial_navigation_set_yaw_up_total) < 3) {
                if (car_scan_mode == 0) {
                    car_mode.now_mode = CAMERA;
                }
                else if (car_scan_mode == 1) {
                    car_mode.now_mode = ELECTROMAGNETIC;
                }
                out_home_flag = 3;
            }
        }
    }
}

void in_home(void) {
    if (((navigation.y_set - navigation.y_cur) < -1000) || (my_abs(navigation.x_set - navigation.x_cur) < 100) && go_flag == 1) {
        in_home_flag = 1;
        car_mode.now_mode = INERTIAL_NAVIGATION;
        navigation.x_set = 0;   //mm
        navigation.y_set = 0;   //mm
        bdc_speed_max = 200;
        if ((my_abs(navigation.y_set - navigation.y_cur) < 15) && (my_abs(navigation.x_set - navigation.x_cur) < 15)) {
            Buzzer(0);
            STOP_BLDC(1);
            SET_BDC_PWM(0);
            SET_BLDC_B_PWM(0);
            SET_BLDC_F_PWM(0);
        }
    }
}
// //惯导坐标系刷新
// void Inertial_Navigation_Update(void) { //周期2ms 一秒钟500次 encoder_bdc/150*50 mm/s
//     //总坐标计算，参考系为发车参考系
//     navigation.cur_yaw = eulerAngle_yaw_total - navigation.start_yaw;
//     navigation.x_cur = navigation.x_cur - sin(navigation.cur_yaw / 180 * 3.14159f) * (encoder_bdc / 150 * 50 * 0.002);
//     navigation.y_cur = navigation.y_cur + cos(navigation.cur_yaw / 180 * 3.14159f) * (encoder_bdc / 150 * 50 * 0.002);
//     //计算当前坐标与目标坐标的角度
//     if (car_mode.now_mode == INERTIAL_NAVIGATION) {
//         //回车库角度计算
//         if (navigation.y_cur - navigation.y_set < 0 && navigation.x_cur - navigation.x_set > 0) {
//             navigation.inertial_navigation_set_yaw_up = -atan((navigation.x_cur - navigation.x_set) / (navigation.y_cur - navigation.y_set)) / 3.14159f * 180;
//         }
//         else if (navigation.y_cur - navigation.y_set < 0 && navigation.x_cur - navigation.x_set < 0) {
//             navigation.inertial_navigation_set_yaw_up = -atan((navigation.x_cur - navigation.x_set) / (navigation.y_cur - navigation.y_set)) / 3.14159f * 180;
//         }
//         else {
//             navigation.inertial_navigation_set_yaw_up = -atan((navigation.x_cur - navigation.x_set) / (navigation.y_cur - navigation.y_set)) / 3.14159f * 180 - 180;
//         }
//     }
//     // //十字绕行角度计算
//     // else if (cross.crossroad_state == TURN) {
//     //     navigation.inertial_navigation_set_yaw_up = -atan((navigation.x_cur - navigation.x_set) / (navigation.y_cur - navigation.y_set)) / 3.14159f * 180

//     // }
// }
// while (navigation.inertial_navigation_set_yaw_up < 0) {
//     navigation.inertial_navigation_set_yaw_up += 360;
// }


//到达目标坐标
// if ((my_abs(navigation.y_set - navigation.y_cur) < 10) || (my_abs(navigation.x_set - navigation.x_cur) < 10)) {
//     //car_mode.now_mode = CAMERA;
//     if (go_home_flag == 1) {
//         STOP_BLDC(1);
//         Buzzer(1);
//         SET_BDC_PWM(0);
//         SET_BLDC_B_PWM(0);
//         SET_BLDC_F_PWM(0);
//         //zf_log(0, "Emergency stop");
//         Buzzer(0);
//         //Emergency_flag = 1;
//     }
// }


//=======================下面是纯电磁用的出入库函数=====================================///
Garage  Exit_Garage = { 20,20,20,20,20,20,20,20,20 };
Garage Enter_Garge = { 20,20,20,20,20,20,20,20,20 };



#define Exit_bdc_speed_low 200
#define Exit_bdc_speed_mid 800
#define Exit_yaw_1 -80      //左拐负数
// #define Exit_distance_1 55230 //32cm距离库边线

float Exit_distance_1 = 55230;


void Exit_Garage_handler(void) {

    if ((Exit_Garage.Exit_Garage_direction == LEFT || Exit_Garage.Exit_Garage_direction == RIGHT) && Exit_Garage.Exit_Garage_state != Exit_Garage_around && Exit_Garage.Exit_Garage_state != Exit_Garage_straight && Exit_Garage.Exit_Garage_state != Exit_Garage_leave) {
        Exit_Garage.Exit_Garage_state = Exit_Garage_straight;
        navigation.inertial_navigation_set_yaw = eulerAngle_yaw_total;
        car_mode.now_mode = INERTIAL_NAVIGATION;
        // navigation.inertial_navigation_set_yaw_up_total = eulerAngle_yaw_total;
    }
    else if (Exit_Garage.Exit_Garage_state == Exit_Garage_straight) {//出库先直走固定距离
        test = 2;
        //切换巡线方式为惯导
        car_mode.now_mode = INERTIAL_NAVIGATION;

        navigation.inertial_navigation_set_distance = Exit_distance_1;
        test1 = navigation.inertial_navigation_set_distance - navigation.inertial_navigation_cur_distance;
        if (my_abs(navigation.inertial_navigation_set_distance - navigation.inertial_navigation_cur_distance) < 1500.0) {
            test = 3;

            bdc_speed_max = Exit_bdc_speed_mid;
            Exit_Garage.Exit_Garage_state = Exit_Garage_around;
            navigation.inertial_navigation_set_yaw = eulerAngle_yaw_total;
            if (Exit_Garage.Exit_Garage_direction == LEFT) {//出库左拐
                navigation.inertial_navigation_set_yaw_up_total = eulerAngle_yaw_total + Exit_yaw_1;

            }
            else {//出库右拐
                navigation.inertial_navigation_set_yaw_up_total = eulerAngle_yaw_total - Exit_yaw_1;

            }

        }
    }
    else if (Exit_Garage.Exit_Garage_state == Exit_Garage_around) {    //出库拐弯未添加方向
        test = 4;
        navigation.cur_yaw = eulerAngle_yaw_total;
        if (my_abs(navigation.cur_yaw - navigation.inertial_navigation_set_yaw_up_total) > 3) {
            test1 = my_abs(navigation.cur_yaw - navigation.inertial_navigation_set_yaw_up_total);
            bdc_speed_max = Exit_bdc_speed_low;
        }
        else if (my_abs(navigation.cur_yaw - navigation.inertial_navigation_set_yaw_up_total) <= 3) {
            Exit_Garage.Exit_Garage_state = Exit_Garage_leave;
            car_mode.now_mode = ELECTROMAGNETIC;//切回电磁
            test = 0;
        }
    }
}


#define  Enter_bdc_speed_low 200
#define  Enter_bdc_speed_mid 500
#define  Enter_yaw_1 -90      //左拐负数
#define  Enter_distance_1 13000 ////识别到斑马线后多长距离转向
#define  Enter_distance_2 40000 //转向后走多远到车库

void Enter_Garage_handler(void) {
    //出库成功才入库
    if (Exit_Garage.Exit_Garage_state == Exit_Garage_leave && Zebra_flag == 1 && Enter_Garge.Enter_Garage_state != Enter_Garage_flag && Enter_Garge.Enter_Garage_state != Enter_Garge_straight
        && Enter_Garge.Enter_Garage_state != Enter_Garage_around && Enter_Garge.Enter_Garage_state != Enter_Garge_strgight_low && Enter_Garge.Enter_Garage_state != Enter_Garage_leave) {
        Enter_Garge.Enter_Garage_state = Enter_Garage_flag;
        car_mode.now_mode = INERTIAL_NAVIGATION;
        navigation.inertial_navigation_set_yaw = eulerAngle_yaw_total;//数据更新，防止检测到上次惯导的值
        // navigation.inertial_navigation_set_yaw_up_total = eulerAngle_yaw_total;
    }
    else if (Enter_Garge.Enter_Garage_state == Enter_Garage_flag) {
        test = 2;
        Enter_Garge.Enter_Garage_state = Enter_Garge_straight;
        bdc_speed_max = Enter_bdc_speed_mid;
        navigation.inertial_navigation_set_distance = encoder_bdc_total + Enter_distance_1;//识别到斑马线后多长距离转向

    }
    else if (Enter_Garge.Enter_Garage_state == Enter_Garge_straight) {//减速
        if (my_abs(navigation.inertial_navigation_set_distance - navigation.inertial_navigation_cur_distance) < 500) {
            test = 3;
            Enter_Garge.Enter_Garage_state = Enter_Garage_around;
            if (Exit_Garage.Exit_Garage_direction == LEFT) {//进库左拐
                test = 4;
                navigation.inertial_navigation_set_yaw_up_total = eulerAngle_yaw_total + Enter_yaw_1;//,负数zuo转
                rst_flag = 1;
            }
            else {//进库右拐
                navigation.inertial_navigation_set_yaw_up_total = eulerAngle_yaw_total - Enter_yaw_1;//,
                rst_flag = 1;
            }

        }
    }
    else if (Enter_Garge.Enter_Garage_state == Enter_Garage_around) {//拐弯
        if (my_abs(navigation.cur_yaw - navigation.inertial_navigation_set_yaw_up_total) > 5) {
            bdc_speed_max = Enter_bdc_speed_low;
            test = 5;
        }
        else if (my_abs(navigation.cur_yaw - navigation.inertial_navigation_set_yaw_up_total) <= 5) {
            Enter_Garge.Enter_Garage_state = Enter_Garge_strgight_low;
            navigation.inertial_navigation_set_distance = encoder_bdc_total + Enter_distance_2;//转向后走多远到车库
        }

    }
    else if (Enter_Garge.Enter_Garage_state == Enter_Garge_strgight_low) {
        bdc_speed_max = Enter_bdc_speed_mid;
        car_mode.now_mode = INERTIAL_NAVIGATION;
        test = 6;
        if (my_abs(navigation.inertial_navigation_set_distance - navigation.inertial_navigation_cur_distance) < 2000) {
            Enter_Garge.Enter_Garage_state = Enter_Garage_leave;
        }
    }
    else if (Enter_Garge.Enter_Garage_state == Enter_Garage_leave) {
        //停车
        test = 7;
        STOP_BLDC(1);
        SET_BDC_PWM(0);
        SET_BLDC_B_PWM(0);
        SET_BLDC_F_PWM(0);
    }
}


//计算两点间距离
float Distances_Calculate(float x1, float y1, float x2, float y2) {

    return sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));


}