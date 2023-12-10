/*
 * error.c
 *
 *  Created on: 2022
 *      Author: paper
 */
#include "error.h"

//赛道中值
//////TEST
#define basic 90
#define my_abs(num) (((num)>0)?(num):-(num))
#define Straight 0
#define Left     1
#define Right    2
#define Sancha   3
#define Ring   4
#define image_Y 120
#define image_X 180
#define Width_wandao  ((1+(Speed_Left-150)*0.008)) //1.6
//-----------------------用于PID三个参数的调整------------------------
// float Tp_S = 1.5,Td_S = 5,Ti_S = 0.8;
//-------------------------------------------------------------------
int16 yawspeed=0;
//权重 从上到下
uint8 Weight[120]={      
                         50,50,50,50,50,50,50,50,50,50,
                         65,65,65,65,65,65,65,65,65,65,
                         70,70,70,70,70,70,70,70,70,70,
                         65,65,65,65,65,65,65,65,65,65,
                         65,65,65,65,65,65,65,65,65,65,
                         40,40,40,40,40,40,40,40,40,40,
                         30,30,30,30,30,30,30,30,30,30,
                         25,25,25,25,25,25,25,25,25,25,
                         20,20,20,20,20,20,20,20,20,20,
                         15,15,15,15,15,15,15,15,15,15,
                         10,10,10,10,10,10,10,10,10,10,
                          5, 5, 5, 5, 5, 5, 5, 5, 5, 5
                  };
int32 Error = 0;
int32 LastError=0;
int32 Ec;
int32 WeightSum=0;
int32 CenterMeanValue=90; //理想中线值
int32 CenterSum=0;
uint8 Track_Type = 0;
float new_speed_Straight,new_speed_Left,new_speed_Right;
float U_d , U_p;
//偏差加权
void get_deviation(void)
{
    CenterSum=0;
    CenterMeanValue=0;
    WeightSum=0;
    for(uint8 hang=image_Y-1;hang>endline;hang--)
    {
        if(leftlineflag[hang]!=0||rightlineflag[hang]!=0)
        {
            CenterSum+=midline[hang]*Weight[hang];
            WeightSum+=Weight[hang];
        }
    }
    //算出加权平均后中线的值
    if(WeightSum!=0)
    {
        CenterMeanValue=(CenterSum/WeightSum); 
    }
    LastError = Error ;
    Ec = Error - LastError;
    Error = CenterMeanValue-basic;
    //bebug
    if(Error == -90&&leftlineflag[119]==2&&rightlineflag[119]==0)
    {
        Error = 90;
    }
}
uint8 unlost_R=0,unlost_L=0,min_break_L=0,min_break_R=0;
//赛道弯曲判断加权误差
void Weighted_error(void)
{
    if(scratch_cnt>=20&&start_count<=51)
    {
      Error = 0;
    }
    else if(rightline_Curvature>30)
    {
      rightline_Curvature =30;
    }
    else if(rightline_Curvature<-30)
    {
      rightline_Curvature =-30;
    }
     if(leftline_Curvature>30)
    {
      leftline_Curvature =30;
    }
    else if(leftline_Curvature<-30)
    {
      leftline_Curvature =-30;
    }
    for(uint8 hang=image_Y-1;hang>endline;hang--)
    {
        if(leftlineflag[hang]!=0)
        {
            unlost_L++; 
        }
        if(rightlineflag[hang]!=0)
        {
            unlost_R++;
        }
    }
    //圆环
    if(ringflag != 0)
    {
        Track_Type = Ring;
        if(ringflag ==3||ringflag ==13)
        {
            if(icm_ring>=1900000)
            {
                 //Error = Error - rightline_Curvature*rightline_Curvature*0 - endline*0.7;
                if((Error > Error_ring_min)&&Error_ring_min!=0)
                {
                    Error =  Error_ring_min;
                }
            }
             //抓取圆环转向最小值 
            if(icm_ring>=1000000&&min_break_L==0)
            {
                min_break_L=1;
                Error_ring_min = Error ;
            }
        }
        else if(ringflag ==4||ringflag ==14)
        {
            //Error = Error + leftline_Curvature*leftline_Curvature*0 + endline*0.7;
            if(icm_ring<=-1800000)
            {
                if((Error < Error_ring_min)&&Error_ring_min!=0)
                {
                    Error =  Error_ring_min;
                } 
            }
             //抓取圆环转向最小值
            if(icm_ring<=-1000000&&min_break_R==0)
            {
                min_break_R =1 ;
                Error_ring_min = Error ;
            }
        }

    }
    //左转
    else if(LEFTLOSED&&(Error<=-5)&&(unlost_R>(image_Y-endline)*2/3)&&(unlost_L<(image_Y-endline)*15/16))//&&rightline_Curvature<=-9)
    {
      
      Track_Type = Left; 
      //根据斜率 拟合一个合适的方程
      //Error = Error ;
      //Error = Error/(1+exp(-Error));
      //new_speed_Left = log(Speed_Left)*3 + pow(Speed_Left,0.25); 
    }
    //右转
    else if(RIGHTLOSED&&(Error>=5)&&(unlost_L>(image_Y-endline)*2/3)&&(unlost_R<(image_Y-endline)*15/16))//&&rightline_Curvature>=9)
    {
      Track_Type = Right;   
      //根据斜率 拟合一个合适的方程
     // Error = Error  ;
     // //Error = Error/(1+exp(-Error));
     //new_speed_Right = log(Speed_Right)*3 + pow(Speed_Right,0.25);
    }
    else
    //直线
    {
        //new_speed_Straight = log(Speed_Right)*2;
        Track_Type = Straight;
    }
    //unlost=0;
    unlost_L=0,unlost_R=0;
}
float Differential_speed=0;
//弯道pid
void DM_Control(void)
{
        //需调
        float error_P=Error,error_D=Error-LastError,error_I=Error+LastError;
        U_d = Fuzzy_D((float)Error , (float)Ec);
        U_p = Fuzzy_P((float)Error , (float)Ec);
        LastError=Error; 
        yawspeed = (int16)((U_p*error_P + U_d*error_D)*0.7+yawspeed*0.3);
}     
  
