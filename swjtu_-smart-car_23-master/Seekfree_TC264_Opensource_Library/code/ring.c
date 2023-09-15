///*
// * ring.c
// *
// *  Created on: 2023年4月18日
// *      Author: Cappi
// */
#include "ring.h"
#define my_abs(num) (((num)>0)?(num):-(num))
#define MI 150000

///******************************************************************************
//圆环函数：
//
//******************************************************************************/
Ring ring = { 0, 0, 0, 0 , 0, 0 , 0 };
Crossroad cross = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
Inertial_Navigation navigation = { 0, 0, 0, 0, 0, 0, 0 };
Inertial_Navigation Garage_navigation = { 0, 0, 0, 0, 0, 0, 0 };
Roadblock roadblock = { 0, 0, 0, 0 };
uint8 go_home_flag = 0;
#define RING_ENTRY_DISTANCE 0.4 * 150000    
#define RING_ENTRY_YAW 60 
float destination_yaw_set = 0, destination_yaw = 0;
uint8 ringflag1 = 0, ringflag2 = 0;
void ring_recognition(void) {
    if (ring.ring_state == FINDING) {
        ringflag1 = 0; ringflag2 = 0;
        if (disgy_AD_val_2 > 50 && disgy_AD_val_3 > 50) {
            ring.ring_left_white = 0;
            ring.ring_right_white = 0;
            for (uint8 i = MT9V03X_H - 1; i > endline; i--) {
                if (leftlineflag[i] == 0) {
                    ring.ring_left_white++;
                }
                if (rightlineflag[i] == 0) {
                    ring.ring_right_white++;
                }
            }
            if (ring.ring_left_white > 80 && ring.ring_right_white < 40) {
                ring.ring_state = ENTRY;
                ring.ring_yaw_start = eulerAngle_yaw_total;
                ring.ring_direction = LEFT;
            }
            else if (ring.ring_left_white < 40 && ring.ring_right_white > 80) {
                ring.ring_state = ENTRY;
                ring.ring_yaw_start = eulerAngle_yaw_total;
                ring.ring_direction = RIGHT;
            }
            else {
                ring.ring_direction = STRAIGHT;
            }
        }
        else {
            ring.ring_direction = STRAIGHT;
        }
    }


}
void ring_handler(void) {
    ring.ring_yaw_cur = eulerAngle_yaw_total - ring.ring_yaw_start;
    if (ring.ring_state == ENTRY) {
        //从上到下找白黑白跳变点
        if (ring.ring_direction == LEFT) {
            uint8 white_to_black = 0, black_to_white = 0;
            //下到上
            for (uint8 i = MT9V03X_H - 5;i > 5;i--) {
                //左到右
                for (uint8 j = 5;j < rightline[i] - 5;j++) {
                    if (Threshold_Image[i][j] == 255 && Threshold_Image[i][j + 1] == 255 && Threshold_Image[i][j + 2] == 0) {
                        white_to_black = j;
                        continue;
                    }
                }
                for (uint8 j = 5;j < rightline[i] - 5;j++) {
                    if (Threshold_Image[i][j] == 0 && Threshold_Image[i][j + 1] == 255 && Threshold_Image[i][j + 2] == 255) {
                        black_to_white = j;
                        continue;
                    }
                }
                if (white_to_black && black_to_white && white_to_black < black_to_white) {
                    ring.L_ENTRY_hang = i;
                    ring.L_ENTRY_LIE = black_to_white;
                    ring.L_ENTRY_FLAG = 1;
                    break;
                }
                // for (uint8 j = rightline[i];j > 5;j--) {
                //     if (Threshold_Image[i][j] == 255 && Threshold_Image[i][j - 1] == 0 && Threshold_Image[i][j - 2] == 0) {
                //         black_to_white = j;
                //         continue;
                //     }
                // }
                // for (uint8 j = rightline[i];j > 5;j--) {
                //     if (Threshold_Image[i][j] == 0 && Threshold_Image[i][j - 1] == 255 && Threshold_Image[i][j - 2] == 255) {
                //         white_to_black = j;
                //         continue;
                //     }
                // }
                // if (white_to_black && black_to_white && white_to_black > black_to_white) {
                //     ring.L_ENTRY_hang = i;
                //     ring.L_ENTRY_LIE = black_to_white;
                //     ring.L_ENTRY_FLAG = 1;
                //     break;
                // }
            }
            //补线
            //计算斜率
            if (ring.L_ENTRY_FLAG == 1) {
                float k = (float)(MT9V03X_W - ring.L_ENTRY_LIE) / (float)(MT9V03X_H - ring.L_ENTRY_hang), kk = 0;
                k = k * 0.85;
                for (uint8 i = MT9V03X_H - 5;i > 0;i--) {
                    rightline[i] = MT9V03X_W - 1 - kk;
                    leftline[i] = 0;
                    rightlineflag[i] = 2;
                    leftlineflag[i] = 2;
                    kk += k;
                }
            }
            //角点到达最下
            if (ring.L_ENTRY_hang > MT9V03X_H - 60 || ringflag2 == 1) {
                ringflag1 = 1;
            }
            if (my_abs(ring.ring_yaw_cur) > 40) {
                ringflag2 = 1;
            }
            if (ringflag1 && ringflag2) {
                ring.ring_state = TURN;
            }
        }
        else if (ring.ring_direction == RIGHT) {
            uint8 white_to_black = 0, black_to_white = 0;
            //下到上
            for (uint8 i = MT9V03X_H - 5;i > 5;i--) {
                //右到左
                for (uint8 j = MT9V03X_W - 5;j > leftline[i] + 5;j--) {
                    if (Threshold_Image[i][j] == 255 && Threshold_Image[i][j - 1] == 255 && Threshold_Image[i][j - 2] == 0) {
                        white_to_black = j;
                        continue;
                    }
                }
                for (uint8 j = MT9V03X_W - 5;j > leftline[i] + 5;j--) {
                    if (Threshold_Image[i][j] == 0 && Threshold_Image[i][j - 1] == 255 && Threshold_Image[i][j - 2] == 255) {
                        black_to_white = j;
                        continue;
                    }
                }
                if (white_to_black && black_to_white && white_to_black > black_to_white) {
                    ring.R_ENTRY_hang = i;
                    ring.R_ENTRY_LIE = black_to_white;
                    ring.R_ENTRY_FLAG = 1;
                    break;
                }
            }
            //补线
           //计算斜率
            if (ring.R_ENTRY_FLAG == 1) {
                float k = (float)(ring.R_ENTRY_LIE) / (float)(MT9V03X_H - ring.R_ENTRY_hang), kk = 0;
                k = k * 0.85;
                for (uint8 i = MT9V03X_H - 5;i > 0;i--) {
                    leftline[i] = 1 + kk;
                    rightline[i] = MT9V03X_W - 1;
                    rightlineflag[i] = 2;
                    leftlineflag[i] = 2;
                    kk += k;
                }
            }
            //角点到达最下
            if (ring.R_ENTRY_hang > MT9V03X_H - 60 || ringflag2 == 1) {
                ringflag1 = 1;
            }
            if (my_abs(ring.ring_yaw_cur) > 40) {
                ringflag2 = 1;
            }
            if (ringflag1 && ringflag2) {
                ring.ring_state = TURN;
            }
        }
    }
    else if (ring.ring_state == TURN) {
        if (ring.ring_direction == LEFT) {
            //找出环右边线丢线数目
            for (uint8 i = MT9V03X_H - 5;i > 5;i--) {
                if (rightlineflag[i] != 0 && rightlineflag[i - 1] == 0 && rightlineflag[i - 2] == 0) {
                    ring.L_LEAVE_hang = i;
                }
                if (ring.L_LEAVE_hang > MT9V03X_H - 60 && my_abs(ring.ring_yaw_cur) > 200) {
                    ring.ring_state = LEAVE;
                }
            }
        }
        else if (ring.ring_direction == RIGHT) {
            //找出环左边线丢线数目
            for (uint8 i = MT9V03X_H - 5;i > 5;i--) {
                if (leftlineflag[i] != 0 && leftlineflag[i - 1] == 0 && leftlineflag[i - 2] == 0) {
                    ring.R_LEAVE_hang = i;
                }
                if (ring.R_LEAVE_hang > MT9V03X_H - 60 && my_abs(ring.ring_yaw_cur) > 200) {
                    ring.ring_state = LEAVE;
                }
            }
        }
    }
    else if (ring.ring_state == LEAVE) {
        if (ring.ring_direction == LEFT) {
            for (uint8 i = MT9V03X_H - 5;i > endline;i--) {
                rightline[i] = MT9V03X_W - (endline * 2.5 + 20);
                leftline[i] = 0;
                rightlineflag[i] = 2;
                leftlineflag[i] = 2;
            }
        }
        else if (ring.ring_direction == RIGHT) {
            for (uint8 i = MT9V03X_H - 5;i > endline;i--) {
                leftline[i] = endline * 2.5 + 20;
                rightline[i] = MT9V03X_W - 1;
                rightlineflag[i] = 2;
                leftlineflag[i] = 2;
            }
        }
        if (my_abs(ring.ring_yaw_cur) > 320) {
            ring.ring_state = RESTART;
        }
    }
}


///******************************************************************************
//十字识别函数：
//
//******************************************************************************/
uint8 L_DOWN, R_DOWN, L_UP, R_UP;
void crossroad_recognition(void) {
    //清零变量
    cross.crossroad_left_white = 0;
    cross.crossroad_right_white = 0;
    cross.crossroad_left_black = 0;
    cross.crossroad_right_black = 0;
    cross.crossroad_left_black_to_white = 0;
    cross.crossroad_right_black_to_white = 0;

    if (cross.crossroad_state == FINDING) {
        for (uint8 i = MT9V03X_H - 1; i > endline; i--) {
            if (leftlineflag[i] == 0) {
                cross.crossroad_left_white++;
            }
            else {
                cross.crossroad_left_black++;
            }
            if (rightlineflag[i] == 0) {
                cross.crossroad_right_white++;
            }
            else {
                cross.crossroad_right_black++;
            }
        }
        for (uint8 i = MT9V03X_H - 20; i > endline; i--) {
            if (leftlineflag[i] != 0 && leftlineflag[i - 1] == 0) {
                for (uint8 j = i; j > endline; j--) {
                    if (leftlineflag[j] == 0) {
                        cross.crossroad_left_black_to_white++;
                    }
                }
                break;
            }
        }
        for (uint8 i = MT9V03X_H - 20; i > endline; i--) {
            if (rightlineflag[i] != 0 && rightlineflag[i - 1] == 0) {
                for (uint8 j = i; j > endline; j--) {
                    if (rightlineflag[j] == 0) {
                        cross.crossroad_right_black_to_white++;
                    }
                }
                break;
            }
        }


        //清零变量
        L_DOWN = 0, L_UP = 0, R_DOWN = 0, R_UP = 0;
        //找拐点
        for (uint8 i = MT9V03X_H - 1;i > endline;i--) {
            if ((leftline[i - 2] - leftline[i]) >= 0 && (leftline[i - 4] - leftline[i - 2]) >= 0
                && (leftline[i - 6] - leftline[i - 4]) <= 0 && (leftline[i - 8] - leftline[i - 6]) <= 0 && leftlineflag[i - 4] != 0) {
                L_DOWN = i;
                break;
            }
        }
        if (L_DOWN) {
            for (uint8 i = L_DOWN;i > endline;i--) {
                if ((leftline[i - 2] - leftline[i]) <= 0 && (leftline[i - 4] - leftline[i - 2]) <= 0
                    && (leftline[i - 6] - leftline[i - 4]) <= 0 && (leftline[i - 8] - leftline[i - 6]) <= 0 && leftlineflag[i - 8] != 0 && leftlineflag[i + 10] == 0) {
                    L_UP = i;
                    break;
                }
            }
        }
        for (uint8 i = MT9V03X_H - 1;i > endline;i--) {
            if ((rightline[i - 2] - rightline[i]) <= 0 && (rightline[i - 4] - rightline[i - 2]) <= 0
                && (rightline[i - 6] - rightline[i - 4]) >= 0 && (rightline[i - 8] - rightline[i - 6]) >= 0 && rightlineflag[i - 4] != 0) {
                R_DOWN = i;
                break;
            }
        }
        if (R_DOWN) {
            for (uint8 i = L_DOWN;i > endline;i--) {
                if ((leftline[i - 2] - leftline[i]) >= 0 && (leftline[i - 4] - leftline[i - 2]) >= 0
                    && (leftline[i - 6] - leftline[i - 4]) >= 0 && (leftline[i - 8] - leftline[i - 6]) >= 0 && rightlineflag[i - 8] != 0 && rightlineflag[i + 10] == 0) {
                    R_UP = i;
                    break;
                }
            }
        }
        //判断是否为十字
        if (cross.crossroad_right_black_to_white > 40 && cross.crossroad_left_black_to_white > 40 && debug.cross_dericetion != 0) {
            bdc_speed_max = 500;
            cross.crossroad_state = ENTRY;
            car_mode.now_mode = INERTIAL_NAVIGATION;
            navigation.inertial_navigation_set_yaw = eulerAngle_yaw_total;
            navigation.inertial_navigation_set_yaw_up_total = eulerAngle_yaw_total;
           // rst_flag = 1;
        }
    }
}
void crossroad_handler(void) {
    if (cross.crossroad_state == ENTRY) {
        if ((disgy_AD_val_4 < 10 && disgy_AD_val_1 >30) || (disgy_AD_val_4 > 30 && disgy_AD_val_1 < 10)) {
            navigation.inertial_navigation_set_yaw = eulerAngle_yaw_total;
            if (debug.cross_dericetion == 1) {
                navigation.inertial_navigation_set_yaw_up_total = eulerAngle_yaw_total - 90;
            }
            else if (debug.cross_dericetion == 2) {
                navigation.inertial_navigation_set_yaw_up_total = eulerAngle_yaw_total + 90;
            }

            cross.crossroad_state = TURN;
        }
    }
    else if (cross.crossroad_state == TURN) {
        car_mode.now_mode = INERTIAL_NAVIGATION;
        if (my_abs(navigation.cur_yaw - navigation.inertial_navigation_set_yaw_up_total) > 3) {
            bdc_speed_max = 0;
        }
        else if (my_abs(navigation.cur_yaw - navigation.inertial_navigation_set_yaw_up_total) <= 3) {
            cross.crossroad_state = FINDING;
            car_mode.now_mode = CAMERA;
        }
    }
}

#define OBSTACLES1_yaw 60
#define OBSTACLES2_yaw -90
#define OBSTACLES3_yaw 30
#define OBSTACLES1_distance (0.9f * 150000)
#define OBSTACLES2_distance (0.9f * 150000 *1.732f )
int Threshold_Image_black = 0, Threshold_Image_white = 0;
float OBSTACLES_distance = 4.5f * 150000;  //进行归一化的编码器值
void roadblock_recognition(void) {
    // dl1a_get_distance();//TOF测距
    static float RAMP_encoder = 0;

    switch ((uint8)car_scan_mode) {
    case 1://纯电磁模式
        if (roadblock.roadblock_state == NONE) {
            Threshold_Image_white = 0, Threshold_Image_black = 0;
            dl1a_get_distance();//TOF测距
            if (dl1a_distance_mm < 1200 && dl1a_distance_mm > 650)
                roadblock.roadblock_state = ROADBLock;//可能为障碍
            else if (dl1a_distance_mm < 650)
                roadblock.roadblock_state = RAMP;//坡道
            for (uint8 hang = MT9V03X_H - 5;hang > MT9V03X_H - 35;hang--) {
                for (uint8 lie = 5;lie < MT9V03X_W - 5;lie++) {
                    if (Threshold_Image[hang][lie] == 255) {
                        Threshold_Image_white++;
                    }
                    else {
                        Threshold_Image_black++;
                    }
                }
            }
            if (Threshold_Image_black > 4000) {
                roadblock.roadblock_state = BREAKER1;
            }
        }
        else if (roadblock.roadblock_state == ROADBLock) {
            dl1a_get_distance();//TOF测距
            if (dl1a_distance_mm < 500)//0.5m有障碍
                // Buzzer(1);
                navigation.inertial_navigation_set_distance = encoder_bdc_total + OBSTACLES1_distance;
            navigation.inertial_navigation_set_yaw = eulerAngle_yaw_total;
            navigation.inertial_navigation_set_yaw_up_total = eulerAngle_yaw_total + OBSTACLES1_yaw;
            // bdc_speed_max = 0;
            car_mode.now_mode = INERTIAL_NAVIGATION;
            roadblock.roadblock_state = OBSTACLES1;
            roadblock.yaw_start = eulerAngle_yaw_total;
            roadblock.distance_start = encoder_bdc_total;
        }
        // else if (roadblock.roadblock_state == RAMP) {
        //     RAMP_encoder = encoder_bdc_total;
        //     if (my_abs(RAMP_encoder - encoder_bdc_total) > 100000) {//一定距离后退出坡道恢复默认
        //         roadblock.roadblock_state = NONE;
        //     }

        // }
        break;
    case 0://摄像头模式
        //读取距离
        if (roadblock.roadblock_state == NONE) {
            dl1a_get_distance();//TOF测距
            if (dl1a_distance_mm < 1200) {
                bdc_speed_max = 800;
            }
            if (dl1a_distance_mm < 700) //0.6m内有障碍
            {
                //判断是否为路障
                if (endline > 20) {
                    // Buzzer(1);
                    navigation.inertial_navigation_set_distance = encoder_bdc_total + OBSTACLES1_distance;
                    navigation.inertial_navigation_set_yaw = eulerAngle_yaw_total;
                    navigation.inertial_navigation_set_yaw_up_total = eulerAngle_yaw_total + OBSTACLES1_yaw;
                    // bdc_speed_max = 0;
                    car_mode.now_mode = INERTIAL_NAVIGATION;
                    roadblock.roadblock_state = OBSTACLES1;
                    roadblock.yaw_start = eulerAngle_yaw_total;
                    roadblock.distance_start = encoder_bdc_total;
                }
                // //判断是否为坡道
                // else {
                //     roadblock.roadblock_state = RAMP;
                // }
            }
            else if (dl1a_distance_mm > 1000) {
                float i = (BlackEndR + BlackEndL + BlackEndM) / 3.f;
                if ((endline > 40 && my_abs(image_Error) < 20) && my_abs(BlackEndR - i) < 5 && my_abs(BlackEndL - i) < 5 && my_abs(BlackEndM - i) < 5) {
                    // rst_flag = 1;
                    navigation.inertial_navigation_set_yaw = eulerAngle_yaw_total;
                    roadblock.roadblock_state = BREAKER1;
                    rst_flag = 1;
                }
                else {
                    //roadblock.roadblock_state = NONE;
                }
            }
            Threshold_Image_white = 0, Threshold_Image_black = 0;
            //扫全局图像
            for (uint8 hang = MT9V03X_H - 5;hang > 35;hang--) {
                for (uint8 lie = 5;lie < MT9V03X_W - 5;lie++) {
                    if (Threshold_Image[hang][lie] == 255) {
                        Threshold_Image_white++;
                    }
                    else {
                        Threshold_Image_black++;
                    }
                }
            }
            if (Threshold_Image_black > 10000) {
                roadblock.roadblock_state = BREAKER1;
                rst_flag = 1;
            }
            break;
        }
    }
}
void breaker_handler(void) {
    //前往车库
    if (roadblock.roadblock_state == BREAKER1) {
        // if (rst_flag == 0) {
        car_mode.now_mode = INERTIAL_NAVIGATION;
        go_home_flag = 1;
        navigation.x_set = 0;   //mm
        navigation.y_set = 0;   //mm
        //到达目标坐标
        if ((my_abs(navigation.y_set - navigation.y_cur) < 20) && (my_abs(navigation.x_set - navigation.x_cur) < 20)) {
            navigation.inertial_navigation_set_yaw = eulerAngle_yaw_total;
            roadblock.roadblock_state = BREAKER2;
            bdc_speed_max = 0;
            //设定偏航角为发车角度
            navigation.inertial_navigation_set_yaw_up_total = navigation.start_yaw;

        }
        // }
    }
    //等待转向回正，设置目标点为计时器触发
    else if (roadblock.roadblock_state == BREAKER2) {
        Buzzer(0);

        if (my_abs(navigation.cur_yaw - navigation.inertial_navigation_set_yaw_up_total) <= 3) {
            navigation.x_set = 0;   //mm
            navigation.y_set = 300;   //mm
            roadblock.roadblock_state = BREAKER3;
            bdc_speed_max = 500;
        }
    }
    //回车库
    else if (roadblock.roadblock_state == BREAKER3) {
        Buzzer(1);
        if ((my_abs(navigation.y_set - navigation.y_cur) < 20) && (my_abs(navigation.x_set - navigation.x_cur) < 20)) {
            navigation.x_set = 0;   //mm
            navigation.y_set = 0;   //mm
            bdc_speed_max = 300;
            navigation.inertial_navigation_set_yaw = eulerAngle_yaw_total;
            // navigation.inertial_navigation_set_yaw_up_total = navigation.start_yaw + 180;
            roadblock.roadblock_state = BREAKER4;
        }
    }
    else if (roadblock.roadblock_state == BREAKER4) {
        if ((my_abs(navigation.y_set - navigation.y_cur) < 40) && (my_abs(navigation.x_set - navigation.x_cur) < 40)) {
            Buzzer(0);
            STOP_BLDC(1);
            SET_BDC_PWM(0);
            SET_BLDC_B_PWM(0);
            SET_BLDC_F_PWM(0);
        }
    }

}
void roadblock_handler(void) {
    //处理路障
    if (roadblock.roadblock_state == OBSTACLES1) {
        //切换巡线方式为惯导
        car_mode.now_mode = INERTIAL_NAVIGATION;
        if (my_abs(navigation.cur_yaw - navigation.inertial_navigation_set_yaw_up_total) > 3) {
            bdc_speed_max = 100;
        }
        else if (my_abs(navigation.cur_yaw - navigation.inertial_navigation_set_yaw_up_total) <= 3) {
            bdc_speed_max = 600;
            if ((navigation.inertial_navigation_set_distance - navigation.inertial_navigation_cur_distance) < 100) {
                roadblock.roadblock_state = OBSTACLES2;
                navigation.inertial_navigation_set_distance = encoder_bdc_total + OBSTACLES2_distance;
                navigation.inertial_navigation_set_yaw = eulerAngle_yaw_total;
                navigation.inertial_navigation_set_yaw_up_total = eulerAngle_yaw_total + OBSTACLES2_yaw;
            }
        }
    }
    //转入巡线
    else if (roadblock.roadblock_state == OBSTACLES2) {
        if (my_abs(navigation.cur_yaw - navigation.inertial_navigation_set_yaw_up_total) > 3) {
            bdc_speed_max = 100;
        }
        else if (my_abs(navigation.cur_yaw - navigation.inertial_navigation_set_yaw_up_total) <= 3) {
            bdc_speed_max = 600;
            if ((navigation.inertial_navigation_set_distance - navigation.inertial_navigation_cur_distance) < 100) {
                roadblock.roadblock_state = OBSTACLES3;
                navigation.inertial_navigation_set_yaw = eulerAngle_yaw_total;
                navigation.inertial_navigation_set_yaw_up_total = eulerAngle_yaw_total + OBSTACLES3_yaw;
            }
        }
    }
    //回正
    else if (roadblock.roadblock_state == OBSTACLES3) {
        if (my_abs(navigation.cur_yaw - navigation.inertial_navigation_set_yaw_up_total) > 3) {
            bdc_speed_max = 100;
        }
        else if (my_abs(navigation.cur_yaw - navigation.inertial_navigation_set_yaw_up_total) <= 3) {
            roadblock.roadblock_state = NONE;
            car_mode.now_mode = CAMERA;
            rst_flag = 1;
        }
    }
}








//===================================纯电磁圆环函数===============================

//纯电磁识别圆环
void ring_recognition_EM(void) {

    if (ring.ring_state == FINDING && ((disgy_AD_val_2 + disgy_AD_val_3) > 150) && ((disgy_AD_val_1 + disgy_AD_val_2 + disgy_AD_val_3 + disgy_AD_val_4) > 180)) {
        ring.ring_state = ENTRY;
        ring.ring_yaw_start = eulerAngle_yaw_total;
        test1 = 2;
    }
    //     if(ring.ring_state == ENTRY)
    // {
    //     car_mode.last_mode = car_mode.now_mode;
    //     car_mode.now_mode = INERTIAL_NAVIGATION;
    // }else if (ring.ring_state == TURN)
    // {
    // car_mode.now_mode = car_mode.last_mode;
    // navigation.inertial_navigation_set_yaw_up_total = eulerAngle_yaw_total ;
    // } 
    if (ring.ring_state == TURN) {

        ring.ring_state = FINDING;
    }





}

//纯电磁过圆环
void ring_handler_EM(void) {
    // ring.ring_yaw_cur = eulerAngle_yaw_total - ring.ring_yaw_start;

    if (ring.ring_state == ENTRY) {

        if (((disgy_AD_val_1 + disgy_AD_val_2 + disgy_AD_val_3 + disgy_AD_val_4) < 120)) {
            ring.ring_state = TURN;
        }
    }
    // else if (ring.ring_state == TURN) {

    //     ring.ring_yaw_cur = eulerAngle_yaw_total - ring.ring_yaw_start;

    //     if (((disgy_AD_val_2 + disgy_AD_val_3 + disgy_AD_val_4) > 210 && disgy_AD_val_1 < 70) ||
    //     ((disgy_AD_val_2 + disgy_AD_val_3 + disgy_AD_val_1) > 210 && disgy_AD_val_4 < 70))
    //     {
    //     ring.ring_state = LEAVE;
    //     }

    // }else if( ring.ring_state == LEAVE && my_abs(ring.ring_yaw_cur) >290){

    //     ring.ring_state = RESTART;
    //     test1 = 5;


    // }
}

