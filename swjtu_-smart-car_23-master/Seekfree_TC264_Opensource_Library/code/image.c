/*
 * image.c
 *
 *  Created on: 2022年1月6日
 *      Author: paper
 */
 //处理图像的代码，仅二值化用。
#include "image.h"
//赛道宽度比例
#define Width_zhixian  1
#define Width_wandao  ((1+(Speed_Left-150)*0.008)) //1.6
#define Straight 0
#define Left     1
#define Right    2
#define Lose     0
#define Find     2
uint8 track_width[Row] =
{ 0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,
      0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,
      0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,
      0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,
      0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,
      0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,
      0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,
      0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,
      0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,
      0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,
      0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,
      0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0
};
//二值化图像
IFX_ALIGN(4) uint8 Threshold_Image[image_Y][image_X];
//二值化阈值
uint8 Gate = 55;//115
//存左右中边线数组 1-118
uint8 leftline[image_Y];
int16 midline[image_Y];
uint8 rightline[image_Y];

//存储标志 2可信 1不一定可信 0丢线
uint8 leftlineflag[image_Y];
uint8 rightlineflag[image_Y];
uint8 DropRow = 0;
//截止行变量
uint8 endline = 0, frontline = 0;
uint8 whiteline_left = image_Y;
uint8 whiteline_right = image_Y;
uint8 BlackEndL = 0;
uint8 BlackEndM = 0;
uint8 BlackEndR = 0;
uint8 BlackEndMax = 0;
uint8 BlackEndMin = 0;
uint8 leftline_endline = 0;
uint8 rightline_endline = 0;
//斜率变量
float leftline_Curvature = 0;
float rightline_Curvature = 0;
float leftline_slope = 0;
float rightline_slope = 0;
uint8 LEFTLOSED = 0;
uint8 RIGHTLOSED = 0;
uint8 imageover = 0;
float track_width_error = 180;




//计算真实赛道宽度
void track_width_real(void) {
    for (uint8 hang = image_Y - 5;hang > 0;hang--) {
        track_width[hang] = track_width_error;
        track_width_error -= 1.23;
    }
}
uint8 Gate_set = 160;

//得到二值化图像
void Get_Threshold_Image(void) {
    Gate = my_adapt_threshold(*mt9v03x_image, MT9V03X_W, MT9V03X_H) - gate_offset;
    if (Gate > Gate_set + 20)Gate = Gate_set + 20;
    if (Gate < Gate_set - 20)Gate = Gate_set - 20;
    //二值化
    for (uint8 hang = 0;hang < MT9V03X_H;hang++)
        for (uint8 lie = 0;lie < MT9V03X_W;lie++) {
            if (mt9v03x_image[hang][lie] >= Gate)
                Threshold_Image[hang][lie] = white;
            else
                Threshold_Image[hang][lie] = black;
        }

}
//像素滤波
void Pixle_Filter(void) {
    int nr;  //行
    int nc;  //列

    for (nr = 10; nr < 110; nr++) {
        for (nc = 10; nc < 170; nc++) {
            if ((Threshold_Image[nr][nc] == black) && (Threshold_Image[nr - 1][nc] + Threshold_Image[nr + 1][nc] + Threshold_Image[nr][nc + 1] + Threshold_Image[nr][nc - 1] >= white * 3)) {
                Threshold_Image[nr][nc] = white;
            }
        }
    }
}
//大津法算法
uint8 my_adapt_threshold(uint8* image, uint16 col, uint16 row)   //注意计算阈值的一定要是原图像
{
#define GrayScale 256
    uint16 width = col;
    uint16 height = row;
    int pixelCount[GrayScale];
    float pixelPro[GrayScale];
    int i, j, pixelSum = width * height / 4;
    uint8 threshold = 0;
    uint8* data = image;  //指向像素数据的指针
    for (i = 0; i < GrayScale; i++) {
        pixelCount[i] = 0;
        pixelPro[i] = 0;
    }
    uint32 gray_sum = 0;
    //统计灰度级中每个像素在整幅图像中的个数
    for (i = 0; i < height; i += 2) {
        for (j = 0; j < width; j += 2) {
            pixelCount[(int)data[i * width + j]]++;  //将当前的点的像素值作为计数数组的下标
            gray_sum += (int)data[i * width + j];       //灰度值总和
        }
    }
    //计算每个像素值的点在整幅图像中的比例
    for (i = 0; i < GrayScale; i++) {
        pixelPro[i] = (float)pixelCount[i] / pixelSum;

    }
    //遍历灰度级[0,255]
    float w0, w1, u0tmp, u1tmp, u0, u1, u, deltaTmp, deltaMax = 0;


    w0 = w1 = u0tmp = u1tmp = u0 = u1 = u = deltaTmp = 0;
    for (j = 0; j < GrayScale; j++) {

        w0 += pixelPro[j];  //背景部分每个灰度值的像素点所占比例之和   即背景部分的比例
        u0tmp += j * pixelPro[j];  //背景部分 每个灰度值的点的比例 *灰度值

        w1 = 1 - w0;
        u1tmp = gray_sum / pixelSum - u0tmp;

        u0 = u0tmp / w0;              //背景平均灰度
        u1 = u1tmp / w1;              //前景平均灰度
        u = u0tmp + u1tmp;            //全局平均灰度
        deltaTmp = w0 * pow((u0 - u), 2) + w1 * pow((u1 - u), 2);
        if (deltaTmp > deltaMax) {
            deltaMax = deltaTmp;
            threshold = (uint8)j;
        }
        if (deltaTmp < deltaMax) {
            break;
        }
    }
    return threshold;
}//寻找左侧边线，并且记录丢线点
//第一行图像寻线
//起跑线与其他图像寻线方式区别
void get_route_first(uint8 hang, uint8 start) {
    //清零

    for (uint8 a = 0;a <= image_Y;a++) {
        leftline[a] = 0;
        rightline[a] = image_X - 1;
        leftlineflag[a] = 0;
        rightlineflag[a] = 0;
        midline[a] = image_X / 2;
    }
    LEFTLOSED = 0;
    RIGHTLOSED = 0;
    //////////////////////////////////////////////////////////////左边线
    if (Threshold_Image[hang][start] == white) //底边中点为白
    {
        //底边中点为白 向 左巡线找跳变 (白->黑)  //左弯成立 直道成立 右弯成立
        for (uint8 lie = start;lie >= 2;lie--) {

            if (Threshold_Image[hang][lie] == white && Threshold_Image[hang][lie - 1] == black) {
                leftline[hang] = lie;
                leftlineflag[hang] = Find;
                break;
            }
        }
    }
    //底边中点为黑 向 右巡线找跳变 (黑->白) //左急弯成立 右急弯成立
    else {

        for (uint8 lie = start;lie <= image_X - 2;lie++) {
            if (Threshold_Image[hang][lie] == black && Threshold_Image[hang][lie + 1] == white) {
                leftline[hang] = lie;
                leftlineflag[hang] = Find;
                break;
            }
        }
    }
    //如果 没找到 ，直接给定值
    if (leftlineflag[hang] == 0) {
        leftline[hang] = 1;
    }

    //////////////////////////////////////////////////////////////右边线
    if (Threshold_Image[hang][start] == white) {

        //底边中点为白 向 右巡线找跳变 (白->黑)  //左弯成立 直道成立 右弯成立
        for (uint8 lie = start;lie <= image_X - 2;lie++) {
            if (Threshold_Image[hang][lie] == white && Threshold_Image[hang][lie + 1] == black) {
                rightline[hang] = lie;
                rightlineflag[hang] = Find;
                break;
            }
        }
    }
    //底边中点为黑 向 左巡线找跳变 (黑->白) //左急弯成立 右急弯成立
    else {

        //向左找
        for (uint8 lie = start;lie >= 2;lie--) {
            if (Threshold_Image[hang][lie] == black && Threshold_Image[hang][lie - 1] == white) {
                rightline[hang] = lie;
                rightlineflag[hang] = Find;
                break;
            }
        }
    }
    //如果没找到，直接给定值
    if (rightlineflag[hang] == 0) {
        rightline[hang] = image_X - 2;
    }
    //如果左右边线都不绝对可信
    // if(rightlineflag[hang]==1&&leftlineflag[hang]==1)
    // {
    //   leftline[hang]=1;
    //   rightline[hang]=image_X-2;
    // }
    // leftlineflag[hang]=1;
    // rightlineflag[hang]=1;

}
//跟踪寻线
void get_route_all(void) {
    //左侧巡线疑似有问题(显示出来劣于右侧巡线)
    //idea:用上八邻域巡线
    //初始化
    uint8 Lstart = 0, L_max = 0, L_min = 0;
    uint8 Rstart = 0, R_max = 0, R_min = 0;
    uint8 range = 5;
    for (uint8 hang = image_Y - 2;hang > 0;hang--) {
        //左边线
        //确定搜线范围
        //根据上一行确定本行寻线点
        if (leftlineflag[hang + 1] == 0)//如果上一行丢线,按照从中间开扫的方式巡线
        {
            Lstart = 3;
            L_max = image_X - 2;
            L_min = 2;
        }
        else//如果上一行未丢线,按照跟踪巡线的方式巡线
        {
            Lstart = leftline[hang + 1];
            L_max = leftline[hang + 1] + range;
            L_min = leftline[hang + 1] - range;
            //限幅
            if (L_max > image_X - 1)
                L_max = image_X - 2;
            if (L_min < 0)
                L_min = 2;
        }
        //左边线
        if (Threshold_Image[hang][Lstart] == black)//下面一行的列数对应本行点为黑点(按照透视原理一般成立)
        {
            //向右找 黑->白 跳变       直道成立 右弯成立 右急弯成立
            for (uint8 lie = Lstart;lie < L_max;lie++) {
                if (Threshold_Image[hang][lie] == black && Threshold_Image[hang][lie + 1] == white) {
                    leftline[hang] = lie;
                    leftlineflag[hang] = Find;
                    break;
                }
            }
        }
        else//下面一行的列数对应本行点为白点(出现左转)
        {
            //向左找 白->黑 跳变    左弯成立 左急弯成立
            for (uint8 lie = Lstart;lie > L_min;lie--) {
                if (Threshold_Image[hang][lie] == white && Threshold_Image[hang][lie - 1] == black) {
                    leftline[hang] = lie;
                    leftlineflag[hang] = Find;
                    break;
                }
            }
        }
        //如果还没找到说明丢线
        if (leftlineflag[hang] == 0) {
            leftline[hang] = 1;
        }
        //右边线
        //确定寻线范围
        if (rightlineflag[hang + 1] == 0) {
            Rstart = image_X - 3;
            R_max = image_X - 2;
            R_min = 2;
        }
        else {
            Rstart = rightline[hang + 1];
            R_max = rightline[hang + 1] + range;
            R_min = rightline[hang + 1] - range;
            if (R_max > image_X - 1)
                R_max = image_X - 2;
            if (R_min < 0)
                R_min = 2;
        }
        //右边线
        if (Threshold_Image[hang][Rstart] == black) {
            //向左找
            for (uint8 lie = Rstart;lie > R_min;lie--) {
                if (Threshold_Image[hang][lie] == black && Threshold_Image[hang][lie - 1] == white) {
                    rightline[hang] = lie;
                    rightlineflag[hang] = Find;
                    break;
                }
            }
        }
        else {
            //向右找
            for (uint8 lie = Rstart;lie < R_max;lie++) {
                if (Threshold_Image[hang][lie] == white && Threshold_Image[hang][lie + 1] == black) {
                    rightline[hang] = lie;
                    rightlineflag[hang] = Find;
                    break;
                }
            }
            //如果还没找到
            if (rightlineflag[hang] == Lose) {
                rightline[hang] = 178;
            }
        }
        //寻找错误,按照丢线处理
        if (leftline[hang] > rightline[hang]) {
            leftline[hang] = 1;
            rightline[hang] = 178;
            leftlineflag[hang] = Lose;
            rightlineflag[hang] = Lose;
        }
    }
    //车最远一行按上一行补
    leftline[0] = leftline[1];
    leftlineflag[0] = leftlineflag[1];
    rightline[0] = rightline[1];
    rightlineflag[0] = rightlineflag[1];
}

//中线拟合
void get_route_midline(void) {
    for (uint8 hang = image_Y - 5;hang > endline;hang--) {


        if ((leftlineflag[hang] == 2) && (rightlineflag[hang] == 0)) {
            // rightline[hang]=leftline[hang]+Width_zhixian*track_width[hang];
            midline[hang] = leftline[hang] + Width_zhixian * track_width[hang] / 2;
        }
        //左丢右不丢
        else if ((leftlineflag[hang] == 0) && (rightlineflag[hang] == 2)) {
            //leftline[hang]=rightline[hang]-Width_zhixian*track_width[hang];
            midline[hang] = rightline[hang] - Width_zhixian * track_width[hang] / 2;
        }
        //全不丢
        else if ((leftlineflag[hang] == 2) && (rightlineflag[hang] == 2)) {
            midline[hang] = (leftline[hang] + rightline[hang]) / 2;
        }
        else
            midline[hang] = image_X / 2;
    }
}
//截止行
void get_route_endline(void) {
    unsigned char LEndFlag = 0;//标志位
    unsigned char MEndFlag = 0;
    unsigned char REndFlag = 0;
    int i = 0;
    BlackEndL = 0;
    BlackEndM = 0;
    BlackEndR = 0;
    for (i = Row - 1;i > 0;i--) {
        if (Threshold_Image[i][Col / 2] == white && !MEndFlag) {
            BlackEndM++;//中线截止行 计算
        }
        else if (i > 1 && Threshold_Image[i - 1][Col / 2] == black && Threshold_Image[i - 2][Col / 2] == black)//连续两行是黑色
        {
            MEndFlag = 1;//中线截止行已找到
        }


        if (Threshold_Image[i][Col / 4] == white && !LEndFlag) {
            BlackEndL++;//左线截止行 计算
        }
        else if (i > 1 && Threshold_Image[i - 1][Col / 4] == black && Threshold_Image[i - 2][Col / 4] == black) {
            LEndFlag = 1;//左线截止行已找到
        }


        if (Threshold_Image[i][Col * 3 / 4] == white && !REndFlag) {
            BlackEndR++;//右线截止行 计算
        }
        else if (i > 1 && Threshold_Image[i - 1][Col * 3 / 4] == black && Threshold_Image[i - 2][Col * 3 / 4] == black) {
            REndFlag = 1;//右线截止行已找到
        }
    }
    BlackEndMax = max(BlackEndL, BlackEndM, BlackEndR);//取大值
    BlackEndMin = BlackEndM;
    if (BlackEndMax > 120) {
        BlackEndMax = 120;
    }
    if (BlackEndMin > 120) {
        BlackEndMin = 120;
    }
    endline = 120 - BlackEndMax;//截止行行数
    frontline = 120 - BlackEndMin;
    if (endline >= 120) endline = 120;
    else if (endline <= 0)  endline = 0;

    if (frontline >= 120) frontline = 120;
    else if (frontline <= 0)  frontline = 0;

    if (frontline < endline) frontline = endline;
}
uint16 uint_range(uint16 a, uint16 b, uint16 c) {
    if (b < a)b = a;
    else if (b > c)b = c;
    return b;
}
void Draw_line(void) {
    for (uint8 y = MT9V03X_H - 5;y > 0;y--) {
        if ((leftlineflag[y] == 0) || (rightlineflag[y] == 0)) {
            ips200_draw_point(uint_range(0, midline[y], 240 - 1), y, RGB565_RED);
        }
        else {
            ips200_draw_point(uint_range(0, midline[y], 240 - 1), y, RGB565_GREEN);
        }
        ips200_draw_point(leftline[y], y, RGB565_BLUE);
        ips200_draw_point(rightline[y], y, RGB565_BLUE);
        ips200_draw_point(ring.L_ENTRY_LIE, y, RGB565_RED);
        ips200_draw_point(ring.R_ENTRY_LIE, y, RGB565_RED);
    }
    for (uint8 lie = MT9V03X_W;lie > 0;lie--) {
        ips200_draw_point(lie, endline, RGB565_RED);
        ips200_draw_point(lie, ring.L_ENTRY_hang, RGB565_GREEN);
        ips200_draw_point(lie, ring.R_ENTRY_hang, RGB565_GREEN);
        //ips200_draw_point(lie, R_UP, RGB565_GREEN);
        //ips200_draw_point(lie, L_DOWN, RGB565_GREEN);
       // ips200_draw_point(lie, L_UP, RGB565_GREEN);
    }
}
uint8 max(uint8 a, uint8 b, uint8 c) {
    uint8 max;
    max = a;
    if (b > max) {
        max = b;
    }
    if (c > max) {
        max = c;
    }
    return max;
}
uint8 min(uint8 a, uint8 b, uint8 c) {
    uint8 min;
    min = a;
    if (b < min) {
        min = b;
    }
    if (c < min) {
        min = c;
    }
    return min;
}

void Curvature_determination(void) {
    leftline_endline = 0;
    rightline_endline = 0;
    //判断左边线实际巡线截止行( 从 下 向 上 巡 线 )
    for (uint8 hang = 119;hang > endline;hang--) {
        //左侧已经没有线
        if (leftlineflag[hang] == 0) { LEFTLOSED = 1;break; }
    }
    for (uint8 hang = 110;hang > endline;hang--) {
        if (leftlineflag[hang + 2] == Find && leftlineflag[hang + 1] == Find && leftlineflag[hang] == Find && leftlineflag[hang - 1] != Find) {
            leftline_endline = hang;
            break;
        }

    }
    if (leftline_endline == 0)
        leftline_endline = endline + 5;
    //判断右边线实际巡线截止行
    for (uint8 hang = 119;hang > endline;hang--) {
        //右侧已经没有线
        if (rightlineflag[hang] == 0) { RIGHTLOSED = 1;break; }
    }
    for (uint8 hang = 110;hang > endline;hang--) {
        if (rightlineflag[hang + 2] == Find && rightlineflag[hang + 1] == Find && rightlineflag[hang] == Find && rightlineflag[hang - 1] != Find) {
            rightline_endline = hang;
            break;
        }
    }
    if (rightline_endline == 0)
        rightline_endline = endline + 5;

    if (!LEFTLOSED) {
        //leftline_Curvature = 2*((leftline[119-leftline_endline/2]-leftline[image_Y-1])*(leftline_endline-image_Y+1)-(leftline[leftline_endline]-leftline[image_Y-1])*(119-leftline_endline/2-image_Y+1))
        leftline_Curvature = (((leftline[leftline_endline]) * 1.0) / ((120 - leftline_endline) * 1.0)) * 5.0;
        // int32 x1 = leftline[image_Y-1-5],x2 =leftline[(image_Y-1-(5+image_Y-1-leftline_endline)/2)],x3 = leftline[leftline_endline];
        // int32 y1 = 5,y3 = image_Y-leftline_endline,y2 = (y1+y3)/2;
        // int32 AB = m_sqrt((y2-y1)*(y2-y1)+(x2-x1)*(x2-x1));
        // int32 BC = m_sqrt((y3-y2)*(y3-y2)+(x3-x2)*(x3-x2));
        // int32 AC = m_sqrt((y3-y1)*(y3-y1)+(x3-x1)*(x3-x1));
        // int32 S_ABC = ((x2-x1)*(y3-y1)-(x3-x1)*(y2-y1))/2;
        //leftline_Curvature = 4.0*S_ABC/(AB*BC*AC)*1000;
        //leftline_slope     = (y3-y1)/(x3-x1);
    }
    else {
        leftline_Curvature = 0;
        leftline_slope = 0;
    }


    if (!RIGHTLOSED) {
        rightline_Curvature = (((rightline[rightline_endline] - 180) * 1.0) / ((120 - rightline_endline) * 1.0)) * 5.0;
        // int32 x1 = rightline[image_Y-1-5],x2 =rightline[(image_Y-1-(5+image_Y-1-rightline_endline)/2)],x3 = rightline[rightline_endline];
        // int32 y1 = 5,y3 = image_Y-rightline_endline,y2 = (y1+y3)/2;
        // int32 AB = m_sqrt((y2-y1)*(y2-y1)+(x2-x1)*(x2-x1));
        // int32 BC = m_sqrt((y3-y2)*(y3-y2)+(x3-x2)*(x3-x2));
        // int32 AC = m_sqrt((y3-y1)*(y3-y1)+(x3-x1)*(x3-x1));
        // int32 S_ABC = ((x2-x1)*(y3-y1)-(x3-x1)*(y2-y1))/2;
        //rightline_Curvature = 4.0*S_ABC/(AB*BC*AC)*1000;
        //rightline_slope     = (y3-y1)/(x3-x1);
    }
    else {
        rightline_Curvature = 0;
        rightline_slope = 0;
    }

    //根据截止行来简单地计算左右赛道的斜率
}
//二分法求无符号整数平方跟
unsigned int m_sqrt(unsigned int x) {
    unsigned char ans = 0, p = 0x80;
    while (p != 0) {
        ans += p;
        if (ans * ans > x) {
            ans -= p;
        }
        p = (unsigned)(p / 2);
    }
    return(ans);
}




