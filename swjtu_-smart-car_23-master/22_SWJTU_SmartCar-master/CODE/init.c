/*
 * init.c
 *
 *  Created on: 2022年1月6日
 *      Author: paper
 */
//初始化代码，只运行一次
#include "init.h"
void Init(void)
{
 
        get_clk();
        //初始化蜂鸣器引脚
        gpio_init(BEEP_PIN, GPO, 0, PUSHPULL);
        Buzzer(1);
        //按键初始化
        gpio_init(KEY1,GPI,0,PULLUP);
        gpio_init(KEY2,GPI,0,PULLUP);
        gpio_init(KEY3,GPI,0,PULLUP);
        gpio_init(KEY4,GPI,0,PULLUP);
        //拨码开关初始化
        gpio_init(SW1,GPI,0,PULLUP);
        gpio_init(SW2,GPI,0,PULLUP);
        //初始化舵机
        gtm_pwm_init(S_MOTOR_PIN, 50, duty);
        //初始化PWM引脚
        gtm_pwm_init(MOTOR1_PWM, 17000, 0);
        gtm_pwm_init(MOTOR2_PWM, 17000, 0);
        gtm_pwm_init(MOTOR3_PWM, 17000, 0);
        gtm_pwm_init(MOTOR4_PWM, 17000, 0);
        //初始化霍尔
        gpio_init(P10_5, GPI, 0, PULLDOWN);  //霍尔引脚初始化
        
        //初始化编码器
        gpt12_init(GPT12_T5, GPT12_T5INB_P10_3, GPT12_T5EUDB_P10_1);
        gpt12_init(GPT12_T6, GPT12_T6INA_P20_3, GPT12_T6EUDA_P20_0);
        //初始化速度环pid
        Init_pid_wheel();
        //初始化图传
        uart_init(UART_0, 2000000, UART0_TX_P14_0,UART0_RX_P14_1);
        //初始化无线串口
        //seekfree_wireless_init(); 

        //初始化tof
        Distance_measurement_Init();
        //初始化ICM
        icm_int();
        //初始化摄像头
        
        mt9v03x_init();
        Buzzer(0);
        //初始化屏幕
        ips200_init();
        //定时器初始化
        pit_interrupt_ms(CCU6_0, PIT_CH0, 2);
        //等待所有核心初始化完毕
        IfxCpu_emitEvent(&g_cpuSyncEvent);
        IfxCpu_waitEvent(&g_cpuSyncEvent, 0xFFFF);
        //开启总中断
        enableInterrupts();
        //计算真实赛道宽度
        track_width_real();
}



