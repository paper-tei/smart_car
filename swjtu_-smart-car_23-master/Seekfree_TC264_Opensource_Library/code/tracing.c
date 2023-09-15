/*
 * tracing.c
 *
 *  Created on: 2022年11月18日
 *      Author: paper
 */
#include "tracing.h"
 /*#define MT9V03X_W               (188)                                           // 图像宽度     范围 [1-752]
   #define MT9V03X_H               (120)                                           // 图像高度     范围 [1-480]*/
#define MT9V03X_BREAK (1)                                     // 图像舍弃边界
   // #define Tracing_Time_all   (250)   //边线最大长度
#define local_thres_ERROR (-3)                                // 避免强行分割
int Tracing_Time_L = 0, Tracing_Time_R = 0, Tracing_Time = 0; // 当前图像巡线变更次数
unsigned char front_thres = 0, front_left_thres = 0, front_right_thres = 0, Tracing_Width = 45;
float left_line[Tracing_Time_all][2];
float right_line[Tracing_Time_all][2];
float left_line_Triangular_filtering[Tracing_Time_all][2];
float right_line_Triangular_filtering[Tracing_Time_all][2];
//左右边线等距采样
float left_line_Equidistant_sampling[Tracing_Time_all][2];
float right_line_Equidistant_sampling[Tracing_Time_all][2];
float left_line_angle[Tracing_Time_all];
float right_line_angle[Tracing_Time_all];
float left_line_angle_out[Tracing_Time_all];
float right_line_angle_out[Tracing_Time_all];
float left_line_middle[Tracing_Time_all][2];
float right_line_middle[Tracing_Time_all][2];
int bottom_grayscale_average = 128; // 底部二值化阈值
// 自适应二值化
unsigned char Adaptive_Threshold(unsigned char x, unsigned char y) {
    // 遍历该点周围
    if (x <= 3 || y <= 3 || x >= RESULT_COL - 4 || y >= RESULT_COL - 4) {
        int point_grayscale_count = 0, dx, dy;
        for (dx = -1; dx <= 1; dx++) {
            for (dy = -1; dy <= 1; dy++) {
                point_grayscale_count += ImageUsed[x + dx][y + dy];
            }
        }
        return (unsigned char)(point_grayscale_count / 9.);
    }
    else {
        int point_grayscale_count = 0, dx, dy;
        for (dx = -3; dx <= 3; dx++) {
            for (dy = -3; dy <= 3; dy++) {
                point_grayscale_count += ImageUsed[x + dx][y + dy];
            }
        }
        return (unsigned char)(point_grayscale_count / 49.);
    }
}
void Leftline_Tracing(void) {
    // 清零
    for (int i = 0; i < Tracing_Time_L; i++) {
        left_line[i][0] = 0;
        left_line[i][1] = 0;
        left_line_Triangular_filtering[i][0] = 0;
        left_line_Triangular_filtering[i][1] = 0;
        left_line_Equidistant_sampling[i][0] = 0;
        left_line_Equidistant_sampling[i][1] = 0;
        left_line_angle[i] = 0;
        left_line_angle_out[i] = 0;
        left_line_middle[i][0] = 0;
        left_line_middle[i][1] = 0;
    }
    // 起始点判断
    unsigned char bottom_left = MT9V03X_W_IP * 1 / 2 - 1;
    // 1.如果底部坐标中点为白
    // if(mt9v03x_image_Tracing[MT9V03X_H-MT9V03X_BREAK][bottom_left-MT9V03X_BREAK/2] > bottom_grayscale_average)
    {
        for (; bottom_left > MT9V03X_BREAK + 1; bottom_left--) {
            if (ImageUsed[MT9V03X_H_IP - MT9V03X_BREAK][bottom_left - 1] < bottom_grayscale_average)
                break;
        }

    } // 如果底部坐标中点为黑
    // else
    // {
    //     for(;bottom_left>MT9V03X_BREAK;bottom_left--)
    //     {
    //         if(mt9v03x_image_Tracing[MT9V03X_H-MT9V03X_BREAK][bottom_left-MT9V03X_BREAK/2] > bottom_grayscale_average + 80)
    //         break;
    //     }
    //     for(;bottom_left>MT9V03X_BREAK;bottom_left--)
    //     {
    //         if(mt9v03x_image_Tracing[MT9V03X_H-MT9V03X_BREAK][bottom_left-MT9V03X_BREAK/2] < bottom_grayscale_average)
    //         break;
    //     }
    // }

    if (ImageUsed[MT9V03X_H_IP - MT9V03X_BREAK][bottom_left] > bottom_grayscale_average) {
        // 开始左迷宫走线
        // 向x,y移动的坐标数,目前的方向
        int dir_dx = 0, dir_dy = 0, dir_turn = 0, local_thres = 0, Absolute_direction = 0;
        for (Tracing_Time_L = 0; Tracing_Time_L < Tracing_Time_all; Tracing_Time_L++) {
            // 本地点的x,y坐标
            unsigned char x = MT9V03X_H_IP - 2 + dir_dx;
            unsigned char y = bottom_left + dir_dy;

            // 探测是否越界
            //if (x <= 0 || x >= MT9V03X_H_IP - 1 || y > MT9V03X_W_IP / 2 || y <= 0 || Absolute_direction > 3)
            if (x <= 0 || x >= MT9V03X_H_IP - 1 || y >= MT9V03X_W_IP - 1 || y <= 0 || Absolute_direction > 3)
                break;
            // 探测本地点的自适应阈值
            local_thres = Adaptive_Threshold(x, y) + local_thres_ERROR;
            // 探测本地点的前方点和前方左侧点的阈值,根据方向分别提取
            // 1.方向向上
            if (dir_turn == 0) {
                front_thres = ImageUsed[x - 1][y];
                front_left_thres = ImageUsed[x - 1][y - 1];
            }
            // 2.方向向右
            else if (dir_turn == 1) {
                front_thres = ImageUsed[x][y + 1];
                front_left_thres = ImageUsed[x - 1][y + 1];
            }
            // 3.方向向下
            else if (dir_turn == 2) {
                front_thres = ImageUsed[x + 1][y];
                front_left_thres = ImageUsed[x + 1][y + 1];
            }
            // 4.方向向左
            else if (dir_turn == 3) {
                front_thres = ImageUsed[x][y - 1];
                front_left_thres = ImageUsed[x + 1][y - 1];
            }
            // 左手巡线算法,执行转向和位移
            if (front_thres <= local_thres) { // 如果前方为黑,右转,不做位移
                dir_turn = (dir_turn + 1) % 4;
                Absolute_direction++;
            }
            else if (front_left_thres <= local_thres) { // 如果前左方为黑,且前方为白,直走,位移1位
                if (dir_turn == 0) {
                    dir_dx += -1;
                    dir_dy += 0;
                }
                else if (dir_turn == 1) {
                    dir_dx += 0;
                    dir_dy += 1;
                }
                else if (dir_turn == 2) {
                    dir_dx += 1;
                    dir_dy += 0;
                }
                else if (dir_turn == 3) {
                    dir_dx += 0;
                    dir_dy += -1;
                }
                Absolute_direction = 0;
                // dir_turn = dir_turn;
            }
            else { // 如果前左方为白,且前方为白,左走,连续移动
                if (dir_turn == 0) {
                    dir_dx += -1;
                    dir_dy += -1;
                }
                else if (dir_turn == 1) {
                    dir_dx += -1;
                    dir_dy += 1;
                }
                else if (dir_turn == 2) {
                    dir_dx += 1;
                    dir_dy += 1;
                }
                else if (dir_turn == 3) {
                    dir_dx += 1;
                    dir_dy += -1;
                }
                dir_turn = (dir_turn + 3) % 4;
                Absolute_direction = 0;
            }
            // 记录当前点入左边线数组
            left_line[Tracing_Time_L][0] = x;
            left_line[Tracing_Time_L][1] = y;
        } //<---该循环是全图扫线循环
    }
}
void Rightline_Tracing(void) {
    // 清零
    for (int i = 0; i < Tracing_Time_R; i++) {
        right_line[i][0] = 0;
        right_line[i][1] = 0;
        right_line_Triangular_filtering[i][0] = 0;
        right_line_Triangular_filtering[i][1] = 0;
        right_line_Equidistant_sampling[i][0] = 0;
        right_line_Equidistant_sampling[i][1] = 0;
        right_line_angle[i] = 0;
        right_line_angle_out[i] = 0;
        right_line_middle[i][0] = 0;
        right_line_middle[i][1] = 0;
    }
    // 起始点判断
    unsigned char bottom_left = MT9V03X_W_IP * 1 / 2 + 1;
    // 1.如果底部坐标中点为白
    // if(mt9v03x_image_Tracing[MT9V03X_H-MT9V03X_BREAK][bottom_left-MT9V03X_BREAK/2] > bottom_grayscale_average){
    for (; bottom_left < MT9V03X_W_IP - MT9V03X_BREAK - 1; bottom_left++) {
        if (ImageUsed[MT9V03X_H_IP - MT9V03X_BREAK - 1][bottom_left + 1] < bottom_grayscale_average)
            break;
    }
    //}//如果底部坐标中点为黑
    // else
    // {
    //     for(;bottom_left>MT9V03X_BREAK;bottom_left--)
    //     {
    //         if(mt9v03x_image_Tracing[MT9V03X_H-MT9V03X_BREAK][bottom_left-MT9V03X_BREAK/2] > bottom_grayscale_average + 80)
    //         break;
    //     }
    //     for(;bottom_left>MT9V03X_BREAK;bottom_left--)
    //     {
    //         if(mt9v03x_image_Tracing[MT9V03X_H-MT9V03X_BREAK][bottom_left-MT9V03X_BREAK/2] < bottom_grayscale_average)
    //         break;
    //     }
    // }
    if (ImageUsed[MT9V03X_H_IP - MT9V03X_BREAK - 1][bottom_left] > bottom_grayscale_average) {
        // 开始右迷宫走线
        // 向x,y移动的坐标数,目前的方向
        int dir_dx = 0, dir_dy = 0, dir_turn = 0, local_thres = 0, Absolute_direction = 0;
        for (Tracing_Time_R = 0; Tracing_Time_R < Tracing_Time_all; Tracing_Time_R++) {
            // 本地点的x,y坐标
            unsigned char x = MT9V03X_H_IP - 2 + dir_dx;
            unsigned char y = bottom_left + dir_dy;
            // 探测是否越界
            //if (x <= 0 || x >= MT9V03X_H_IP - 1 || y < MT9V03X_W_IP / 2 || y >= MT9V03X_W_IP - 1 || Absolute_direction > 3)
            if (x <= 0 || x >= MT9V03X_H_IP - 1 || y <= 0 || y >= MT9V03X_W_IP - 1 || Absolute_direction > 3)
                break;
            // 探测本地点的自适应阈值
            local_thres = Adaptive_Threshold(x, y) + local_thres_ERROR;
            // 探测本地点的前方点和前方左侧点的阈值,根据方向分别提取
            // 1.方向向上
            if (dir_turn == 0) {
                front_thres = ImageUsed[x - 1][y];
                front_right_thres = ImageUsed[x - 1][y + 1];
            }
            // 2.方向向左
            else if (dir_turn == 1) {
                front_thres = ImageUsed[x][y - 1];
                front_right_thres = ImageUsed[x - 1][y - 1];
            }
            // 3.方向向下
            else if (dir_turn == 2) {
                front_thres = ImageUsed[x + 1][y];
                front_right_thres = ImageUsed[x + 1][y - 1];
            }
            // 4.方向向右
            else if (dir_turn == 3) {
                front_thres = ImageUsed[x][y + 1];
                front_right_thres = ImageUsed[x + 1][y + 1];
            }
            // 右手巡线算法,执行转向和位移
            if (front_thres <= local_thres) { // 如果前方为黑,左转,不做位移
                dir_turn = (dir_turn + 1) % 4;
                Absolute_direction++;
            }
            else if (front_right_thres <= local_thres) { // 如果前右方为黑,且前方为白,直走,位移1位
                if (dir_turn == 0) {
                    dir_dx += -1;
                    dir_dy += 0;
                }
                else if (dir_turn == 1) {
                    dir_dx += 0;
                    dir_dy += -1;
                }
                else if (dir_turn == 2) {
                    dir_dx += 1;
                    dir_dy += 0;
                }
                else if (dir_turn == 3) {
                    dir_dx += 0;
                    dir_dy += 1;
                }
                Absolute_direction = 0;
                // dir_turn = dir_turn;
            }
            else { // 如果前右方为白,且前方为白,右走,连续移动
                if (dir_turn == 0) {
                    dir_dx += -1;
                    dir_dy += 1;
                }
                else if (dir_turn == 1) {
                    dir_dx += -1;
                    dir_dy += -1;
                }
                else if (dir_turn == 2) {
                    dir_dx += 1;
                    dir_dy += -1;
                }
                else if (dir_turn == 3) {
                    dir_dx += 1;
                    dir_dy += 1;
                }
                dir_turn = (dir_turn + 3) % 4;
                Absolute_direction = 0;
            }
            // 记录当前点入左边线数组
            right_line[Tracing_Time_R][0] = x;
            right_line[Tracing_Time_R][1] = y;
        } //<---该循环是全图扫线循环
    }
}

void Bottom_Tracing(void) {
    // 寻找截止行
    // 首先获得一个底部的平均值
    unsigned char i;
    unsigned int bottom_grayscale_count;
    for (i = MT9V03X_BREAK; i < MT9V03X_W_IP - MT9V03X_BREAK - 2; i++) {
        bottom_grayscale_count += ImageUsed[MT9V03X_H_IP - MT9V03X_BREAK][i];
    }
    bottom_grayscale_average = bottom_grayscale_count / (MT9V03X_W_IP - MT9V03X_BREAK * 2) - 20;
    if (bottom_grayscale_average < 10) {
        bottom_grayscale_average = 10;
    }
    Leftline_Tracing();
    Rightline_Tracing();
    Tracing_Time = ((Tracing_Time_L < Tracing_Time_R) ? Tracing_Time_L : Tracing_Time_R);
}
