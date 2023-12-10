#ifndef CODE_FILTERING_H_
#define CODE_FILTERING_H_
#include "zf_common_headfile.h"

extern float pixel_per_meter, sample_dist, angle_dist, road_width;


int clip(int x, int low, int up);
float fclip(float x, float low, float up);
void Equidistant_sampling(float pts_in[][2], int num1, float pts_out[][2], int* num2, float dist);
void Triangular_filtering(float pts_in[][2], int num, float pts_out[][2], int kernel);
void local_angle_points(float pts_in[][2], int num, float angle_out[], int dist);
void nms_angle(float angle_in[], int num, float angle_out[], int kernel);
void track_leftline(float pts_in[][2], int num, float pts_out[][2], int approx_num, float dist);
void track_rightline(float pts_in[][2], int num, float pts_out[][2], int approx_num, float dist);


typedef struct {
    float X_last; //上一时刻的最优结果  X(k-|k-1)
    float X_mid;  //当前时刻的预测结果  X(k|k-1)
    float X_now;  //当前时刻的最优结果  X(k|k)
    float P_mid;  //当前时刻预测结果的协方差  P(k|k-1)
    float P_now;  //当前时刻最优结果的协方差  P(k|k)
    float P_last; //上一时刻最优结果的协方差  P(k-1|k-1)
    float kg;     //kalman增益
    float A;      //系统参数
	float B;
    float Q;
    float R;
    float H;
}extKalman_t;

//卡尔曼滤波
void KalmanCreate(extKalman_t *p,float T_Q,float T_R);
float KalmanFilter(extKalman_t* p,float dat);
extern extKalman_t Zero_Kalman;
#endif 
