/*********************************************************************************************************************
 * TC264 Opensourec Library 即（TC264 开源库）是一个基于官方 SDK
 *接口的第三方开源库 Copyright (c) 2022 SEEKFREE 逐飞科技
 *
 * 本文件是 TC264 开源库的一部分
 *
 * TC264 开源库 是免费软件
 * 您可以根据自由软件基金会发布的 GPL（GNU General Public License，即
 *GNU通用公共许可证）的条款 即 GPL 的第3版（即
 *GPL3.0）或（您选择的）任何后来的版本，重新发布和/或修改它
 *
 * 本开源库的发布是希望它能发挥作用，但并未对其作任何的保证
 * 甚至没有隐含的适销性或适合特定用途的保证
 * 更多细节请参见 GPL
 *
 * 您应该在收到本开源库的同时收到一份 GPL 的副本
 * 如果没有，请参阅<https://www.gnu.org/licenses/>
 *
 * 额外注明：
 * 本开源库使用 GPL3.0 开源许可证协议 以上许可申明为译文版本
 * 许可申明英文版在 libraries/doc 文件夹下的 GPL3_permission_statement.txt
 *文件中 许可证副本在 libraries 文件夹下 即该文件夹下的 LICENSE 文件
 * 欢迎各位使用并传播本程序 但修改内容时必须保留逐飞科技的版权声明（即本声明）
 *
 * 文件名称          cpu0_main
 * 公司名称          成都逐飞科技有限公司
 * 版本信息          查看 libraries/doc 文件夹内 version 文件 版本说明
 * 开发环境          ADS v1.8.0
 * 适用平台          TC264D
 * 店铺链接          https://seekfree.taobao.com/
 *
 * 修改记录
 * 日期              作者                备注
 * 2022-09-15       pudding            first version
 ********************************************************************************************************************/
#include "init.h"
#include "zf_common_headfile.h"
#pragma section all "cpu0_dsram"
#define IMAGE 22//巡线算法选择
uint8_t show[RESULT_ROW][RESULT_COL];
uint8_t flag_Crossroad = 0;
#define IPCH 60 // 上位机显示图像高度
#define IPCW 80 // 上位机显示图像宽度
#define RED 1
#define BLUE 2
#define YELLOW 3
#define GREEN 4


float Mh = MT9V03X_H;
float Lh = IPCH;
float Mw = MT9V03X_W;
float Lw = IPCW;
uint8 Image_Use[IPCH][IPCW];


uint32 time_start = 0;//车出车库时候为起始时间
uint32 Distance_Garge = 500;//距离车库距离开摄像头

void Compressimage() {
    int i, j, row, line;
    const float div_h = Mh / Lh, div_w = Mw / Lw;
    for (i = 0; i < IPCH; i++) {
        row = i * div_h + 0.5;
        for (j = 0; j < IPCW; j++) {
            line = j * div_w + 0.5;
            Image_Use[i][j] = mt9v03x_image[row][line];
        }
    }
}
void core0_main(void) {
    clock_init();  // 获取时钟频率<务必保留>
    debug_init();
    Init();
    cpu_wait_event_ready();  // 等待所有核心初始化完毕
    system_start();
    track_width_real();
    //STOP_BLDC(1);
    //卡尔漫滤波
    KalmanCreate(&Zero_Kalman, 10, 200);
    Emergency_flag = 0;
    while (TRUE) {
        //uint32 time1 = system_getval();

        if (mt9v03x_finish_flag) {
#if IMAGE == 22
            Get_Threshold_Image();
            //起始巡线
            get_route_first(MT9V03X_H - 5, MT9V03X_W / 2);
            //跟踪巡线
            get_route_all();
            //寻找截止行
            get_route_endline();
            //曲率及左右丢线判断
            Curvature_determination();
            ////


           // roadblock_recognition();


            ////
            //元素判断
            if (ring.ring_state == FINDING && cross.crossroad_state == FINDING && roadblock.roadblock_state == FINDING && go_flag == 1 && out_home_flag == 3 && car_scan_mode == 0) {
                crossroad_recognition();//判断十字
            }
            if (ring.ring_state == FINDING && cross.crossroad_state == FINDING && roadblock.roadblock_state == FINDING && go_flag == 1 && out_home_flag == 3 && car_scan_mode == 0) {
                ring_recognition();//判断圆环
            }
            if (ring.ring_state == FINDING && cross.crossroad_state == FINDING && roadblock.roadblock_state == FINDING && go_flag == 1 && out_home_flag == 3) {
                roadblock_recognition();//坡道,路障,断路判断
            }
            if (ring.ring_state == FINDING && cross.crossroad_state == FINDING && roadblock.roadblock_state == FINDING) {
                out_home();
                //in_home();
            }
            if (ring.ring_state != FINDING || cross.crossroad_state != FINDING || roadblock.roadblock_state != FINDING) {
                // Buzzer(1);
            }
            else {
                Buzzer(0);
            }
            // scratch_line();//判断车库
                 //纯电磁模式

            ring_handler();//圆环处理
            crossroad_handler();//十字处理
            roadblock_handler();//路障处理
            breaker_handler();//断路处理


            //补中线
            get_route_midline();
            //误差处理(第一次Error的矩阵加权)
            get_deviation();
            //赛道弯曲判断加权误差
            Weighted_error();//摄像头判断直路弯道，圆环出入
#elif IMAGE == 23
            //迷宫扫线
            Bottom_Tracing();
            //三角滤波
            Triangular_filtering(left_line, Tracing_Time_L, left_line_Triangular_filtering, 5);
            Triangular_filtering(right_line, Tracing_Time_R, right_line_Triangular_filtering, 5);
            //等距滤波
            int rpts0s_num = sizeof(left_line_Equidistant_sampling) / sizeof(left_line_Equidistant_sampling[0]);
            int rpts1s_num = sizeof(right_line_Equidistant_sampling) / sizeof(right_line_Equidistant_sampling[0]);
            Equidistant_sampling(left_line_Triangular_filtering, Tracing_Time_L, left_line_Equidistant_sampling, &rpts0s_num, sample_dist * pixel_per_meter);
            Equidistant_sampling(right_line_Triangular_filtering, Tracing_Time_R, right_line_Equidistant_sampling, &rpts1s_num, sample_dist * pixel_per_meter);
            //曲率计算
            local_angle_po ints(left_line_Equidistant_sampling, Tracing_Time_L, left_line_angle, (int)(round)(angle_dist / sample_dist));
            local_angle_points(right_line_Equidistant_sampling, Tracing_Time_R, right_line_angle, (int)(round)(angle_dist / sample_dist));
            //非极大抑制
            nms_angle(left_line_angle, Tracing_Time_L, left_line_angle_out, (int)(round)(angle_dist / sample_dist));
            nms_angle(right_line_angle, Tracing_Time_R, right_line_angle_out, (int)(round)(angle_dist / sample_dist));
            // 左右中线跟踪
            track_leftline(left_line_Equidistant_sampling, rpts0s_num, left_line_middle, (int)(round)(angle_dist / sample_dist), pixel_per_meter * road_width / 2);
            //int rptsc0_num = rpts0s_num;
            track_rightline(right_line_Equidistant_sampling, rpts1s_num, right_line_middle, (int)(round)(angle_dist / sample_dist), pixel_per_meter * road_width / 2);
            //int rptsc1_num = rpts1s_num;
            //左右边线特征提取
            //line_flag_find();
            //描线
            drawing(left_line_Equidistant_sampling, right_line_Equidistant_sampling, left_line_middle, right_line_middle);
            for (int i = 0; i < RESULT_ROW; i++) {
                for (int j = 0; j < RESULT_COL; j++) {
                    show[i][j] = ImageUsed[i][j];
                }
            }
#endif
            mt9v03x_finish_flag = 0;
        }
        if (go_flag == 0) {
            if (debug.flag == 0) {
                ips200_show_gray_image(0, 0, Threshold_Image[0], MT9V03X_W, MT9V03X_H, MT9V03X_W, MT9V03X_H, 0);
                //Compressimage();
                //ipc_send_image(Image_Use, 60, 80);
                ips200_show_float(0, 130, endline, 4, 6);
                ips200_show_float(50, 130, image_Error, 4, 6);
                // ips200_show_float(0, 150, BlackEndL, 4, 6);
                // ips200_show_float(50, 150, BlackEndM, 4, 4);
                // ips200_show_float(0, 170, BlackEndR, 4, 4);
                // ips200_show_float(50, 170, navigation.y_cur, 4, 4);
                //  ips200_show_float(0, 150, roadblock.roadblock_state, 8, 2);
                // ips200_show_float(50, 150, navigation.inertial_navigation_set_distance - navigation.inertial_navigation_cur_distance, 4, 4);
                // ips200_show_float(0, 170, servo_pwm, 4, 4);
                // ips200_show_float(50, 170, disgy_AD_val_1, 4, 4);
                ips200_show_float(0, 150, disgy_AD_val_1, 6, 4);
                ips200_show_float(100, 150, disgy_AD_val_2, 6, 4);
                ips200_show_float(0, 170, disgy_AD_val_3, 4, 4);
                ips200_show_float(100, 170, disgy_AD_val_4, 4, 4);


                ips200_show_float(0, 190, AD_val_1, 4, 4);
                ips200_show_float(100, 190, AD_val_2, 4, 4);
                ips200_show_float(0, 210, AD_val_3, 4, 4);
                ips200_show_float(100, 210, AD_val_4, 4, 4);
                ips200_show_float(0, 230, Threshold_Image_black, 6, 4);
                ips200_show_float(100, 230, Gate, 4, 4);
                // ips200_show_float(0, 210, navigation.inertial_navigation_set_yaw_up_total, 4, 4);
                // ips200_show_float(0, 230, navigation.yaw, 4, 4);
                 //ips200_show_float(100, 230, navigation.inertial_navigation_set_yaw_up, 4, 4);
                 // ips200_show_float(0, 250, navigation.inertial_navigation_set_yaw, 4, 6);
                 // ips200_show_float(100, 250, navigation.cur_yaw, 4, 6);
                 // ips200_show_float(0, 270, ((rightline[118] - leftline[118]) - (rightline[58] - leftline[58])) / 60.f, 4, 6);
                 //ips200_show_float(100, 270, my_abs((navigation.cur_yaw - navigation.inertial_navigation_set_yaw_up)), 6, 6);
                 // ips200_show_float(0, 170, AD_val_1_min, 4, 6);
                 // ips200_show_float(50, 170, AD_val_2_min, 4, 6);
                 //ips200_show_float(0, 250, Error_Em.adc_sum, 4, 6);
                ips200_show_float(0, 290, (eulerAngle_pitch + ZHONGZHI_PITCH), 4, 6);
                ips200_show_float(100, 290, (eulerAngle_roll + ZHONGZHI_roll), 4, 6);
                // ips200_show_float(100, 290, car_mode.now_mode, 4, 6);
                Draw_line();
            }
            else {
                debug_display();
            }
        }
        // ips200_show_float(0, 190, navigation.cur_yaw, 4, 4);
        // ips200_show_float(0, 190,  navigation.inertial_navigation_cur_distance, 8, 4);
        // ips200_show_float(100, 190,navigation.inertial_navigation_set_distance, 6, 4);

        // ips200_show_float(0, 210, test1, 8, 4);
        //  ips200_show_float(0, 230,car_mode.now_mode, 4, 4);
        //  ips200_show_float(100, 230, test, 4, 4);

                //  ips200_show_float(100, 230, navigation.inertial_navigation_set_yaw_up, 4, 4);
                 // ips200_show_float(0, 250, navigation.inertial_navigation_set_yaw, 4, 6);
                 // ips200_show_float(100, 250, navigation.cur_yaw, 4, 6);
//-----------------------------------------上位机发送部分----------------------------------------//
#if PID_ASSISTANT_EN
        // 下位机状态
        Data_uploaded_to_computer((int)(Dynamic_zero_set * 10), (int)image_Error, CURVES_CH1);//调角速度环PID通道1
        Data_uploaded_to_computer((int)(Dynamic_zero * 10), (int)pidout_3f, CURVES_CH2);//调角度环PID通道2
        Data_uploaded_to_computer((int)(icm_data_gyro_y * 10), (int)eulerAngle_roll, CURVES_CH3);//调速度环PID通道3
        receiving_process();
        //串口接收
        //  uart_lower_computer();// 野火上位机的接收中断处理函数，使用上位机的时候不开，测试用
        // 给上位机发送目标值数据，功能一般用不到
        if (protocol_flag == 1) {
            //  Data_uploaded_to_computer((int)pidout_2f, (int)-icm_data_gyro_y, CURVES_CH1);//调角速度环PID通道1
            //  Data_uploaded_to_computer(pidout_2f, -icm_data_gyro_y, CURVES_CH1);//调角速度环PID通道1
            //  Data_uploaded_to_computer((int)(pidout_1f + Mechanical_zero), (int)-eulerAngle_roll, CURVES_CH2);//调角度环PID通道2
            //  Data_uploaded_to_computer((int)pidout_1f + Mechanical_zero, (int)(encoder_bldc_f + encoder_bldc_b) / 2, CURVES_CH3);//调速度环PID通道3
            //  Data_uploaded_to_computer((int)eulerAngle_pitch, (int)icm_data_gyro_y, CURVES_CH4);//调前进环PID通道4
            //  Data_uploaded_to_computer((int)-bdc_speed, (int)encoder_bdc, CURVES_CH5);//调前进环PID通道5
            //  Data_uploaded_to_computer((int)pidout_4f, (int)image_Error, CURVES_CH1);//调角速度环PID通道1
            //   buzzer_uart();//蜂鸣器提示声，//收到上位机
        }
#endif
    }
}

#pragma section all restore
/**************************** 代码区域 ****************************/
