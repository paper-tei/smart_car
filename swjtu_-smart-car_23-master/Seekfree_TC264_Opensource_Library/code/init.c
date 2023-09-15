/*
 * init.c
 *
 *  Created on: 2022年11月18日
 *      Author: paper
 */

#include "init.h"

void Buzzer(uint8 value) { gpio_set_level(BEEP_PIN, value); }
void Init(void) {
    // 初始化蜂鸣器引脚
    gpio_init(BEEP_PIN, GPO, 0, GPO_PUSH_PULL);
    //system_delay(30000, 10000);
    pwm_init(BDC_PWM_PIN, 30000, 0);           // 定义有刷电机PWM引脚
    pwm_init(BDC_DIR_PIN, 30000, 0);
    //system_delay_ms(800);
    // 初始化ADC电流检测
    //adc_init(A_F_PIN, ADC_12BIT);
    //adc_init(A_B_PIN, ADC_12BIT);
    //BLDC_Current_Init();




     // 初始化屏幕
    ips200_set_dir(IPS200_PORTAIT);
    ips200_init(IPS200_TYPE_PARALLEL8);
    Buzzer(1);
    // 按键初始化
    gpio_init(KEY1, GPI, 0, GPI_PULL_DOWN);
    gpio_init(KEY2, GPI, 0, GPI_PULL_DOWN);
    gpio_init(KEY3, GPI, 0, GPI_PULL_DOWN);
    gpio_init(KEY4, GPI, 0, GPI_PULL_DOWN);
    gpio_init(KEY5, GPI, 0, GPI_PULL_DOWN);
    gpio_init(KEY6, GPI, 0, GPI_PULL_DOWN);
    gpio_init(KEY7, GPI, 0, GPI_PULL_DOWN);
    gpio_init(KEY8, GPI, 0, GPI_PULL_DOWN);
    // 拨码开关初始化
   //     gpio_init(SW1, GPI, 0, GPI_PULL_UP);
   //     gpio_init(SW2, GPI, 0, GPI_PULL_UP);
        // 初始化PWM引脚
    pwm_init(BLDC_F_PWM_PIN, 12500, 10000);  // 定义前_无刷电机PWM引脚
    pwm_init(BLDC_B_PWM_PIN, 12500, 10000);  // 定义后_无刷电机PWM引脚

    pwm_init(SERVO_A_PWM_PIN, 50, 0);
    pwm_init(SERVO_B_PWM_PIN, 50, 0);
    // 初始化方向刹车引脚
    gpio_init(BLDC_B_STOP_PIN, GPO, 1, GPO_PUSH_PULL);
    gpio_init(BLDC_F_STOP_PIN, GPO, 1, GPO_PUSH_PULL);
    gpio_init(BLDC_B_DIR_PIN, GPO, 0, GPO_PUSH_PULL);
    gpio_init(BLDC_F_DIR_PIN, GPO, 0, GPO_PUSH_PULL);
    // 初始化编码器
    encoder_quad_init(TIM4_ENCODER, A_PIN_F, B_PIN_F);  // 前无刷电机

    encoder_quad_init(TIM6_ENCODER, A_PIN_B, B_PIN_B);  // 后无刷电机
    encoder_dir_init(TIM5_ENCODER, LSB_ENCODER, DIR_ENCODER);  // 有刷电机

    car_mode.now_mode = CAMERA;

    // 初始化图传
 //     gpio_init(P10_2, GPI, GPIO_HIGH, GPI_PULL_UP);              // 初始化CTS脚
    // uart_init(UART_2, 3000000, UART2_TX_P10_5, UART2_RX_P10_6); // 初始化串口，波特率3000000
    //uart_init(UART_3, 115200, UART3_TX_P15_6, UART3_RX_P15_7); // 初始化串口，波特率3000000
    //     uart_rx_interrupt(UART_2, 1);
    //uart_rx_interrupt(UART_3, 1);
    //     //wireless_uart_init();
       // 初始化imu
    imu660ra_init();
    Buzzer(0);
    // 修正零漂
    gyroOffset_init();
    // 初始化摄像头
    mt9v03x_init();
    //     // 逆透视变换
    //     //ImagePerspective_Init();
    //     //ImageChange_Init();
    dl1a_init();
    // 定时器初始化
    pit_ms_init(CCU60_CH0, 1);
    //初始化adc电磁输入
    adc_init(ADC_CHANNEL1, ADC_12BIT);                                          // 初始化对应 ADC 通道为对应精度
    adc_init(ADC_CHANNEL2, ADC_12BIT);                                          // 初始化对应 ADC 通道为对应精度
    adc_init(ADC_CHANNEL3, ADC_12BIT);                                          // 初始化对应 ADC 通道为对应精度
    adc_init(ADC_CHANNEL4, ADC_12BIT);                                           // 初始化对应 ADC 通道为对应精度
    //adc_init(ADC_CHANNEL7, ADC_12BIT);
    key_read_data();
    Em_black_Init();// 电磁2                                         // 初始化对应 ADC 通道为对应精度
    //     //wireless_uart_init();                                                // 判断是否通过初始化

#if PID_ASSISTANT_EN

    uint8 uart_get_data[128];
    fifo_init(&uart_data_fifo, FIFO_DATA_8BIT, uart_get_data, 128);              // 初始化 fifo 挂载缓冲区
    uart_init(UART_INDEX_PROTOCOL, UART_BAUDRATE_PROTOCOL, UART_TX_PIN_PROTOCOL, UART_RX_PIN_PROTOCOL);             // 初始化串口

    protocol_init();//下位机


    // int32_t target = 0;
    // /*初始化时，上发stop，同步上位机的启动按钮状态*/
    // set_computer_value(SEND_STOP_CMD, CURVES_CH1, NULL, 0);

    // /*获取默认的目标值*/
    // target = (int32_t)get_pid_target(&roll_speed_wheel);
    // /*给通道1发送目标值*/
    // set_computer_value(SEND_TARGET_CMD, CURVES_CH1, &target, 1);

    // /*获取默认的目标值*/
    // // target = GetTargetMaxSpeed();
    // target= (int32_t)get_pid_target(&roll_angle_wheel);
    // /*给通道2发送目标值*/
    // set_computer_value(SEND_TARGET_CMD, CURVES_CH2, &target, 1);
#endif

    //PID参数初始化
    PID_param_init();//因为这个初始化中含有一些串口通信和上下位机通信的东西，初始化放在这两个后面，不然会有BUG


}
