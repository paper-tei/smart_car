#include "ring.h"
/******************************************************************************
圆环函数：

******************************************************************************/
#define image_Y 120
#define image_X 180
uint8 ring_left_ru   =     1   ;       //   左入   越小越厉害
uint8 ring_left_chu =     60     ;       //   左出1    越小越厉害
uint8 ring_right_ru   =      Col -1   ;       //   右入    越大越厉害
uint8 ring_right_chu =      60    ;       //   右出1    越大越厉害
int32 Error_ring_min = 0;
uint16 R_lost_cnt=0,L_lost_cnt=0,R_find_cnt=0,L_find_cnt=0,ALL_find_cnt=0;
uint16 ring_start_L=Row-1,ring_start_R=Row-1,ring_end_L=Row-1,ring_end_R=Row-1,ring_xie_L=Row-1,ring_xie_R=Row-1;
uint16 ring_start_L_Flag=0,ring_start_R_Flag=0,ring_end_L_Flag=0,ring_end_R_Flag=0,ring_xie_L_Flag=0,ring_xie_R_Flag=0;
uint16 ring_cnt=0,ring_end=0;
uint16 ringflag=0,ring_ok_flag=0;
uint16 ring_tiaobian=0;
uint16 ring_white=0,ring_white_di=0;
int ring_B_L=-40,ring_B_R=40;
uint16 ring_hang_R[Col],ring_hang_L[Col];
uint16 ring_ru_break=0;
uint8 icm_ring_flag=0;
int32 icm_ring =0;
uint8 k_reduce_right_count=0,k_plus_right_count=0,right_ring_first_hang=0;
uint8 white_point_Ring_right=0,white_point_Ring_right_count = 0;
void ring_start(void)
{
  //处理西部赛的急弯圆环丢线问题
  //右边线趋势
  for(uint8 hang = image_Y-20;hang>image_Y-60;hang--)
  {
    k_reduce_right_count=0;
    k_plus_right_count=0;
    right_ring_first_hang = 0;
    for(uint8 hang_1 = hang+15;hang_1>=hang-15;hang_1--)
    {
      if(rightline[hang_1]>rightline[hang_1-1]&&rightlineflag[hang_1]!=0) //k<0 
      {
        k_reduce_right_count++;
      }
      if(rightline[hang_1]<rightline[hang_1-1]&&rightlineflag[hang_1]!=0) //k>0
      {
        k_plus_right_count++;
      }
    }
    if((k_reduce_right_count>=8&&k_plus_right_count>=3)&&(rightline[hang+10]<rightline[hang-10]))//找到拐点
    {
      right_ring_first_hang = hang;
      break;
    }
  }//往上巡线
  if(right_ring_first_hang)
  {
    white_point_Ring_right = 0;
    for(uint8 hang = right_ring_first_hang;hang>endline;hang--)
    {
      //白点
      white_point_Ring_right_count = 0;
      for(uint8 lie=0;lie<image_X;lie++)
      {
          if(Threshold_Image[hang][lie]==white)
          {
              white_point_Ring_right_count++;
          }
      }   
      if(white_point_Ring_right_count>image_Y*1/2&&leftlineflag[hang]==0&&leftlineflag[image_Y-5]==0)
      {
        white_point_Ring_right++;
      }
      if(white_point_Ring_right >= 15 && san_cha_flag == 0 && Number_of_san_cha == 1 && first_way == 0 && in_san_cha == 0)
      {
        //强制补线
        uint8 k = 0;
        for(uint8 hang = right_ring_first_hang-1;hang>endline;hang--)
        {
          k++;
          rightline[hang] = rightline[right_ring_first_hang]-k;
        }
      }
      else
      {
         
      }
    }
  }
}

//圆环识别
void ring_find(void)
{
    
  R_lost_cnt=0,L_lost_cnt=0,ALL_find_cnt=0,R_find_cnt=0,L_find_cnt=0;
  ring_B_L=0,ring_B_R=0;
  ring_cnt=0;
  ring_start_L=Row-1,ring_start_R=Row-1,ring_end_L=Row-1,ring_end_R=Row-1,ring_xie_L=Row-1,ring_xie_R=Row-1;
  ring_start_L_Flag=0,ring_start_R_Flag=0,ring_end_L_Flag=0,ring_end_R_Flag=0,ring_xie_L_Flag=0,ring_xie_R_Flag=0;
  ring_tiaobian=0;
  ring_white=0,ring_white_di=0;

  //左起始点
  for(uint8 hang=Row-7;hang>endline+2;hang--)
  {
    if(leftlineflag[hang]!=0&&leftlineflag[hang-1]==0&&leftlineflag[hang-2]==0)
    {
        //从下到上丢线
      ring_start_L=hang;
      ring_start_L_Flag=1;
      break;
    }
  }
  //左终点
  if(ring_start_L_Flag==1)
  {
    //左终点
    for(uint8 hang=ring_start_L;hang>endline;hang--)
    {
        //从下到上找到线
      if(leftlineflag[hang+2]==0&&leftlineflag[hang+1]!=0&&leftlineflag[hang]!=0)
      {
        ring_end_L=hang;
        ring_end_L_Flag=1;
        break;
      }
    }
    //左斜点
    for(uint8 hang=Row-5;hang>ring_start_L;hang--)
    {
       
      if(leftlineflag[hang+5]!=0&&leftlineflag[hang]!=0)
      {
        if(leftline[hang+5]-leftline[hang]>0)
        {
         ring_xie_L=hang;
         ring_xie_L_Flag=1;
         break;
        }
      }
    }
  }
  //右起始点
  for(uint8 hang=Row-7;hang>endline+2;hang--)
  {
    if(rightlineflag[hang]!=0&&rightlineflag[hang-1]==0&&rightlineflag[hang-2]==0)
    {
      ring_start_R=hang;
      ring_start_R_Flag=1;
      break;
    }
  }
  //右终点
  if(ring_start_R_Flag==1)
  {
    //右终点
    for(uint8 hang=ring_start_R;hang>endline;hang--)
    {
      if(rightlineflag[hang+2]==0&&rightlineflag[hang+1]!=0&&rightlineflag[hang]!=0)
      {
        ring_end_R=hang;
        ring_end_R_Flag=1;
        break;
      }
    }
    //右斜点
    for(uint8 hang=Row-5;hang>ring_start_R;hang--)
    {
       if(rightlineflag[hang+5]!=0&&rightlineflag[hang]!=0)
      {
        if(rightline[hang+5]-rightline[hang]<0)
        {
         ring_xie_R=hang;
         ring_xie_R_Flag=1;
         break;
        }
      }
    }
  }
  //左圆环 三点都找到
  if(ring_start_L_Flag==1&&ring_end_L_Flag==1&&ring_xie_L_Flag==1&&ring_start_R_Flag==0&&ring_end_R_Flag==0&&ring_xie_R_Flag==0&&endline<=25&&Error>=-20&&Error<=20)//22  &&ring_start_L>24
  {

    //入口左右丢线 左全丢右全不丢
    for(uint8 hang=ring_start_L-2;hang>ring_end_L+2;hang--)
    {
      if(rightlineflag[hang]==0)
        R_lost_cnt++;
      if(leftlineflag[hang]==1)
        L_find_cnt++;
    }
    //最底行白点数较少 防止斜入十字误判
    for(uint8 hang=110;hang>100;hang--)
    {
      // if(rightline[hang]>=178)
      //   ring_white_di++;
    }
    //截止行向上找全线 防止小弯误判
    for(uint8 hang=ring_end_L;hang>0;hang--)
    {
      if(leftlineflag[hang]!=0&&rightlineflag[hang]!=0)
        ALL_find_cnt++;
    }

    //找跳变 白黑黑白跳变 圆环第一特征
    for(uint8 hang=ring_xie_L;hang>endline+2;hang--)
    {
      if(Threshold_Image[hang][leftline[ring_xie_L]/2]==black&&Threshold_Image[hang-1][leftline[ring_xie_L]/2]==white)
        ring_tiaobian++;
      if(Threshold_Image[hang][leftline[ring_xie_L]/2]==white&&Threshold_Image[hang-1][leftline[ring_xie_L]/2]==black)
        ring_tiaobian++;
    }

    if(ring_tiaobian>=2&&R_lost_cnt<=1&&L_find_cnt<=2&&ring_white<=2&&ring_white_di<=2&&ring_start_L-ring_end_L>=20&&ALL_find_cnt>=5)
    {
       
       //寻找左趋势线
       for(uint8 hang=ring_xie_L;hang>ring_end_L;hang--)
       {
           if(leftline[hang]>leftline[hang-1])
           {
               ring_cnt++;
           }
       }
      if(ring_cnt>=10&&ring_end_L>=28&&number_of_back_loop == 2)
      {
        ringflag=1;
        icm_ring_flag = 1;
        Buzzer(1);


      }
    }
  }
  //右圆环
  if(ring_start_L_Flag==0&&ring_end_L_Flag==0&&ring_xie_L_Flag==0&&ring_start_R_Flag==1&&ring_end_R_Flag==1&&ring_xie_R_Flag==1&&endline<=15&&Error>=-20&&Error<=20)//&&ring_start_R>24
  {
    //入口左右丢线 左全丢右全不丢
    for(uint8 hang=ring_start_R-2;hang>ring_end_R+2;hang--)
    {
      if(rightlineflag[hang]==1)
        R_find_cnt++;
      if(leftlineflag[hang]==0)
        L_lost_cnt++;
    }
    //最底行白点数较少 防止斜入十字误判
    for(uint8 hang=110;hang>100;hang--)
    {
      // if(leftline[hang]<=2)
      //   ring_white_di++;
    }
    //截止行向上找全线 防止小弯误判
    for(uint8 hang=ring_end_R;hang>0;hang--)
    {
      if(leftlineflag[hang]!=0&&rightlineflag[hang]!=0)
        ALL_find_cnt++;
    }
    //找跳变 白黑黑白跳变 圆环第一特征
    for(uint8 hang=ring_xie_R;hang>endline+2;hang--)
    {
      if(Threshold_Image[hang][(Col+rightline[ring_xie_R])/2]==black&&Threshold_Image[hang-1][(Col+rightline[ring_xie_R])/2]==white)
        ring_tiaobian++;
      if(Threshold_Image[hang][(Col+rightline[ring_xie_R])/2]==white&&Threshold_Image[hang-1][(Col+rightline[ring_xie_R])/2]==black)
         ring_tiaobian++; 
               
    }
    //判断条件找小环标志位
     

    if(ring_tiaobian>=2&&R_find_cnt<=1&&L_lost_cnt<=2&&ring_white<=2&&ring_white_di<=2&&ring_start_R-ring_end_R>=20&&ALL_find_cnt>=5)
    {
      for(uint8 hang=ring_xie_R;hang>ring_end_R;hang--)
       {
           if(rightline[hang]<rightline[hang-1])
           {
               ring_cnt++;
           }
       }
               
      if(ring_cnt>=10&&ring_end_R>=24&&number_of_back_loop == 1)
      {
        ringflag=2;
        icm_ring_flag = 1;
        Buzzer(1);
      }
    }
  }


}
uint8 left_ru_point=0;
//入圆环补线
//入圆环补线
void ring_ru(void)
{

  for(uint8 lie=0;lie<Col;lie++)
  {
    ring_hang_R[lie]=0;
    ring_hang_L[lie]=0;
  }

  //左圆环
   if(ringflag==1)
   {
     //找终点
     for(uint8 hang=Row-2;hang>endline+3;hang--)
     {
       if(leftlineflag[hang]==0&&leftlineflag[hang-1]==0&&leftlineflag[hang-2]!=0)
       {
         ring_end_L=hang;
         ring_end_L_Flag=1;
         break;
       }
     }
     //截止行查找
     //找左边白点数
     int lost=0;
     for(uint8 hang=Row-5;hang>45;hang--)
     {
       if(leftlineflag[hang]==0||leftline[hang]<5)
       {
         lost++;
       }
     }
     if(lost>=15)
     {
       //截止行标定
       for(uint8 hang=Row-60;hang>endline;hang--)
       {
         if(Threshold_Image[hang][20]==black&&Threshold_Image[hang+1][20]==black)
         {
           endline=hang+2;
           break;
         }
       }
     }
     else
     {
       //截止行标定
       for(uint8 hang=Row-60;hang>endline;hang--)
       {
         if(Threshold_Image[hang][45]==black&&Threshold_Image[hang+1][45]==black)
         {
           endline=hang+2;
           break;
         }
       }
     }
    ring_ru_break = 0;
    for(uint8 hang=Row-5;hang>endline;hang--)
    {
      if(rightlineflag[hang]==0&&endline>=15)
      {
        ring_ru_break++;
      }
      if(Threshold_Image[hang][Col-5]==white)
      {
        ring_ru_break++;
      }
    }
    //计算斜率
    // float k=0,kk=0;
    // k=(float)(ring_left_ru-Speed_Left/10-endline-120)/(55-endline);//k为负值
    // //补线
    // for(uint8 hang=55;hang>endline;hang--)
    // {
    //     kk+=k;
    //     leftline[hang]=1;leftlineflag[hang]=0;
    //     rightline[hang]=119+kk;rightlineflag[hang]=2;
        
    //     if(rightline[hang]>=119) rightline[hang]=119;
    //     if(leftline[hang]<=1) leftline[hang]=1;
    // }
    for(uint8 hang=Row-1;hang>=endline;hang--)
    {
      rightlineflag[hang]=0;
      if(leftlineflag[hang]==0)
      {
        leftlineflag[hang]=2;
        leftline[hang]=ring_left_ru;
      }
    }
    //清标志
    if(icm_ring>350000) 
    {
      ringflag=3;
    }
      
  }
   //右圆环
   else if(ringflag==2)
   {

     for(uint8 hang=Row-2;hang>endline+3;hang--)
     {
       if(rightlineflag[hang]==0&&rightlineflag[hang-1]==0&&rightlineflag[hang-2]!=0)
       {
         ring_end_R=hang;
         ring_end_R_Flag=1;
         break;
       }
     }
     //截止行查找
     //找右边白点数
     int lost=0;
     for(uint8 hang=Row-5;hang>45;hang--)
     {
       if(rightlineflag[hang]==0||rightline[hang]>Col-5)
       {
         lost++;
       }
     }
     if(lost>=15)
     {
       //截止行标定
       for(uint8 hang=Row-60;hang>endline;hang--)
       {
         if(Threshold_Image[hang][Col-20]==black&&Threshold_Image[hang+1][Col-20]==black)
         {
           endline=hang+2;
           break;
         }
       }
     }
     else
     {
       //截止行标定
       for(uint8 hang=Row-60;hang>endline;hang--)
       {
         if(Threshold_Image[hang][Col-45]==black&&Threshold_Image[hang+1][Col-45]==black)
         {
           endline=hang+2;
           break;
         }
       }
     }
    
    
    //清标志位
    ring_ru_break = 0;
    for(uint8 hang=Row-5;hang>endline;hang--)
    {
      if(leftlineflag[hang]==0&&endline>=15)
      {
        ring_ru_break++;
      }
      if(Threshold_Image[hang][5]==white)
      {
        ring_ru_break++;
      }
    }

     //计算斜率
    // float k=0,kk=0;
    // k=(float)(ring_right_ru+Speed_Left/10+endline)/(55-endline);//k为正值
    // //补线
    // for(uint8 hang=55;hang>endline;hang--)
    // {
    //     kk+=k;
    //     rightline[hang]=1;rightlineflag[hang]=0;
    //     leftline[hang]=kk;leftlineflag[hang]=2;
        
    //     if(rightline[hang]>=119) rightline[hang]=119;
    //     if(leftline[hang]<=1) leftline[hang]=1;
    // }
    for(uint8 hang=Row-1;hang>=endline;hang--)
    {
      leftlineflag[hang]=0;
      if(rightlineflag[hang]==0)
      {
        rightlineflag[hang]=2;
        rightline[hang]=ring_right_ru;
      }
    }
    if(icm_ring<-350000) 
    {
      ringflag=4; 
    }
  }
}
int ring_chu_num=0;
uint8 number_of_ring = 0;
void ring_chu(void)
{
  ring_chu_num=0;
  ring_end=0;
  //左圆环
  if(ringflag==3||ringflag==13)
  {
   
    //出环标志
    for(uint8 hang=Row-1;hang>endline;hang--)
    {
      if(leftlineflag[hang]==0&&rightlineflag[hang]==0)
        ring_chu_num++;
    }
    //出环打角
    if(icm_ring>=2200000)
    {
      ringflag=13;
      for(uint8 hang=Row-1;hang>endline;hang--)
      {
        leftline[hang]=0;
        rightline[hang]=ring_left_chu-endline; 
        if(rightline[hang]>=Col-1) rightline[hang]=Col-1;
        if(leftline[hang]<=1) leftline[hang]=1;
      }
    }
     if(icm_ring>=2600000)
     {
      icm_ring_flag =0;
      ring_end=1;
      ringflag=0;
      Buzzer(0);
      Error_ring_min  = 0;
      min_break_L=0;
      ring_ok_flag=1;//进入定时器操作
      number_of_ring++;
    }
  }
  //右圆环
  else if(ringflag==4||ringflag==14)
  {
     
     //出环标志
    for(uint8 hang=Row-1;hang>endline;hang--)
    {
      if(leftlineflag[hang]==0&&rightlineflag[hang]==0)
        ring_chu_num++;
    }
    if(icm_ring<=-2200000)
    {
      ringflag=14;
      for(uint8 hang=Row-1;hang>endline;hang--)
      {
        leftline[hang]=ring_right_chu+endline;
        rightline[hang]=Col-1;
        if(rightline[hang]>=Col-1) rightline[hang]=Col-1;
        if(leftline[hang]<=1) leftline[hang]=1;
      }
    }
    if(icm_ring<=-2600000)
     {
      icm_ring_flag =0;
      ring_end=1;
      ringflag=0;
      Error_ring_min = 0;
      min_break_R=0;
      Buzzer(0);
      ring_ok_flag=1;//进入定时器操作
      number_of_ring++;
    }
  }

  

}


