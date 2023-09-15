//
// Created by RUPC on 2022/10/29.
//
#include "In_perspective.h"

uint8_t* PerImg_ip[RESULT_ROW][RESULT_COL];

//static uint8_t BlackColor = 0;	//无内容部分像素值
/******************变换参数******************************/

//去畸变参数
double cameraMatrix[3][3] = { {296.482019,0.000000,152.664982},{0.000000,286.375269,104.540031},{0.000000,0.000000,1.000000} };
double distCoeffs[5] = { -0.459946,0.283675,0.002304,0.002566,-0.109265 };
int move_xy[2] = { 55,32 };
//逆透视参数
///double change_un_Mat[3][3] = { {-0.268586,0.154240,-4.845014},{0.021850,-0.003979,-9.919369},{0.000098,0.000883,-0.129130} };
/*******************************************************/
// void find_xy(int x, int y, int local[2]) {
//     double fx = cameraMatrix[0][0]
//         , fy = cameraMatrix[1][1]
//         , ux = cameraMatrix[0][2]
//         , uy = cameraMatrix[1][2]
//         , k1 = distCoeffs[0]
//         , k2 = distCoeffs[1]
//         , k3 = distCoeffs[4]
//         , p1 = distCoeffs[2]
//         , p2 = distCoeffs[3];
//     double xCorrected = (x - ux) / fx;
//     double yCorrected = (y - uy) / fy;
//     double xDistortion, yDistortion;
//     double r2 = xCorrected * xCorrected + yCorrected * yCorrected;
//     double deltaRa = 1. + k1 * r2 + k2 * r2 * r2 + k3 * r2 * r2 * r2;
//     double deltaRb = 1 / (1.);
//     double deltaTx = 2. * p1 * xCorrected * yCorrected + p2 * (r2 + 2. * xCorrected * xCorrected);
//     double deltaTy = p1 * (r2 + 2. * yCorrected * yCorrected) + 2. * p2 * xCorrected * yCorrected;
//     xDistortion = xCorrected * deltaRa * deltaRb + deltaTx;
//     yDistortion = yCorrected * deltaRa * deltaRb + deltaTy;
//     xDistortion = xDistortion * fx + ux;
//     yDistortion = yDistortion * fy + uy;
//     if (yDistortion >= 0 && yDistortion < USED_ROW && xDistortion >= 0 && xDistortion < USED_COL) {
//         local[0] = (int)yDistortion;
//         local[1] = (int)xDistortion;
//     }
//     else {
//         local[0] = -1;
//         local[1] = -1;
//     }
// }

// void find_xy1(int x, int y, int local[2]) {
//     int local_x = (int)((change_un_Mat[0][0] * x
//         + change_un_Mat[0][1] * y + change_un_Mat[0][2])
//         / (change_un_Mat[2][0] * x + change_un_Mat[2][1] * y
//             + change_un_Mat[2][2]));
//     int local_y = (int)((change_un_Mat[1][0] * x
//         + change_un_Mat[1][1] * y + change_un_Mat[1][2])
//         / (change_un_Mat[2][0] * x + change_un_Mat[2][1] * y
//             + change_un_Mat[2][2]));
//     if (local_x
//         >= 0 && local_y >= 0) {
//         local[0] = local_y;
//         local[1] = local_x;
//     }
//     else {
//         local[0] = -1;
//         local[1] = -1;
//     }
// }

// void ImageChange_Init(void) {
//     for (int i = 0; i < RESULT_ROW; i++) {
//         for (int j = 0; j < RESULT_COL; j++) {
//             int local_xy[2] = { -1 };
//             find_xy1(j, i, local_xy);
//             if (local_xy[0] != -1 && local_xy[0] != -1) {
//                 int local_xy1[2] = { -1 };
//                 find_xy(local_xy[1] - move_xy[0], local_xy[0] - move_xy[1], local_xy1);
//                 if (local_xy1[0] != -1 && local_xy1[1] != -1) {
//                     PerImg_ip[i][j] = &PER_IMG[local_xy1[0]][local_xy1[1]];
//                 }
//                 else PerImg_ip[i][j] = &BlackColor;
//             }
//             else PerImg_ip[i][j] = &BlackColor;
//         }
//     }
// }

void ImagePerspective_Init(void) {
    static uint8_t BlackColor = 120;
    double change_un_Mat[3][3] = { {0.442019,-0.436842,23.039813},{0.000957,0.034601,4.589996},{-0.000603,-0.004325,0.340024} };
    for (int i = 0; i < RESULT_COL;i++) {
        for (int j = 0; j < RESULT_ROW;j++) {
            int local_x = (int)((change_un_Mat[0][0] * i
                + change_un_Mat[0][1] * j + change_un_Mat[0][2])
                / (change_un_Mat[2][0] * i + change_un_Mat[2][1] * j
                    + change_un_Mat[2][2]));
            int local_y = (int)((change_un_Mat[1][0] * i
                + change_un_Mat[1][1] * j + change_un_Mat[1][2])
                / (change_un_Mat[2][0] * i + change_un_Mat[2][1] * j
                    + change_un_Mat[2][2]));
            if (local_x
                >= 0 && local_y >= 0 && local_y < USED_ROW && local_x < USED_COL) {
                PerImg_ip[j][i] = &PER_IMG[local_y][local_x];
            }
            else {
                PerImg_ip[j][i] = &BlackColor;          //&PER_IMG[0][0];
            }

        }
    }
}
/*ImageUsed[0][0]代表图像左上角的值*/

/*完成摄像头初始化后，调用一次ImagePerspective_Init，此后，直接调用ImageUsed   即为去畸变结果*/

