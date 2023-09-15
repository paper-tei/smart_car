/*
 * pid.c
 *
 *  Created on: 2022年1月13日
 *      Author: paper
 */
#include "Three_Forks_Road.h"
#define image_Y 120
#define image_X 180
#define True 1
#define False 0
#define my_abs(num) (((num)>0)?(num):-(num))
uint8 san_cha_tiaobian=0,san_cha_point=0,left_san_cha_flag=0,right_san_cha_flag=0,san_cha_L_chu_zhe_flag=0;
uint8 left_san_cha_hang=0,right_san_cha_hang=0,san_cha_flag=0;
uint8 left_san_cha_lose=0,right_san_cha_lose=0,left_san_cha_find=0,right_san_cha_find=0;
uint8 Number_of_san_cha=0,in_san_cha=0;
uint32 white_point_sum=0;
int16 white_point_L[image_Y];
int16 white_point_R[image_Y];
uint8 san_cha_da_L=0,san_cha_xiao_L=0;
uint8 san_cha_da_R=0,san_cha_xiao_R=0;
uint8 san_cha_ok_flag = 0,icm_san_cha_flag=0;
int32 icm_san_cha = 0;
int32 dutyru = 300000;
int32 dutychu = 380000;
void Three_Forks_Road_scan(void)
{
    //第一次判断
    if(san_cha_flag==0&&in_san_cha==0)
    {
        memset(white_point_L,0,sizeof(white_point_L));
        memset(white_point_R,0,sizeof(white_point_R));
        white_point_sum=0;
        left_san_cha_hang=0;
        left_san_cha_flag=False;
        right_san_cha_hang=0;
        right_san_cha_flag=False;
        left_san_cha_lose=0;
        right_san_cha_lose=0;
        //找下侧两个拐点
        for(uint8 hang=image_Y-1;hang>endline;hang--)
        {
            if((leftline[hang-5]<leftline[hang])&&(leftline[hang+5]<leftline[hang]))
            {
                left_san_cha_hang=hang;
                if(left_san_cha_hang>image_Y*1/4)
                    left_san_cha_flag=True;
                //找到左侧拐点
                break;
            }
        }
        for(uint8 hang=image_Y-1;hang>endline;hang--)
        {
            if((rightline[hang-5]>rightline[hang])&&(rightline[hang+5]>rightline[hang]))
            {
                right_san_cha_hang=hang;
                if(right_san_cha_hang>image_Y*1/4)
                    right_san_cha_flag=True;
                //找到右侧拐点
                break;
            }
        }
        //正入                   
        if(right_san_cha_flag==True&&left_san_cha_flag==True&&(my_abs(left_san_cha_hang-right_san_cha_hang)<=30))
        {
            //判断拐点附近的赛道宽度变化趋势
            //开一个新数组
            uint8 Three_Forks_Road[image_Y][image_X];
            for(uint8 hang=0;hang<image_Y;hang++)
            {
                for(uint8 lie=0;lie<image_X;lie++)
                {
                    Three_Forks_Road[hang][lie] = Threshold_Image[hang][lie];
                }
            }
            //首先开始填充白块
            for(uint8 hang=left_san_cha_hang;hang>0;hang--)
            {
                if(Threshold_Image[hang][image_X/2-1]==black)
                {
                    for(uint8 lie=image_X/2-1;lie>5;lie--)
                    {
                        if(Three_Forks_Road[hang][lie]==black)
                            Three_Forks_Road[hang][lie]=white;
                        else
                            continue ;
                    }
                }
            }
            for(uint8 hang=right_san_cha_hang;hang>0;hang--)
            {
                if(Threshold_Image[hang][image_X/2]==black)
                {
                    for(uint8 lie=image_X/2;lie<image_X-5;lie++)
                    {
                        if(Three_Forks_Road[hang][lie]==black)
                            Three_Forks_Road[hang][lie]=white;
                        else
                            continue ;
                    }
                }
            }
            //扫描赛道宽度 从拐点向上扫描
            for(uint8 hang=left_san_cha_hang;hang>=left_san_cha_hang-25;hang--)
            {
                //左侧白点
                for(uint8 lie=image_X/2;lie>5;lie--)
                {
                    if(Three_Forks_Road[hang][lie]==white)
                    {
                        white_point_L[hang]++;
                    }
                } 
            }
            for(uint8 hang=right_san_cha_hang;hang>=right_san_cha_hang-25;hang--)
            {
                //右侧白点
                for(uint8 lie=image_X/2;lie<image_X-5;lie++)
                {
                    if(Three_Forks_Road[hang][lie]==white)
                    {
                        white_point_R[hang]++;
                    }
                }   
            }
            //扫描趋势  (v2.0 分为左右两侧找点)
            for(uint8 hang=left_san_cha_hang;hang>=left_san_cha_hang-25;hang--)//从下到上
            {
                //左侧
                if(white_point_L[hang]>white_point_L[hang-1])//大到小
                {
                    san_cha_da_L++;
                }
                else if(white_point_L[hang]<white_point_L[hang-1])
                {
                    san_cha_xiao_L++;
                } 
            }
            for(uint8 hang=right_san_cha_hang;hang>=right_san_cha_hang-25;hang--)//从下到上
            {
                 //右侧
                if(white_point_R[hang]>white_point_R[hang-1])//大到小
                {
                    san_cha_da_R++;
                }
                else if(white_point_R[hang]<white_point_R[hang-1])
                {
                    san_cha_xiao_R++;
                }
            }
            //如果为变小再变大的特征   简单的处理了下左入右入
            //if((san_cha_xiao_L>=7&&san_cha_xiao_R>=7)||(san_cha_xiao_L>=18&&san_cha_xiao_R>=3)||(san_cha_xiao_L>=3&&san_cha_xiao_R>=18))//||(san_cha_da_L>=8&&san_cha_xiao_L>=1)||(san_cha_da_R>=8&&san_cha_xiao_R>=1))
            if(san_cha_xiao_L>=7&&san_cha_xiao_R>=7)
            {
                san_cha_flag=1;//初步确定符合三岔特征
            }
            san_cha_xiao_L=0;
            san_cha_da_L=0;
            san_cha_xiao_R=0;
            san_cha_da_R=0;
            
        }
    }
    //二次判断,找三岔特征点(拐点往上丢线)
    else if(san_cha_flag==1)
    {
        //清零
        left_san_cha_lose=0;
        right_san_cha_lose=0;
        san_cha_tiaobian=0;
        right_san_cha_find=0;
        left_san_cha_find=0;
        //丢线点
       for(uint8 hang=left_san_cha_hang;hang>0;hang--)
       {
           if(leftlineflag[hang]!=0&&leftlineflag[hang-1]==0)
             {
               left_san_cha_lose=1;
                 break;
             }
       }
       for(uint8 hang=right_san_cha_hang;hang>0;hang--)
       {
           if(rightlineflag[hang]!=0&&rightlineflag[hang-1]==0)
             {
               right_san_cha_lose=1;
                 break;
             }
       }
      //非丢线点（与十字做区分）(递减非丢线）
        for(uint8 hang=left_san_cha_hang;hang>endline;hang--)
        {
            if(leftlineflag[hang]==2&&(leftline[hang]-leftline[hang+1]<0))
                left_san_cha_find++;
        }
        for(uint8 hang=right_san_cha_hang;hang>endline;hang--)
        {
            if(rightlineflag[hang]==2&&(rightline[hang]-rightline[hang+1]>0))
               right_san_cha_find++;
        }
      //跳变 丢线 斜率 非丢线
       //if((right_san_cha_lose==1)&&(left_san_cha_lose==1))
       //{   
           //if(((left_san_cha_find>=13)&&(right_san_cha_find>=13))||((left_san_cha_find>=2)&&(right_san_cha_find>=18))||((left_san_cha_find>=18)&&(right_san_cha_find>=2)))
        if((left_san_cha_find>=8)&&(right_san_cha_find>=8))
        {
            san_cha_flag=2;
        }
        else
        {
            san_cha_flag=0;
        }
       //}
       // else
       // {
       //    san_cha_flag=0;
       // }
    }
}
uint8 real_endline=0;
void Three_Forks_Road_ru(void)
{
    if(san_cha_flag==2||san_cha_flag==3)
    {
        Buzzer(1);
        //出三岔(趁着左右判断未被覆盖)
        uint8 ulose=0;
        //判断是否已经进入三岔 
        for(uint8 hang=image_Y-10;hang>image_Y/2;hang--)
        {
            if(leftlineflag[hang]==2&&rightlineflag[hang]==2)
            {
                ulose++;
            }
        }
        if(san_cha_flag==3)
        {  
            icm_san_cha_flag = 1;
            //if(ulose>=45)
            if(icm_san_cha>dutyru||icm_san_cha<-dutyru)
            {
                san_cha_flag=0;
                left_san_cha_find=0;
                right_san_cha_find=0;
                real_endline=0;
                Buzzer(0);
                in_san_cha=1; 
            }
        }
        //继续寻找
        for(uint8 hang=image_Y/2;hang>0;hang--)
        {
            if(leftlineflag[hang]==2&&rightlineflag[hang]==2)
            {
                ulose++;
            }
        }
        for(uint8 lie=15;lie<image_X-15;lie++)
        {
           if(Threshold_Image[real_endline][lie]==white&&Threshold_Image[real_endline][lie+1]==black)
               real_endline++;
        }
        real_endline--;//防止过找
        //endline = real_endline;
            //if(real_endline>10)
            if(ulose<40)
            {
                if(Number_of_san_cha==0) //第一次进入三岔 左转
                {
                    san_cha_flag=3;
                    for(uint8 hang=endline;hang<image_Y;hang++)
                    {
                        rightlineflag[hang]=0;
                        if(leftlineflag[hang]==0)
                        {
                            leftlineflag[hang]=2;
                            leftline[hang]=1;
                        }
                    }
                }
                else//第二次右转
                //if(real_endline>10)
                //if(ulose<60)
                {
                    san_cha_flag=3;
                    for(uint8 hang=endline;hang<image_Y;hang++)
                    {
                    leftlineflag[hang]=0;
                    if(rightlineflag[hang]==0)
                        {
                            leftlineflag[hang]=2;
                            leftline[hang]=1;
                            // rightlineflag[hang]=2;
                            // rightline[hang]=image_X-1;
                        }
                    }
                }
            }
            else//取消限制点之上的巡线,防止车乱跑
            {
                for(uint8 hang = 0;hang<image_Y-40;hang++)
                {
                    rightlineflag[hang]=0;
                    leftlineflag[hang]=0;
                }
            }
            
     }
}
uint8 left_san_cha_chu_lost=0,right_san_cha_chu_lost=0;
uint8 encoder_san_cha_flag = 0;
int   encoder_san_cha = 0;
void Three_Forks_Road_chu(void)
{
    left_san_cha_flag=0;
    right_san_cha_flag=0;
    if(in_san_cha==1)
    {

        // for(uint8 hang=image_Y-1;hang>endline;hang--)
        // {
        //     if((leftline[hang-5]<leftline[hang])&&(leftline[hang+5]<leftline[hang]))
        //     {
        //         left_san_cha_hang=hang;
        //         if(left_san_cha_hang>image_Y*1/4)
        //             left_san_cha_flag=True;
        //         //找到左侧拐点
        //         break;
        //     }
        // }
        // for(uint8 hang=image_Y-1;hang>endline;hang--)
        // {
        //     if((rightline[hang-5]>rightline[hang])&&(rightline[hang+5]>rightline[hang]))
        //     {
        //         right_san_cha_hang=hang;
        //         if(right_san_cha_hang>image_Y*1/4)
        //             right_san_cha_flag=True;
        //         //找到右侧拐点
        //         break;
        //     }
        // }

        // if((my_abs(left_san_cha_hang-right_san_cha_hang)<=70)&&right_san_cha_flag==True&&left_san_cha_flag==True)
        // {
        //     uint8 Three_Forks_Road[image_Y][image_X];
        //     for(uint8 hang=0;hang<image_Y;hang++)
        //     {
        //         for(uint8 lie=0;lie<image_X;lie++)
        //         {
        //             Three_Forks_Road[hang][lie] = Threshold_Image[hang][lie];
        //         }
        //     }
        //     memset(white_point_L,0,sizeof(white_point_L));
        //     memset(white_point_R,0,sizeof(white_point_R));
        //     //判断拐点附近的赛道宽度变化趋势
        //     //首先开始填充白块
        //     for(uint8 hang=left_san_cha_hang;hang>0;hang--)
        //     {
        //         if(Threshold_Image[hang][image_X/2-1]==black)
        //         {
        //             for(uint8 lie=image_X/2-1;lie>5;lie--)
        //             {
        //                 if(Three_Forks_Road[hang][lie]==black)
        //                     Three_Forks_Road[hang][lie]=white;
        //                 else
        //                     continue ;
        //             }
        //         }
        //     }
        //     for(uint8 hang=right_san_cha_hang;hang>0;hang--)
        //     {
        //         if(Threshold_Image[hang][image_X/2]==black)
        //         {
        //             for(uint8 lie=image_X/2;lie<image_X-5;lie++)
        //             {
        //                 if(Three_Forks_Road[hang][lie]==black)
        //                     Three_Forks_Road[hang][lie]=white;
        //                 else
        //                     continue ;
        //             }
        //         }
        //     }
        //         for(uint8 hang=0;hang<image_Y;hang++)
        //     {
        //         for(uint8 lie=0;lie<image_X;lie++)
        //         {
        //             Three_Forks_Road[hang][lie] = Threshold_Image[hang][lie];
        //         }
        //     }
        //     //扫描赛道宽度 从拐点向上扫描
        //     for(uint8 hang=left_san_cha_hang;hang>=left_san_cha_hang-25;hang--)
        //     {
        //         //左侧白点
        //         for(uint8 lie=image_X/2;lie>5;lie--)
        //         {
        //             if(Three_Forks_Road[hang][lie]==white)
        //             {
        //                 white_point_L[hang]++;
        //             }
        //         } 
        //     }
        //     for(uint8 hang=right_san_cha_hang;hang>=right_san_cha_hang-25;hang--)
        //     {
        //         //右侧白点
        //         for(uint8 lie=image_X/2;lie<image_X-5;lie++)
        //         {
        //             if(Three_Forks_Road[hang][lie]==white)
        //             {
        //                 white_point_R[hang]++;
        //             }
        //         }   
        //     }
        //     //扫描趋势  (v2.0 分为左右两侧找点)
        //     san_cha_da_L = 0;
        //     san_cha_xiao_L = 0;
        //     san_cha_da_R = 0;
        //     san_cha_xiao_R = 0;
        //     for(uint8 hang=left_san_cha_hang;hang>=left_san_cha_hang-25;hang--)//从下到上
        //     {
        //         //左侧
        //         if(white_point_L[hang]>white_point_L[hang-1])//大到小
        //         {
        //             san_cha_da_L++;
        //         }
        //         else if(white_point_L[hang]<white_point_L[hang-1])
        //         {
        //             san_cha_xiao_L++;
        //         } 
        //     }
        //     for(uint8 hang=right_san_cha_hang;hang>=right_san_cha_hang-25;hang--)//从下到上
        //     {
        //         //右侧
        //         if(white_point_R[hang]>white_point_R[hang-1])//大到小
        //         {
        //             san_cha_da_R++;
        //         }
        //         else if(white_point_R[hang]<white_point_R[hang-1])
        //         {
        //             san_cha_xiao_R++;
        //         }
        //     }
        // }

    
        // //如果三岔距离达到 跳出巡线
        // if(san_cha_xiao_L>=8&&san_cha_xiao_R>=8)
         //西部赛,基于陀螺仪的出三岔算法
         if((icm_san_cha<-dutychu&&Number_of_san_cha==0)||(Number_of_san_cha==1&&icm_san_cha<-dutychu))
        {        
            //出三岔
            in_san_cha=3;
            Buzzer(1);
        }
  
    } 
    // else if(in_san_cha==2)
    // {
    //     uint8 ulose=0;
    //     for(uint8 hang=image_Y-10;hang>0;hang--)
    //     {
    //         if(leftlineflag[hang]==2&&rightlineflag[hang]==2)
    //         {
    //             ulose++;
    //         }
    //     }
    //     if(ulose<80)
    //     {
    //         icm_san_cha_flag = 1;
    //         in_san_cha=3;
    //     }
    
    else if(in_san_cha==3)
    {
        if((icm_san_cha>-dutychu*2/3&&Number_of_san_cha==0)||(Number_of_san_cha==1&&icm_san_cha>-dutychu*2/3))
        {
            encoder_san_cha_flag = 0;
            icm_san_cha_flag = 0;
            in_san_cha=0;
            right_san_cha_chu_lost=0;
            left_san_cha_chu_lost=0;
            Number_of_san_cha++;
            // if(Number_of_san_cha==2)
            // {
            //     Number_of_san_cha=0;
            // }
            san_cha_ok_flag = 1;
            Buzzer(0);
        }
        if(Number_of_san_cha == 0)
            {
                for(uint8 hang=endline;hang<image_Y;hang++)
                {
                    rightlineflag[hang] = 0;
                    if(leftlineflag[hang]==0)
                    {
                         leftlineflag[hang]=2;
                         leftline[hang]=1;
                    }
                }
            }
        else if(Number_of_san_cha == 1)
        {
           for(uint8 hang=endline;hang<image_Y;hang++)
                {
                    rightlineflag[hang] = 0;
                    if(leftlineflag[hang]==0)
                    {
                         leftlineflag[hang]=2;
                         leftline[hang]=1;
                    }
                }
        }
        // uint8 ulose=0;
        // for(uint8 hang=image_Y-10;hang>image_Y/2;hang--)
        // {
        //     if(leftlineflag[hang]==2&&rightlineflag[hang]==2)
        //     {
        //         ulose++;
        //     }
        // }
        //if(ulose>45)
        
    }
}
