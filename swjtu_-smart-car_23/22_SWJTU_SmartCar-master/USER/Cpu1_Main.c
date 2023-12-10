/*
  * 主程序
 *  第十七届大学生智能汽车大赛 SWJTU 四轮摄像头组
 *  Created on: 2022年1月6日
 *      Author: paper
 */
#include "headfile.h"
#pragma section all "cpu1_dsram"
//将本语句与#pragma section all restore语句之间的全局变量都放在CPU1的RAM中
void core1_main(void)
{
  //////////////debug   圆环和串级pid冲突吗?  idea 补线截止行增益.  debug Differential_speed 可能造成的中断赋值冲突 idea 根据Differential_speed 动态拟合ei
    disableInterrupts();
    IfxScuWdt_disableCpuWatchdog(IfxScuWdt_getCpuWatchdogPassword());
    //用户在此处调用各种初始化函数等
    //等待所有核心初始化完毕
    IfxCpu_emitEvent(&g_cpuSyncEvent);
    IfxCpu_waitEvent(&g_cpuSyncEvent, 0xFFFF);
    enableInterrupts();
    while (TRUE)
    {    
        //用户在此处编写任务代码
        if(mt9v03x_finish_flag)
          {
          //获取按键状态
         // Key_scan();//(放中断了)
          //////////////////////////////////////////////////图像处理//////////////////////////////////////////////////////////////////
          //大津法二值化
          Get_Threshold_Image();
          //像素滤波
          //Pixle_Filter();
          //斑马线判别(中断)
          // if(go)
          // {
          //   scratch_line();//已添加三秒延时
          // }
          //起步慢速
          if(go&&start_count<=50)
          {
            Speed_Left = Speed_Left_set/50*start_count;
            Speed_Right = Speed_Right_set/50*start_count;
          }
          //ZW_Send_Image(Threshold_Image);
          //起始巡线
          get_route_first(119,90);
          //跟踪巡线
          get_route_all();
          //寻找截止行
          get_route_endline();
          //////////////////////////////////////////////////元素识别//////////////////////////////////////////////////////////////////
          //曲率及左右丢线判断
          Curvature_determination();
          //出入库判断
          if(go)
          {
            if(Out_Home_flag)
            {
              chu_ku();
            }
            else if(In_Home_flag)
            {
              ru_ku();
            }
          }
          //三叉识别
          if(ringflag==0&&(san_cha_flag==0||san_cha_flag==1)&&san_cha_ok_flag == 0&&Out_Home_flag == 0&&back_loop_flag == 0&&number_of_back_loop!=0)
          {
            Three_Forks_Road_scan();
          }
            Three_Forks_Road_ru();
            Three_Forks_Road_chu();
          //圆环识别
          if(ringflag==0&&ring_ok_flag==0&&san_cha_flag==0&&san_cha_ok_flag==0&&Out_Home_flag == 0&&in_san_cha == 0&&back_loop_flag == 0&&Number_of_san_cha!=0&&number_of_back_loop!=0)
          {
            ring_find();
            
          }
         //   ring_start();
            ring_ru();
            ring_chu();
          //回环识别
          if(ringflag==0&&ring_ok_flag==0&&san_cha_flag==0&&san_cha_ok_flag==0&&Out_Home_flag == 0&&in_san_cha == 0 && start_count>=50&&back_loop_flag == 0)
          {
            back_loop_find();
          }
            back_loop_chu();
          //////////////////////////////////////////////////计算误差//////////////////////////////////////////////////////////////////
          //补中线
          get_route_midline();
          //误差处理(第一次Error的矩阵加权)
          get_deviation();
          //加权转向值(根据赛道类型Error的二次处理/赛道类型判断)
          if((ringflag==0||ringflag==3||ringflag==4||ringflag==13||ringflag==14)&&san_cha_flag==0&&in_san_cha==0)
          {
            Weighted_error();
          }
           //if(encoder_all >= 118861)
                 //warning = 1;
          mt9v03x_finish_flag=0;
          imageover=1;
      }
    }
}
#pragma section all restore
