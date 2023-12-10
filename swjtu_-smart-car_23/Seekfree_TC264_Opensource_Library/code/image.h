/*
 * image.h
 *
 *  Created on: 2022年1月6日
 *      Author: paper
 */
#include "zf_common_headfile.h"
#ifndef _IMAGE_H_
#define _IMAGE_H_
#define white 255
#define black 0
#define Row    MT9V03X_H               //图像行数
#define Col    MT9V03X_W               //图像列数
#define image_Y MT9V03X_H
#define image_X MT9V03X_W
extern uint8 Threshold_Image[image_Y][image_X];
extern uint8 Gate;

void Get_Threshold_Image(void);
uint8 my_adapt_threshold(uint8* image, uint16 col, uint16 row);//注意计算阈值的一定要是原图像
void get_route_first(uint8 hang, uint8 start);
void get_route_all(void);
void get_route_endline(void);
void get_route_midline(void);
void Curvature_determination(void);
void White_scan(void);
int regression(int startline, int endline, int p[]);
void Pixle_Filter(void);
void scratch_line(void);
void track_width_real(void);
uint8 max(uint8 a, uint8 b, uint8 c);
uint8 min(uint8 a, uint8 b, uint8 c);
extern int16 midline[Row];
extern uint8 leftline[Row];
extern uint8 rightline[Row];
extern uint8 leftlineflag[Row];
extern uint8 rightlineflag[Row];
unsigned int m_sqrt(unsigned int x);
extern uint8 endline, frontline;
extern uint8 whiteline_left;
extern uint8 whiteline_right;
extern uint8 LEFTLOSED;
extern uint8 RIGHTLOSED;
extern float leftline_Curvature;
extern float rightline_Curvature, leftline_slope, rightline_slope;
extern uint8 imageover;
void Draw_line(void);
extern uint8 BlackEndL;
extern uint8 BlackEndM;
extern uint8 BlackEndR;
#endif /* CODE_IMAGE_H_ */
