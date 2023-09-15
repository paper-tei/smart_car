#include "home.h"


#define uchar unsigned char
unsigned char Out_Home_flag = 0;
unsigned char In_Home_flag = 0;
unsigned char icm_Home_flag = 0;
float icm_home = 0;
int home_range = 300000;
uchar K = 2;
#define image_Y 120
#define image_X 180
void chu_ku(void)//按下发车键即满足Out_Home_flag = 1;转向90°后置0标志.
{
    //跳出状态
    if(icm_home>=home_range)
    {
        Out_Home_flag = 0;
        icm_Home_flag = 0;
        icm_home = 0;
        Buzzer(0);
    }
    //出库误差处理
    //左出库 左边线置零 右边线根据截止行(重新遍历行白点确认)计算
    if(Out_Home_flag)
    {
        for(uchar hang = image_Y-1;hang>endline;hang--)
        {
            Buzzer(1);
            leftline[hang] = 0;
            rightline[hang] = 70;
            leftlineflag[hang] = 2;
            rightlineflag[hang] = 2;
        }
    }
    
}
 
void ru_ku(void)
{
    //跳出状态
    if(icm_home>=home_range)
    {
        for(uchar hang = image_Y-1;hang>endline;hang--)
        {
            leftline[hang] = 0;
            rightline[hang] = 180;
            leftlineflag[hang] = 2;
            rightlineflag[hang] = 2;
        }
    Buzzer(0);
    }
    else
    {
        for(uchar hang = image_Y-1;hang>endline;hang--)
        {
        icm_Home_flag = 1;
        Buzzer(1);
        leftline[hang] = 0;
        rightline[hang] = 120;
        leftlineflag[hang] = 2;
        rightlineflag[hang] = 2;
        }
    }
    
}
    
        