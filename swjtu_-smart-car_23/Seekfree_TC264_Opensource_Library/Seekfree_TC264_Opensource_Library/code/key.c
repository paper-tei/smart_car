/*
 * key.c
 *
 *  Created on: 2022年1月6日
 *      Author: paper
 */
#include "key.h"
 //拨码开关状态变量
uint8 sw1_status;
uint8 sw2_status;
//开关状态变量
uint8 key1_status = 1;
uint8 key2_status = 1;
uint8 key3_status = 1;
uint8 key4_status = 1;
//上一次开关状态变量
uint8 key1_last_status;
uint8 key2_last_status;
uint8 key3_last_status;
uint8 key4_last_status;
//开关标志位
uint8 key1_flag;
uint8 key2_flag;
uint8 key3_flag;
uint8 key4_flag;
uint8 go_flag = 0;

uint8 key_val = 0, key_x = 0, key_y = 0;

void Key_scan(void) {
    static uint8 key_stat = 0;
    key_val = 0, key_x = 0, key_y = 0;
    gpio_init(KEY1, GPI, 1, GPI_PULL_UP);
    gpio_init(KEY2, GPI, 1, GPI_PULL_UP);
    gpio_init(KEY3, GPI, 1, GPI_PULL_UP);
    gpio_init(KEY4, GPI, 1, GPI_PULL_UP);
    gpio_init(KEY5, GPO, 0, GPO_PUSH_PULL);
    gpio_init(KEY6, GPO, 0, GPO_PUSH_PULL);
    gpio_init(KEY7, GPO, 0, GPO_PUSH_PULL);
    gpio_init(KEY8, GPO, 0, GPO_PUSH_PULL);

    if (!gpio_get_level(KEY1))key_x = 3;
    else if (!gpio_get_level(KEY2))key_x = 2;
    else if (!gpio_get_level(KEY3))key_x = 1;
    else if (!gpio_get_level(KEY4))key_x = 0;

    gpio_init(KEY5, GPI, 1, GPI_PULL_UP);
    gpio_init(KEY6, GPI, 1, GPI_PULL_UP);
    gpio_init(KEY7, GPI, 1, GPI_PULL_UP);
    gpio_init(KEY8, GPI, 1, GPI_PULL_UP);
    gpio_init(KEY1, GPO, 0, GPO_PUSH_PULL);
    gpio_init(KEY2, GPO, 0, GPO_PUSH_PULL);
    gpio_init(KEY3, GPO, 0, GPO_PUSH_PULL);
    gpio_init(KEY4, GPO, 0, GPO_PUSH_PULL);

    if (!gpio_get_level(KEY5))key_y = 4;
    else if (!gpio_get_level(KEY6))key_y = 3;
    else if (!gpio_get_level(KEY7))key_y = 2;
    else if (!gpio_get_level(KEY8))key_y = 1;
    key_val = key_x + key_y * 4;
    switch (key_stat) {
    case 0:if (key_val != 0)key_stat = 1;
        break;
    case 1:if (key_val == 0)key_stat = 0;
          else {
        key_stat = 2;
        switch (key_val) {
        case 4:
            key_read_data();
            go_flag = 1;
            break;
        case 5:
            Inertial_Navigation_Start();
            break;
        case 6:
            Em_black_Scan();
            break;
        case 7:go_flag = 0;
            if (debug.flag == 1) {
                if (debug.count == 0) {
                    debug.data[debug.count]++;//debug.cross_dericetion
                }
                else if (debug.count == 9) {
                    debug.data[debug.count] += 10;//ring_speed
                }
                else if (debug.count == 10) {
                    debug.data[debug.count] += 10;//straight_speed
                }
                else if (debug.count == 11) {
                    debug.data[debug.count] += 10;//turn_speed
                }
                else if (debug.count == 12) {
                    debug.data[debug.count] += 1;//car_scan_mode
                }
                else if (debug.count == 13) {
                    debug.data[debug.count] += 1;//break_mode
                }
                else if (debug.count == 14) {
                    debug.data[debug.count] += 1;//out_home_flag
                }
                else if (debug.count == 15) {
                    debug.data[debug.count] += 1;//gate_offset
                }

            }
            break;
        case 8:
            if (debug.flag == 1) {
                if (debug.count == 0) {
                    debug.data[debug.count]--;
                }
                else if (debug.count == 9) {
                    debug.data[debug.count] -= 10;
                }
                else if (debug.count == 10) {
                    debug.data[debug.count] -= 10;
                }
                else if (debug.count == 11) {
                    debug.data[debug.count] -= 10;
                }
                else if (debug.count == 12) {
                    debug.data[debug.count] -= 1;
                }
                else if (debug.count == 13) {
                    debug.data[debug.count] -= 1;
                }
                else if (debug.count == 14) {
                    debug.data[debug.count] -= 1;
                }
                else if (debug.count == 15) {
                    debug.data[debug.count] -= 1;
                }

            }
            break;
        case 9:servo_pwm++;
            break;
        case 10:servo_pwm--;
            break;
        case 11:go_home_flag = 1;
            car_mode.now_mode = INERTIAL_NAVIGATION;
            break;
        case 12:debug.flag = 4;
            break;
        case 13:debug.flag = 5;
            break;
        case 14:debug.flag = 6;
            break;
        case 15:
            if (debug.flag == 1) {//写数据
                key_set_data();
            }
            break;
        case 16:
            if (debug.flag == 1) {//读数据
                key_read_data();
            }
            break;

        case 17:
            debug.count++; //改变下一个参数设置
            if (debug.count == 1)debug.count = 9;
            break;

        case 18:
            debug.flag = 0;//退出调参模式
            ips200_clear();
            break;
            //进入调参模式
        case 19:
            debug.flag = 1;
            ips200_clear();
            break;

        }
    }
          break;
    case 2:if (key_val == 0)key_stat = 0;
        break;
    }
}
//     //清零标志位
//     key1_flag = 0;
//     key2_flag = 0;
//     key3_flag = 0;
//     key4_flag = 0;
//     sw1_status = 0;
//     sw2_status = 0;
//     //读取历史按键状态
//     key1_last_status = key1_status;
//     key2_last_status = key2_status;
//     key3_last_status = key3_status;
//     key4_last_status = key4_status;
//     //读取当前按键状态
//     key1_status = gpio_get_level(KEY1);
//     key2_status = gpio_get_level(KEY2);
//     key3_status = gpio_get_level(KEY3);
//     key4_status = gpio_get_level(KEY4);
//     //获取按键开关状态
//     if (key1_status && !key1_last_status) {
//         key1_flag = 1;
//     }
//     if (key2_status && !key2_last_status) {
//         key2_flag = 1;
//     }
//     if (key3_status && !key3_last_status) {
//         key3_flag = 1;
//     }
//     if (key4_status && !key4_last_status) {
//         key4_flag = 1;
//     }
//     //获取拨码开关状态
//     sw1_status = gpio_get_level(SW1);
//     sw2_status = gpio_get_level(SW2);
//     if (key4_flag) {
//         // bldc_b_speed = 0;
//         // bldc_f_speed = 0;
//          go_flag = 0;


//     }
//     if (key3_flag) {
//         // STOP_BLDC(1);
//     }
//     if (key2_flag) {
//         go_flag = 1;
//         // bldc_b_speed += 10;
//         // bldc_f_speed -= 10;
//         // bdc_speed += 100;
// //        pwm_set_duty(SERVO_B_PWM_PIN, 200);
// //        pwm_set_duty(SERVO_A_PWM_PIN, 200);

//     }
//     if (key1_flag) {
//         yaw_pirouette(360.0f);
//         // bldc_b_speed -= 10;
//         // bldc_f_speed += 10;
//         // bdc_speed -= 100;
// //        pwm_set_duty(SERVO_B_PWM_PIN, 1000);
// //        pwm_set_duty(SERVO_A_PWM_PIN, 1000);
//     }
// }
