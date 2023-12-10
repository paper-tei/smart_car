#ifndef CODE_TRACING_H_
#define CODE_TRACING_H_
#include "zf_common_headfile.h"

#define Tracing_Time_all   200   //边线最大长度
void Bottom_Tracing(void);
void Leftline_Tracing(void);
void Rightline_Tracing(void);
extern float left_line[Tracing_Time_all][2];
extern float right_line[Tracing_Time_all][2];
extern float left_line_Triangular_filtering[Tracing_Time_all][2];
extern float right_line_Triangular_filtering[Tracing_Time_all][2];
extern float left_line_Equidistant_sampling[Tracing_Time_all][2];
extern float right_line_Equidistant_sampling[Tracing_Time_all][2];
extern float left_line_angle_out[Tracing_Time_all];
extern float right_line_angle_out[Tracing_Time_all];
extern float left_line_angle[Tracing_Time_all];
extern float right_line_angle[Tracing_Time_all];
extern float left_line_middle[Tracing_Time_all][2];
extern float right_line_middle[Tracing_Time_all][2];
extern int Tracing_Time, Tracing_Time_L, Tracing_Time_R; //当前图像巡线变更次数
extern uint8  mt9v03x_image_Tracing[MT9V03X_H][MT9V03X_W];
extern unsigned char Tracing_Width;
#endif /* TRACING_H_ */
