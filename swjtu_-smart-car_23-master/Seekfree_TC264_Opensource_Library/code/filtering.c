#include "filtering.h"
float pixel_per_meter = 37;
float sample_dist = 0.05;
float angle_dist = 0.5;
float road_width = 0.45;
 extKalman_t Zero_Kalman;
//范围夹逼
int clip(int x, int low, int up) {
    return x > up ? up : x < low ? low : x;
}
float fclip(float x, float low, float up) {
    return x > up ? up : x < low ? low : x;
}

//三角滤波
// Triangular_filtering(left_line,Tracing_Time, left_line_Triangular_filtering,7)
// Triangular_filtering(right_line,Tracing_Time, right_line_Triangular_filtering,7)
void Triangular_filtering(float pts_in[][2], int num, float pts_out[][2], int kernel) {
    int half = kernel / 2;
    for (int i = 0; i < num; i++) {
        pts_out[i][0] = pts_out[i][1] = 0;
        for (int j = -half; j <= half; j++) {
            pts_out[i][0] += pts_in[clip(i + j, 0, num - 1)][0] * (half + 1 - abs(j));
            pts_out[i][1] += pts_in[clip(i + j, 0, num - 1)][1] * (half + 1 - abs(j));
        }
        pts_out[i][0] /= (2 * half + 2) * (half + 1) / 2;
        pts_out[i][1] /= (2 * half + 2) * (half + 1) / 2;
    }
}
//等距滤波
//                       滤波后边线              等距边线                             真实像素距离
//void Equidistant_sampling(rpts0b, rpts0b_num, rpts0s, &rpts0s_num, sample_dist * pixel_per_meter) 
//  rpts0s_num = sizeof(rpts0s) / sizeof(rpts0s[0]);
void Equidistant_sampling(float pts_in[][2], int num1, float pts_out[][2], int* num2, float dist) {
    int remain = 0, len = 0;
    for (int i = 0; i < num1 - 1 && len < *num2; i++) {
        float x0 = pts_in[i][0];
        float y0 = pts_in[i][1];
        float dx = pts_in[i + 1][0] - x0;
        float dy = pts_in[i + 1][1] - y0;
        float dn = sqrt(dx * dx + dy * dy);//前后两点间距离
        dx /= dn;//单位化
        dy /= dn;
        while (remain < dn && len < *num2) {
            x0 += dx * remain;
            pts_out[len][0] = x0;
            y0 += dy * remain;
            pts_out[len][1] = y0;

            len++;
            dn -= remain;
            remain = dist;
        }
        remain -= dn;
    }
    *num2 = len;
}
//角度提取
void local_angle_points(float pts_in[][2], int num, float angle_out[], int dist) {
    for (int i = 0; i < num; i++) {
        if (i <= 0 || i >= num - 1) {
            angle_out[i] = 0;
            continue;
        }
        float dx1 = pts_in[i][0] - pts_in[clip(i - dist, 0, num - 1)][0];
        float dy1 = pts_in[i][1] - pts_in[clip(i - dist, 0, num - 1)][1];
        float dn1 = sqrtf(dx1 * dx1 + dy1 * dy1);
        float dx2 = pts_in[clip(i + dist, 0, num - 1)][0] - pts_in[i][0];
        float dy2 = pts_in[clip(i + dist, 0, num - 1)][1] - pts_in[i][1];
        float dn2 = sqrtf(dx2 * dx2 + dy2 * dy2);
        float c1 = dx1 / dn1;
        float s1 = dy1 / dn1;
        float c2 = dx2 / dn2;
        float s2 = dy2 / dn2;
        angle_out[i] = atan2f(c1 * s2 - c2 * s1, c2 * c1 + s2 * s1) * 180.f / 3.1415926535f;
    }
}

// 角度变化率非极大抑制
void nms_angle(float angle_in[], int num, float angle_out[], int kernel) {
    int half = kernel / 2;
    for (int i = 0; i < num; i++) {
        angle_out[i] = angle_in[i];
        for (int j = -half; j <= half; j++) {
            if (fabs(angle_in[clip(i + j, 0, num - 1)]) > fabs(angle_out[i])) {
                angle_out[i] = 0;
                break;
            }
        }
    }
}

// 左边线跟踪中线
void track_leftline(float pts_in[][2], int num, float pts_out[][2], int approx_num, float dist) {
    for (int i = 0; i < num; i++) {
        float dx = pts_in[clip(i + approx_num, 0, num - 1)][0] - pts_in[clip(i - approx_num, 0, num - 1)][0];
        float dy = pts_in[clip(i + approx_num, 0, num - 1)][1] - pts_in[clip(i - approx_num, 0, num - 1)][1];
        float dn = sqrt(dx * dx + dy * dy);
        dx /= dn;
        dy /= dn;
        pts_out[i][0] = pts_in[i][0] + dy * dist;
        pts_out[i][1] = pts_in[i][1] - dx * dist;
    }
}

// 右边线跟踪中线
void track_rightline(float pts_in[][2], int num, float pts_out[][2], int approx_num, float dist) {
    for (int i = 0; i < num; i++) {
        float dx = pts_in[clip(i + approx_num, 0, num - 1)][0] -
            pts_in[clip(i - approx_num, 0, num - 1)][0];
        float dy = pts_in[clip(i + approx_num, 0, num - 1)][1] -
            pts_in[clip(i - approx_num, 0, num - 1)][1];
        float dn = sqrt(dx * dx + dy * dy);
        dx /= dn;
        dy /= dn;
        pts_out[i][0] = pts_in[i][0] - dy * dist;
        pts_out[i][1] = pts_in[i][1] + dx * dist;
    }
}
/**    
  * @author  Liu heng
  * 一阶卡尔曼滤波器来自RoboMaster论坛
  *   一维卡尔曼滤波器
  *   使用时先定义一个kalman指针，然后调用kalmanCreate()创建一个滤波器
  *   每次读取到传感器数据后即可调用KalmanFilter()来对数据进行滤波
  *          使用示例
  *          extKalman p;                  //定义一个卡尔曼滤波器结构体
  *          float SersorData;             //需要进行滤波的数据
  *          KalmanCreate(&p,20,200);      //初始化该滤波器的Q=20 R=200参数
  *          while(1)                                                                
  *          {                                                                            
  *             SersorData = sersor();                     //获取数据
  *             SersorData = KalmanFilter(&p,SersorData);  //对数据进行滤波
  *          }                                                                            
  */

/**
  * @name   kalmanCreate
  * @brief  创建一个卡尔曼滤波器
  * @param  p:  滤波器
  *         T_Q:系统噪声协方差
  *         T_R:测量噪声协方差
  *         
  * @retval none
  * @attention R固定，Q越大，代表越信任侧量值，Q无穷代表只用测量值
  *		       	反之，Q越小代表越信任模型预测值，Q为零则是只用模型预测
  */
void KalmanCreate(extKalman_t *p,float T_Q,float T_R)
{
    p->X_last = (float)0;
    p->P_last = 0;
    p->Q = T_Q;
    p->R = T_R;
    p->A = 1;
	p->B = 0;
    p->H = 1;
    p->X_mid = p->X_last;
}

/**
  * @name   KalmanFilter
  * @brief  卡尔曼滤波器
  * @param  p:  滤波器
  *         dat:待滤波数据
  * @retval 滤波后的数据
  * @attention Z(k)是系统输入,即测量值   X(k|k)是卡尔曼滤波后的值,即最终输出
  *            A=1 B=0 H=1 I=1  W(K)  V(k)是高斯白噪声,叠加在测量值上了,可以不用管
  *            以下是卡尔曼的5个核心公式
  *            一阶H'即为它本身,否则为转置矩阵
  */

float KalmanFilter(extKalman_t* p,float dat)
{
    p->X_mid =p->A*p->X_last;                     //百度对应公式(1)    x(k|k-1) = A*X(k-1|k-1)+B*U(k)+W(K)
    p->P_mid = p->A*p->P_last+p->Q;               //百度对应公式(2)    p(k|k-1) = A*p(k-1|k-1)*A'+Q
    p->kg = p->P_mid/(p->P_mid+p->R);             //百度对应公式(4)    kg(k) = p(k|k-1)*H'/(H*p(k|k-1)*H'+R)
    p->X_now = p->X_mid+p->kg*(dat-p->X_mid);     //百度对应公式(3)    x(k|k) = X(k|k-1)+kg(k)*(Z(k)-H*X(k|k-1))
    p->P_now = (1-p->kg)*p->P_mid;                //百度对应公式(5)    p(k|k) = (I-kg(k)*H)*P(k|k-1)
    p->P_last = p->P_now;                         //状态更新
    p->X_last = p->X_now;
    return p->X_now;							  //输出预测结果x(k|k)
}
