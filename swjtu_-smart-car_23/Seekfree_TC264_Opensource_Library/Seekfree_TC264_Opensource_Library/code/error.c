
#include "error.h"
//赛道中值
#define basic 90
#define my_abs(num) (((num)>0)?(num):-(num))
#define Width_wandao  ((1+(Speed_Left-150)*0.008)) //1.6
int16 yawspeed = 0;
//权重 从上到下
uint8 Weight[120] = {
                               10,10,10,10,10,10,10,10,10,10,
                                30,30,30,30,30,30,30,30,30,30,
                              30,30,30,30,30,30,30,30,30,30,
                               30,30,30,30,30,30,30,30,30,30,
                               30,30,30,30,30,30,30,30,30,30,
                              40,40,40,40,40,40,40,40,40,40,
                               50,50,50,50,50,50,50,50,50,50,
                               40,40,40,40,40,40,40,40,40,40,
                               40,40,40,40,40,40,40,40,40,40,
                               20,20,20,20,20,20,20,20,20,20,
                               10,10,10,10,10,10,10,10,10,10,
                               10,10,10,10,10,10,10,10,10,10,
};
float straight_flag_error = 0;
float image_Error = 0;//摄像头偏差，右拐为正，左拐为负
int32 LastError = 0;
int32 WeightSum = 0;
float CenterMeanValue = 90; //理想中线值
int32 CenterSum = 0;
uint8 Track_Type = 0, Track_Type_last = 0;
float image_low_pass = 0.6;
//偏差加权
void get_deviation(void) {
    CenterSum = 0;
    CenterMeanValue = 0;
    WeightSum = 0;
    for (uint8 hang = image_Y - 1;hang > endline;hang--) {
        if (leftlineflag[hang] != 0 || rightlineflag[hang] != 0) {
            CenterSum += midline[hang] * Weight[hang];
            WeightSum += Weight[hang];
        }
    }
    //算出加权平均后中线的值
    if (WeightSum != 0) {
        CenterMeanValue = (CenterSum / WeightSum);
    }
    float image_Error_get = CenterMeanValue - basic;
    //if (out_home_flag != 0) {


    //image_Error = limit(image_Error, -40, 40);
    image_Error = image_Error * image_low_pass + image_Error_get * (1 - image_low_pass);
    image_Error = 0;
    //  }
}
uint8 unlost_R = 0, unlost_L = 0, min_break_L = 0, min_break_R = 0;
uint32 Wobble_time_start = 0;
//赛道弯曲判断加权误差，该函数最好放在圆环处理后面
void Weighted_error(void) {
    unlost_L = 0;
    unlost_R = 0;
    for (uint8 hang = image_Y - 1;hang > endline;hang--) {
        if (leftlineflag[hang] != 0) {
            unlost_L++;
        }
        if (rightlineflag[hang] != 0) {
            unlost_R++;
        }
    }
    if (car_mode.now_mode == CAMERA) {
        //车体摇晃，优先稳定
        // if (abs(icm_data_gyro_y) > 0.0000003 || Wobble_time_start != 0 || flag_pose_1s != 0) {
        //     if (Track_Type != Wobble) {
        //         Wobble_time_start = time_1s;
        //         Track_Type = Wobble;
        //     }
        //     if (time_1s - Wobble_time_start > 0.5)//2s稳定
        //     {
        //         Wobble_time_start = 0;//退出稳定状态
        //     }
        // }
        //进出圆环
        if (ring.ring_state == LEAVE || ring.ring_state == ENTRY) {
            if (Track_Type != Ring_in_out) {
                Track_Type = Ring_in_out;
            }
        }

        else if (ring.ring_state == TURN) {
            if (Track_Type != Ring_turn) {
                Track_Type = Ring_turn;
            }

        }
        //左转
        else if ((LEFTLOSED && (image_Error <= 0) && (unlost_R > (image_Y - endline) * 4 / 9) && (unlost_L < (image_Y - endline) * 12 / 13) && unlost_R < 114) && image_Error <= -5)//&&rightline_Curvature<=-9)
        {
            //Buzzer(1);
            if (Track_Type != Left) {
                Track_Type = Left;
            }
        }
        //右转
        else if ((RIGHTLOSED && (image_Error >= 0) && (unlost_L > (image_Y - endline) * 4 / 9) && (unlost_R < (image_Y - endline) * 12 / 13) && unlost_L < 114) && image_Error >= 5)//&&rightline_Curvature>=9)
        {
            if (Track_Type != Right) {
                Track_Type = Right;
            }

        }
        //直线
        else {
            if (Track_Type != Straight)
                Track_Type = Straight;
        }
    }
    else {
        Track_Type = Straight;
    }
}
