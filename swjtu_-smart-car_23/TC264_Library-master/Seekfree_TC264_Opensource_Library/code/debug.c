/*
 * debug.c
 *
 *  Created on: 2023年1月4日
 *      Author: paper
 */
#include "debug.h"
Debug debug;
#define FLASH_SECTION_INDEX       (0)                                 // 存储数据用的扇区
#define FLASH_PAGE_INDEX          (11)                                // 存储数据用的页码 倒数第一个页码



unsigned char Emergency_flag = 0;
//串口
uint8  get_data = 0;                                                          // 鎺ユ敹鏁版嵁鍙橀噺
uint8 fifo_data_count = 0;                                                     // fifo 鏁版嵁涓暟
//fifo_struct uart_data_fifo;
uint8_t Recv1[128] = { 0 };//本地接收数组
uint8_t rx_cnt = 0;//鎺ユ敹鏁版嵁涓暟璁℃暟鍙橀噺
uint8_t bufcopy[128];//鏈�澶氬彧鍙栧墠64涓暟鎹�
int sizecopy = 128;
// 串口接收数据缓冲区
uint8 fifo_get_data[128];                                                        // fifo 输出读出缓冲区
fifo_struct uart_data_fifo;

unsigned char buzzer_flag = 0;//串口下位机接收到信息蜂鸣器响开关

unsigned char protocol_flag = 0;//上位机发送理想值开关（还有其他功能移步对应状态机），一般状态下位机一直更新目标值和实际值到上位机，更新上位机理想值时，需点一下启动，再点发送目标值，再点一下停止，表示上位机启动给下位机发送理想值，上位机停止给下位机发送理想值
float car_scan_mode = 0;//0摄像头 1纯电磁
float break_mode = 0;//0断路惯导 1断路电磁
float out_home_dir = 0;//0直走 1左转 2右转
float gate_offset = 0;
uint8 data_buffer[128];
uint8 data_len;
void debug_display(void) {    //参数传递（放中断）
    // 0 十字转向方向
    // debug.cross_dericetion = debug.data[0];
    // AD_val_1_max=debug.data[1];
    // AD_val_2_max=debug.data[2];
    // AD_val_3_max=debug.data[3];
    // AD_val_4_max=debug.data[4];
    //AD_val_1_min=debug.data[5];
    //AD_val_2_min=debug.data[6];
    //AD_val_3_min=debug.data[7];
    //AD_val_4_min=debug.data[8];
    //ring_speed = debug.data[9];
    //straight_speed = debug.data[10];
    //turn_speed = debug.data[11];
    //car_scan_mode = debug.data[12];
    //break_mode = debug.data[13];
    //out_home_dir = debug.data[14];
    //gate_offset = debug.data[15];
    ips200_show_string(0, 0, "cross_dericetion");  ips200_show_float(130, 0, debug.cross_dericetion, 4, 6);
    ips200_show_string(0, 20, "ring_speed");       ips200_show_float(130, 20, ring_speed, 4, 6);
    ips200_show_string(0, 40, "straight_speed");   ips200_show_float(130, 40, straight_speed, 4, 6);
    ips200_show_string(0, 60, "turn_speed");       ips200_show_float(130, 60, turn_speed, 4, 6);
    ips200_show_string(0, 80, "car_scan_mode");    ips200_show_float(130, 80, car_scan_mode, 4, 6);
    ips200_show_string(0, 100, "break_mode");      ips200_show_float(130, 100, break_mode, 4, 6);
    ips200_show_string(0, 120, "out_home_dir");    ips200_show_float(130, 120, out_home_dir, 4, 6);
    ips200_show_string(0, 140, "gate_offset");    ips200_show_float(130, 140, gate_offset, 4, 6);
}

void key_read_data(void) {
    flash_read_page_to_buffer(FLASH_SECTION_INDEX, FLASH_PAGE_INDEX);           // 将数据从 flash 读取到缓冲区
    //data值是和按键关联改变的数组
    debug.data[0] = flash_union_buffer[0].float_type;
    debug.data[1] = flash_union_buffer[1].float_type;
    debug.data[2] = flash_union_buffer[2].float_type;
    debug.data[3] = flash_union_buffer[3].float_type;
    debug.data[4] = flash_union_buffer[4].float_type;
    debug.data[5] = flash_union_buffer[5].float_type;
    debug.data[6] = flash_union_buffer[6].float_type;
    debug.data[7] = flash_union_buffer[7].float_type;
    debug.data[8] = flash_union_buffer[8].float_type;
    debug.data[9] = flash_union_buffer[9].float_type;
    debug.data[10] = flash_union_buffer[10].float_type;
    debug.data[11] = flash_union_buffer[11].float_type;
    debug.data[12] = flash_union_buffer[12].float_type;
    debug.data[13] = flash_union_buffer[13].float_type;
    debug.data[14] = flash_union_buffer[14].float_type;
    debug.data[15] = flash_union_buffer[15].float_type;


}
void key_set_data(void) {
    if (flash_check(FLASH_SECTION_INDEX, FLASH_PAGE_INDEX))                      // 判断是否有数据
        flash_erase_page(FLASH_SECTION_INDEX, FLASH_PAGE_INDEX);                // 擦除这一页
    //
    flash_union_buffer[0].float_type = debug.data[0];
    flash_union_buffer[1].float_type = debug.data[1];
    flash_union_buffer[2].float_type = debug.data[2];
    flash_union_buffer[3].float_type = debug.data[3];
    flash_union_buffer[4].float_type = debug.data[4];
    flash_union_buffer[5].float_type = debug.data[5];
    flash_union_buffer[6].float_type = debug.data[6];
    flash_union_buffer[7].float_type = debug.data[7];
    flash_union_buffer[8].float_type = debug.data[8];
    flash_union_buffer[9].float_type = debug.data[9];
    flash_union_buffer[10].float_type = debug.data[10];
    flash_union_buffer[11].float_type = debug.data[11];
    flash_union_buffer[12].float_type = debug.data[12];
    flash_union_buffer[13].float_type = debug.data[13];
    flash_union_buffer[14].float_type = debug.data[14];
    flash_union_buffer[15].float_type = debug.data[15];
    flash_write_page_from_buffer(FLASH_SECTION_INDEX, FLASH_PAGE_INDEX);
}

#define FLASH_SECTION_INDEX       (0)                                 // 存储数据用的扇区
#define FLASH_PAGE_INDEX          (11)                                // 存储数据用的页码 倒数第一个页码


//-------------------------------------------------------------------------------------------------------------------
// 函数简介       UART_INDEX 的接收中断处理函数 这个函数将在 UART_INDEX 对应的中断调用
// 参数说明       void
// 返回参数       void
// 使用示例       uart_rx_interrupt_handler();
//-------------------------------------------------------------------------------------------------------------------
void uart_rx_interrupt_handler(void) {
    //    get_data = uart_read_byte(UART_INDEX);                                      // 接收数据 while 等待式 不建议在中断使用
    //用无线注释掉串口
    //    uart_query_byte(UART_INDEX_PROTOCOL, &get_data);                                     // 接收数据 查询式 有数据会返回 TRUE 没有数据会返回 FALSE
    //    fifo_write_buffer(&uart_data_fifo, &get_data, 1);                           // 将数据写入 fifo 中
}



//-------------------------------------------------------------------------------------------------------------------
// 函数简介       野火上位机 的接收中断处理函数 这个函数将在 UART_INDEX_PROTOCOL 对应的中断调用
// 参数说明       void
// 返回参数       void
// 使用示例
//-------------------------------------------------------------------------------------------------------------------

void uart_lower_computer(void) {
    fifo_data_count = (uint8)wireless_uart_read_buff(data_buffer, 128);             // 查看是否有消息 默认缓冲区是 WIRELESS_UART_BUFFER_SIZE 总共 64 字节
    if (fifo_data_count != 0)                                                       // 收到了消息 读取函数会返回实际读取到的数据个数
    {
        rx_cnt = fifo_data_count;         //接收数据长度


        //清空本地接收数组
        /*void *memset(void *str, int c, size_t n)
        解释：复制字符 c（一个无符号字符）到参数 str 所指向的字符串的前 n 个字符。
        作用：是在一段内存块中填充某个给定的值，它是对较大的结构体或数组进行清零操作的一种最快方法
        头文件：C中#include<string.h>，C++中#include<cstring>*/
        memset(bufcopy, 0, sizecopy);

        memcpy(bufcopy, data_buffer, rx_cnt);//有几个复制几个
        /*void *memcpy(void *destin, void *source, unsigned n);
        作用是：以source指向的地址为起点，将连续的n个字节数据，复制到以destin指向的地址为起点的内存中。
        函数有三个参数，第一个是目标地址，第二个是源地址，第三个是数据长度。

        使用memcpy函数时，需要注意：

        数据长度（第三个参数）的单位是字节（1byte = 8bit）。
        注意该函数有一个返回值，类型是void*，是一个指向destin的指针*/
        protocol_data_recv(bufcopy, rx_cnt);

        memset(data_buffer, 0, sizecopy);//清空接收数组
        rx_cnt = 0;

    }
    // 蓝牙测试
        // data_len = (uint8)wireless_uart_read_buff(data_buffer, 32);             // 查看是否有消息 默认缓冲区是 WIRELESS_UART_BUFFER_SIZE 总共 64 字节
        // if(data_len != 0)                                                       // 收到了消息 读取函数会返回实际读取到的数据个数
        // {
        //     wireless_uart_send_buff(data_buffer, data_len);                     // 将收到的消息发送回去
        //     memset(data_buffer, 0, 32);
        //     func_uint_to_str((char *)data_buffer, data_len);
        //     wireless_uart_send_string("\r\ndata len:");                                 // 显示实际收到的数据信息
        //     wireless_uart_send_buff(data_buffer, strlen((const char *)data_buffer));    // 显示收到的数据个数
        //     wireless_uart_send_string(".\r\n");
        // }
    //串口测试
            // fifo_data_count = fifo_used(&uart_data_fifo);                           // 查看 fifo 是否有数据
            // if(fifo_data_count != 0)                                                // 读取到数据了
            // {
            //     fifo_read_buffer(&uart_data_fifo, fifo_get_data, &fifo_data_count, FIFO_READ_AND_CLEAN);    // 将 fifo 中数据读出并清空 fifo 挂载的缓冲
            //     uart_write_string(UART_INDEX_PROTOCOL, "\r\nUART get data:");                // 输出测试信息
            //     uart_write_buffer(UART_INDEX_PROTOCOL, fifo_get_data, fifo_data_count);      // 将读取到的数据发送出去
            // }



    //无蓝牙串口部分
    //        fifo_data_count = fifo_used(&uart_data_fifo);                           // 查看 fifo 是否有数据
    //        rx_cnt = fifo_data_count;         //接收数据长度
    //        if (fifo_data_count != 0)                                                // 读取到数据了
    //        {
    //            fifo_read_buffer(&uart_data_fifo, Recv1, &fifo_data_count, FIFO_READ_AND_CLEAN);    // 将 fifo 中数据读出并清空 fifo 挂载的缓冲
    //            // uart_write_string(UART_INDEX, "\r\nUART get data:");                // 输出测试信息
    //            // uart_write_buffer(UART_INDEX, fifo_get_data, fifo_data_count);      // 将读取到的数据发送出去
    //            //清空本地接收数组
    //
    //            /*void *memset(void *str, int c, size_t n)
    //            解释：复制字符 c（一个无符号字符）到参数 str 所指向的字符串的前 n 个字符。
    //            作用：是在一段内存块中填充某个给定的值，它是对较大的结构体或数组进行清零操作的一种最快方法
    //            头文件：C中#include<string.h>，C++中#include<cstring>*/
    //            memset(bufcopy, 0, sizecopy);
    //
    //            memcpy(bufcopy, Recv1, rx_cnt);//有几个复制几个
    //            /*void *memcpy(void *destin, void *source, unsigned n);
    //            作用是：以source指向的地址为起点，将连续的n个字节数据，复制到以destin指向的地址为起点的内存中。
    //            函数有三个参数，第一个是目标地址，第二个是源地址，第三个是数据长度。
    //
    //            使用memcpy函数时，需要注意：
    //
    //            数据长度（第三个参数）的单位是字节（1byte = 8bit）。
    //            注意该函数有一个返回值，类型是void*，是一个指向destin的指针*/
    //            protocol_data_recv(bufcopy, rx_cnt);
    //
    //            memset(Recv1, 0, sizecopy);
    //            rx_cnt = 0;
    //        }
}


void buzzer_uart(void) {
    // if (buzzer_flag == 1) {
    Buzzer(1);
    system_delay_ms(20);
    Buzzer(1);
    system_delay_ms(20);
    Buzzer(1);
    system_delay_ms(20);
    Buzzer(0);
    //     buzzer_flag = 0;
    // }
}

void drawing(float x[][2], float y[][2], float x1[][2], float y1[][2])
{
    int i;
    for (i = 0; i < Tracing_Time_L; i++)
    {
        if (x[i][0] > 58)
        {
            x[i][0] = 58;
        }
        if (x[i][1] > 78)
        {
            x[i][1] = 78;
        }
        if (x1[i][0] > 58)
        {
            x1[i][0] = 58;
        }
        if (x1[i][1] > 78)
        {
            x1[i][1] = 78;
        }
        ips200_draw_point(x[i][1] * 4 - 1, x[i][0] * 4 - 1, RGB565_BLUE);
        ips200_draw_point(x[i][1] * 4 - 1, x[i][0] * 4, RGB565_BLUE);
        ips200_draw_point(x[i][1] * 4 - 1, x[i][0] * 4 + 1, RGB565_BLUE);
        ips200_draw_point(x[i][1] * 4, x[i][0] * 4 - 1, RGB565_BLUE);
        ips200_draw_point(x[i][1] * 4, x[i][0] * 4, RGB565_BLUE);
        ips200_draw_point(x[i][1] * 4, x[i][0] * 4 + 1, RGB565_BLUE);
        ips200_draw_point(x[i][1] * 4 + 1, x[i][0] * 4 - 1, RGB565_BLUE);
        ips200_draw_point(x[i][1] * 4 + 1, x[i][0] * 4, RGB565_BLUE);
        ips200_draw_point(x[i][1] * 4 + 1, x[i][0] * 4 + 1, RGB565_BLUE);
        if (left_line_angle_out[i] > 70 || left_line_angle_out[i] < -70)
        {
            ips200_draw_point(x[i][1] * 4 - 1, x[i][0] * 4 - 1, RGB565_RED);
            ips200_draw_point(x[i][1] * 4 - 1, x[i][0] * 4, RGB565_RED);
            ips200_draw_point(x[i][1] * 4 - 1, x[i][0] * 4 + 1, RGB565_RED);
            ips200_draw_point(x[i][1] * 4, x[i][0] * 4 - 1, RGB565_RED);
            ips200_draw_point(x[i][1] * 4, x[i][0] * 4, RGB565_RED);
            ips200_draw_point(x[i][1] * 4, x[i][0] * 4 + 1, RGB565_RED);
            ips200_draw_point(x[i][1] * 4 + 1, x[i][0] * 4 - 1, RGB565_RED);
            ips200_draw_point(x[i][1] * 4 + 1, x[i][0] * 4, RGB565_RED);
            ips200_draw_point(x[i][1] * 4 + 1, x[i][0] * 4 + 1, RGB565_RED);
        }
        ips200_draw_point(x1[i][1] * 4 - 1, x1[i][0] * 4 - 1, RGB565_GREEN);
        ips200_draw_point(x1[i][1] * 4 - 1, x1[i][0] * 4, RGB565_GREEN);
        ips200_draw_point(x1[i][1] * 4 - 1, x1[i][0] * 4 + 1, RGB565_GREEN);
        ips200_draw_point(x1[i][1] * 4, x1[i][0] * 4 - 1, RGB565_GREEN);
        ips200_draw_point(x1[i][1] * 4, x1[i][0] * 4, RGB565_GREEN);
        ips200_draw_point(x1[i][1] * 4, x1[i][0] * 4 + 1, RGB565_GREEN);
        ips200_draw_point(x1[i][1] * 4 + 1, x1[i][0] * 4 - 1, RGB565_GREEN);
        ips200_draw_point(x1[i][1] * 4 + 1, x1[i][0] * 4, RGB565_GREEN);
        ips200_draw_point(x1[i][1] * 4 + 1, x1[i][0] * 4 + 1, RGB565_GREEN);
    }
    for (i = 0; i < Tracing_Time_R; i++)
    {
        if (y[i][0] > 58)
        {
            y[i][0] = 58;
        }
        if (y[i][1] > 78)
        {
            y[i][1] = 78;
        }
        if (y1[i][0] >= 58)
        {
            y1[i][0] = 58;
        }
        if (y1[i][1] >= 78)
        {
            y1[i][1] = 78;
        }
        ips200_draw_point(y[i][1] * 4 - 1, y[i][0] * 4 - 1, RGB565_BLUE);
        ips200_draw_point(y[i][1] * 4 - 1, y[i][0] * 4, RGB565_BLUE);
        ips200_draw_point(y[i][1] * 4 - 1, y[i][0] * 4 + 1, RGB565_BLUE);
        ips200_draw_point(y[i][1] * 4, y[i][0] * 4 - 1, RGB565_BLUE);
        ips200_draw_point(y[i][1] * 4, y[i][0] * 4, RGB565_BLUE);
        ips200_draw_point(y[i][1] * 4, y[i][0] * 4 + 1, RGB565_BLUE);
        ips200_draw_point(y[i][1] * 4 + 1, y[i][0] * 4 - 1, RGB565_BLUE);
        ips200_draw_point(y[i][1] * 4 + 1, y[i][0] * 4, RGB565_BLUE);
        ips200_draw_point(y[i][1] * 4 + 1, y[i][0] * 4 + 1, RGB565_BLUE);
        if (right_line_angle_out[i] > 70 || right_line_angle_out[i] < -70)
        {
            ips200_draw_point(y[i][1] * 4 - 1, y[i][0] * 4 - 1, RGB565_RED);
            ips200_draw_point(y[i][1] * 4 - 1, y[i][0] * 4, RGB565_RED);
            ips200_draw_point(y[i][1] * 4 - 1, y[i][0] * 4 + 1, RGB565_RED);
            ips200_draw_point(y[i][1] * 4, y[i][0] * 4 - 1, RGB565_RED);
            ips200_draw_point(y[i][1] * 4, y[i][0] * 4, RGB565_RED);
            ips200_draw_point(y[i][1] * 4, y[i][0] * 4 + 1, RGB565_RED);
            ips200_draw_point(y[i][1] * 4 + 1, y[i][0] * 4 - 1, RGB565_RED);
            ips200_draw_point(y[i][1] * 4 + 1, y[i][0] * 4, RGB565_RED);
            ips200_draw_point(y[i][1] * 4 + 1, y[i][0] * 4 + 1, RGB565_RED);
        }
        ips200_draw_point(y1[i][1] * 4 - 1, y1[i][0] * 4 - 1, RGB565_GREEN);
        ips200_draw_point(y1[i][1] * 4 - 1, y1[i][0] * 4, RGB565_GREEN);
        ips200_draw_point(y1[i][1] * 4 - 1, y1[i][0] * 4 + 1, RGB565_GREEN);
        ips200_draw_point(y1[i][1] * 4, y1[i][0] * 4 - 1, RGB565_GREEN);
        ips200_draw_point(y1[i][1] * 4, y1[i][0] * 4, RGB565_GREEN);
        ips200_draw_point(y1[i][1] * 4, y1[i][0] * 4 + 1, RGB565_GREEN);
        ips200_draw_point(y1[i][1] * 4 + 1, y1[i][0] * 4 - 1, RGB565_GREEN);
        ips200_draw_point(y1[i][1] * 4 + 1, y1[i][0] * 4, RGB565_GREEN);
        ips200_draw_point(y1[i][1] * 4 + 1, y1[i][0] * 4 + 1, RGB565_GREEN);
    }
}