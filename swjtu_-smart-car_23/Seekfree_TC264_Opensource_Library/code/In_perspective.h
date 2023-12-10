#ifndef CODE_IN_PERSPECTIVE_H_
#define CODE_IN_PERSPECTIVE_H_
#include "zf_common_headfile.h"
#define RESULT_ROW 60 //结果图的行列
#define RESULT_COL 40
#define MT9V03X_H_IP RESULT_ROW
#define MT9V03X_W_IP RESULT_COL
#define         USED_ROW                MT9V03X_H  //用于变换图的行列
#define         USED_COL                MT9V03X_W
#define PER_IMG     mt9v03x_image//mt9v03x_image_dvp:用于透视变换的图像 也可以使用二值化图
#define ImageUsed   *PerImg_ip//*PerImg_ip定义使用的图像，ImageUsed为用于巡线和识别的图像
extern uint8_t* PerImg_ip[RESULT_ROW][RESULT_COL];
//void ImageChange_Init(void);
void ImagePerspective_Init(void);
#endif 
