/*
 * In_out_bound.c
 *
 *  Created on: 2022年2月18日
 *      Author: paper
 */
#include "In_out_bound.h"
int scratch_cnt=0;
uint8 Zebra_flag=0;
#define white 255
#define black 0
uint8 hall_detection_flag = 0;  
int in_home_hang = 0;
void scratch_line(void)
{
  scratch_cnt=0;
  if(Number_of_san_cha==2)
  {
    //起跑线跳变检测
      for(uint8 hang = endline;hang < 120;hang++)
      {
         for(uint8 lie = leftline[hang];lie< rightline[hang];lie++)
        {
          if(Threshold_Image[hang][lie] == white && Threshold_Image[hang][lie + 1] == black)
          scratch_cnt++;
        }
       if(scratch_cnt>=130)
        {
            Buzzer(1);
            Zebra_flag = 1;
           // In_Home_flag = 1;
            break;
        }

    }
  }

  //判断起跑线
   if(Zebra_flag==1)
    {
        //入库左线寻
        if(leftlineflag[119]!=0)
        {
            for(uint8 hang = 119;hang>endline+5;hang--)
            {
                if(leftlineflag[hang] != 0 && leftlineflag[hang-1] == 0 && leftlineflag[hang-2] == 0)
                {
                  in_home_hang = hang;
                  if(in_home_hang>20)
                  {
                    In_Home_flag=1;
                  }

                }
            }
        }
    }
  //     static uint8 hall_detection_count = 0;

	// if(gpio_get(P10_5)){hall_detection_count = 0;}
	// else if(!gpio_get(P10_5) && hall_detection_count < 3){hall_detection_count++;Buzzer(1);}

	// if(hall_detection_count >= 3)hall_detection_flag = 1;
	// else hall_detection_flag = 0;     
  // //判断起跑线
  // if(hall_detection_flag)
  // {
  //     Zebra_flag = 1;
  //     scratch_cnt = 0;
  //    // warning = 1;
  // }
  
}
