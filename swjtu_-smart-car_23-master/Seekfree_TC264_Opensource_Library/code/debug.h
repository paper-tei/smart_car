#ifndef CODE_DEBUG_H_
#define CODE_DEBUG_H_
#include "zf_common_headfile.h"
extern unsigned char buzzer_flag;//串口下位机接收到信息蜂鸣器响开关
extern unsigned char Emergency_flag;
extern unsigned char protocol_flag;//上位机发送理想值开关（还有其他功能移步对应状态机），一般状态下位机一直更新目标值和实际值到上位机，更新上位机理想值时，需点一下启动，再点发送目标值，再点一下停止，表示上位机启动给下位机发送理想值，上位机停止给下位机发送理想值

#define IPCH 60 // 上位机显示图像高度
#define IPCW 80 // 上位机显示图像宽度
#define RED 1
#define BLUE 2
#define YELLOW 3
#define GREEN 4
typedef struct {
    float data[100];
    float cross_dericetion;
    unsigned int count;
    unsigned int flag;
}Debug;
extern uint8 Image_Use[IPCH][IPCW];

void SEND_IAMGE(void);
void SEND_DATA(void);
void SEND_DATA1(float datc);
void SEND_DATA2(float datc);
void drawing(float x[][2], float y[][2], float x1[][2], float y1[][2]);

// 串口接收数据缓冲区
extern uint8 fifo_get_data[128];                                                        // fifo 输出读出缓冲区
extern fifo_struct uart_data_fifo;
extern Debug debug;
extern float  car_scan_mode;//0摄像头 1纯电磁
extern float  break_mode;//0断路惯导 1断路电磁
extern float  out_home_dir;//0直走 1左转 2右转
extern float gate_offset;
void uart_rx_interrupt_handler(void);
void uart_lower_computer(void);
void buzzer_uart(void);
void debug_display(void);
void key_read_data(void);
void key_set_data(void);
//



#endif 
