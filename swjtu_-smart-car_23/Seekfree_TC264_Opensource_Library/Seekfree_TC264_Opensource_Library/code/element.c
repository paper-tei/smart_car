#include "element.h"
#define Tracing_Time_OK 5 //正常边线最小阈值

unsigned char left_line_OK_flag = FALSE;
unsigned char right_line_OK_flag = FALSE;

float my_abs(float x) {
    // 返回x的绝对值，用三元运算符
    return x < 0 ? -x : x;
}

void line_flag_find(void) {
    //首先判断边线是否正常
    left_line_OK_flag = FALSE;
    right_line_OK_flag = FALSE;
    if (Tracing_Time_L > Tracing_Time_OK) {
        int x = 0;
        for (int i = 0;i < Tracing_Time_L;i++) {
            x += left_line_Equidistant_sampling[i][1];
        }
        if (x / Tracing_Time_L > Tracing_Time_OK) {
            left_line_OK_flag = TRUE;
        }
    }
    if (Tracing_Time_R > Tracing_Time_OK) {
        int x = 0;
        for (int i = 0;i < Tracing_Time_R;i++) {
            x += right_line_Equidistant_sampling[i][1];
        }
        if (x / Tracing_Time_R > Tracing_Time_OK) {
            right_line_OK_flag = TRUE;
        }
    }
}
#define ANGLE_90 1
#define ANGLE_90_VALUE 70
unsigned char left_angle_flag = FALSE;
unsigned char right_angle_flag = FALSE;
int left_angle_line = 0;
int right_angle_line = 0;
void angle_feature_extraction(void) {
    left_angle_flag = FALSE;
    right_angle_flag = FALSE;
    //遍历左边线
    for (int x = 0;x < Tracing_Time_L;x++) { //目前只判断了向图像两侧延展的90度角
        if (left_line_angle_out[x] < -ANGLE_90_VALUE) {
            left_angle_flag = ANGLE_90;
            left_angle_line = x;break;
        }
        //else if()圆环类似九十度先不判断 大角度优先判断
    }
    //遍历右边线
    for (int x = 0;x < Tracing_Time_R;x++) { //目前只判断了向图像两侧延展的90度角
        if (right_line_angle_out[x] > ANGLE_90_VALUE) {
            right_angle_flag = ANGLE_90;
            right_angle_line = x;break;
        }
        //else if()圆环类似九十度先不判断 大角度优先判断
    }
}

void element_identification(void) {
    unsigned char cross_flag = FALSE;
    //十字判断
    //思路：分阶段l'l
    //第一阶段：进入十字前 检测到左右边线均存在 ANGLE_90 ，且拐点的行数相差不大，两L角点欧拉距离小于赛道宽度的1.3倍///且L角点之后的边线分别向两边扩充。
    //判断：
    if (cross_flag == FALSE) {
        if (left_angle_flag == ANGLE_90 && right_angle_flag == ANGLE_90) {
            if (my_abs((float)(left_angle_line - right_angle_line)) <= 10) {
                if (my_abs(left_line_Equidistant_sampling[left_angle_line][1] - right_line_Equidistant_sampling[right_angle_line][1]) <= 1.3 * Tracing_Width) {
                    cross_flag = 1;

                }
            }
        }
    }
    //第二阶段：
    else if (cross_flag == 1) {
        
    }

}
