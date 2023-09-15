/*
 * ring.h
 *
 *  Created on: 2022Äê1ÔÂ15ÈÕ
 *      Author: paper
 */

#ifndef _IMAGE_RING_H_
#define _IMAGE_RING_H_
#include "zf_common_headfile.h"

typedef struct {
    unsigned char ring_state;//圆环当前状态
    unsigned char ring_state_last;//圆环上一状态
    unsigned char ring_find;//圆环是否找到
    unsigned char ring_direction;//圆环方向
    float ring_yaw_start;//圆环起始偏航
    float ring_yaw_cur;//圆环当前偏航
    unsigned char ring_left_white;//圆环左侧边线丢线白点数
    unsigned char ring_right_white;//圆环右侧边线丢线白点数
    float ring_error_record[300];//圆环偏航记录
    float ring_error_count;//圆环偏航计数
    float ring_error_down;//圆环偏航最小值
    unsigned char ring_time_count;//圆环时间计数
    uint8  Spinodal_flag;//拐点标志位，1为识别到圆环拐点
    uint8 L_ENTRY_hang;//左入口
    uint8 L_ENTRY_LIE;
    uint8 L_ENTRY_FLAG;
    uint8 R_ENTRY_hang;
    uint8 R_ENTRY_LIE;
    uint8 R_ENTRY_FLAG;
    uint8 L_LEAVE_hang;
    uint8 R_LEAVE_hang;
}Ring;

typedef struct {
    unsigned char crossroad_state;//十字当前状态
    unsigned char crossroad_state_last;//十字上一状态
    unsigned char crossroad_find;//十字是否找到
    unsigned char crossroad_direction;//十字转向方向
    unsigned char crossroad_left_white;//十字左侧边线丢线白点数
    unsigned char crossroad_left_black;//十字左侧边线丢线黑点数
    unsigned char crossroad_right_white;//十字右侧边线丢线白点数
    unsigned char crossroad_right_black;//十字右侧边线丢线黑点数
    unsigned char crossroad_yaw_count;//十字偏航计数
    unsigned char crossroad_left_black_to_white;//十字黑到白
    unsigned char crossroad_right_black_to_white;//十字黑到白
}Crossroad;
typedef struct {
    unsigned char inertial_navigation_state;//惯性导航当前状态
    unsigned char inertial_navigation_state_last;//惯性导航上一状态
    float inertial_navigation_start_absyaw;//惯性导航起始偏航(绝对值)
    float inertial_navigation_set_yaw;//惯性导航设定偏航斜坡当前值
    float inertial_navigation_set_distance;//惯性导航设定距离
    float inertial_navigation_cur_distance;//惯性导航当前距离
    float inertial_navigation_set_yaw_update;//惯性导航设定偏航斜坡更新值
    float inertial_navigation_set_yaw_total;//惯性导航设定偏航斜坡更新值
    float inertial_navigation_set_yaw_up_total;//惯性导航设定偏航斜坡更新值
    float inertial_navigation_set_yaw_old;
    float start_yaw;//惯导起始偏航
    float cur_yaw;//惯导当前偏航
    float x_cur;//惯导当前x坐标
    float y_cur;//惯导当前y坐标
    float x_set;//惯导设定x坐标
    float y_set;//惯导设定y坐标
    float x_start;//惯导起始x坐标
    float y_start;//惯导起始y坐标
}Inertial_Navigation;
typedef struct {
    unsigned char roadblock_state;//路障当前状态
    unsigned char roadblock_state_last;//路障上一状态
    unsigned char roadblock_direction;//路障转向方向
    unsigned char roadblock_yaw_count;//路障偏航计数
    float yaw_start;//路障起始偏航
    float distance_start;//路障起始距离
}Roadblock;

typedef struct {
    unsigned char last_mode;//车当前状态
    unsigned char now_mode;//车一状态
}Car_mode;
enum {
    CAMERA,//摄像头循迹
    ELECTROMAGNETIC,//电磁循迹
    INERTIAL_NAVIGATION,//惯导循迹
    INERTIAL_NAVIGATION_READY,
    ROADBLOCK,//路障

    OUT_HOME
};
enum {
    STRAIGHT,
    LEFT,
    RIGHT

};
enum {
    FINDING, ENTRY, TURN, LEAVE, RESTART
};
enum {
    NONE,
    OBSTACLES1,
    OBSTACLES2,
    OBSTACLES3,
    OBSTACLES4,
    RAMP,
    BREAKER1,
    BREAKER2,
    BREAKER3,
    BREAKER4,
    ROADBLock
};

void crossroad_recognition(void);
void crossroad_handler(void);
void ring_recognition(void);
void ring_handler(void);
void roadblock_recognition(void);
void roadblock_handler(void);
void breaker_handler(void);

void ring_handler_EM(void);//纯电磁过圆环
void ring_recognition_EM(void);//纯电磁识别圆环

extern Ring ring;
extern Crossroad cross;
extern Inertial_Navigation navigation;
extern Roadblock roadblock;
extern  uint8 Crossroad_judge_flag;
extern float destination_yaw_set, destination_yaw;
extern uint8 L_DOWN, R_DOWN, L_UP, R_UP;
extern Car_mode car_mode;
extern uint8 go_home_flag;
extern Inertial_Navigation Garage_navigation;
extern int Threshold_Image_black, Threshold_Image_white;
#endif
/* CODE_RING_H_ */
