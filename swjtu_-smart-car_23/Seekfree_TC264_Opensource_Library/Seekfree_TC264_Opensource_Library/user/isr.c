/********************************************************************************************************************
* TC264 Opensourec Library 即（TC264 开源库）是一个基于官方 SDK 接口的第三方开源库
* Copyright (c) 2022 SEEKFREE 逐飞科技
*
* 本文件是 TC264 开源库的一部分
*
* TC264 开源库 是免费软件
* 您可以根据自由软件基金会发布的 GPL（GNU General Public License，即 GNU通用公共许可证）的条款
* 即 GPL 的第3版（即 GPL3.0）或（您选择的）任何后来的版本，重新发布和/或修改它
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
* 许可申明英文版在 libraries/doc 文件夹下的 GPL3_permission_statement.txt 文件中
* 许可证副本在 libraries 文件夹下 即该文件夹下的 LICENSE 文件
* 欢迎各位使用并传播本程序 但修改内容时必须保留逐飞科技的版权声明（即本声明）
*
* 文件名称          isr
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

#include "isr_config.h"
#include "isr.h"
// **************************** PIT中断函数 ****************************
IFX_INTERRUPT(cc60_pit_ch0_isr, 0, CCU6_0_CH0_ISR_PRIORITY) {
    interrupt_global_enable(0);                     // 开启中断嵌套
    pit_clear_flag(CCU60_CH0);
    ///////////////////
    static uint8 t_2ms = 0;
    static uint8 t_6ms = 0;
    static uint8 t_10ms = 0;
    static uint8 t_20ms = 0;
    static uint8 t_100ms = 0;
    static uint16 t_1s = 0;
    t_2ms++;
    t_6ms++;
    t_10ms++;
    t_20ms++;
    t_100ms++;
    t_1s++;
    //2ms控制周期 解算陀螺仪姿态数据
    if (t_2ms == 2) {
        t_2ms = 0;
        Flag.T_2ms = 1;
    }
    //6ms控制周期
    if (t_6ms == 6) {
        t_6ms = 0;
        Flag.T_6ms = 1;
    }
    //10ms控制周期
    if (t_10ms == 10) {
        t_10ms = 0;
        Flag.T_10ms = 1;
    }
    //20ms控制周期
    if (t_20ms == 20) {
        t_20ms = 0;
        Flag.T_20ms = 1;
    }
    //100ms控制周期
    if (t_100ms == 100) {
        t_100ms = 0;
        Flag.T_100ms = 1;
    }
    //1s控制周期
    if (t_1s == 1000) {
        t_1s = 0;
        Flag.T_1s = 1;
    }
    if (Emergency_flag == 0) {
        Fuse_result();
    }

}


IFX_INTERRUPT(cc60_pit_ch1_isr, 0, CCU6_0_CH1_ISR_PRIORITY) {
    interrupt_global_enable(0);                     // 开启中断嵌套
    pit_clear_flag(CCU60_CH1);




}

IFX_INTERRUPT(cc61_pit_ch0_isr, 0, CCU6_1_CH0_ISR_PRIORITY) {
    interrupt_global_enable(0);                     // 开启中断嵌套
    pit_clear_flag(CCU61_CH0);




}

IFX_INTERRUPT(cc61_pit_ch1_isr, 0, CCU6_1_CH1_ISR_PRIORITY) {
    interrupt_global_enable(0);                     // 开启中断嵌套
    pit_clear_flag(CCU61_CH1);





}
// **************************** PIT中断函数 ****************************


// **************************** 外部中断函数 ****************************
IFX_INTERRUPT(exti_ch0_ch4_isr, 0, EXTI_CH0_CH4_INT_PRIO) {
    interrupt_global_enable(0);                     // 开启中断嵌套
    if (exti_flag_get(ERU_CH0_REQ0_P15_4))           // 通道0中断
    {
        exti_flag_clear(ERU_CH0_REQ0_P15_4);
        wireless_module_uart_handler();                 // 无线模块统一回调函数
    }

    if (exti_flag_get(ERU_CH4_REQ13_P15_5))          // 通道4中断
    {
        exti_flag_clear(ERU_CH4_REQ13_P15_5);
    }
}

IFX_INTERRUPT(exti_ch1_ch5_isr, 0, EXTI_CH1_CH5_INT_PRIO) {
    interrupt_global_enable(0);                     // 开启中断嵌套
    if (exti_flag_get(ERU_CH1_REQ10_P14_3))          // 通道1中断
    {
        exti_flag_clear(ERU_CH1_REQ10_P14_3);



    }

    if (exti_flag_get(ERU_CH5_REQ1_P15_8))           // 通道5中断
    {
        exti_flag_clear(ERU_CH5_REQ1_P15_8);




    }
}

// 由于摄像头pclk引脚默认占用了 2通道，用于触发DMA，因此这里不再定义中断函数
// IFX_INTERRUPT(exti_ch2_ch6_isr, 0, EXTI_CH2_CH6_INT_PRIO)
// {
//  interrupt_global_enable(0);                     // 开启中断嵌套
//  if(exti_flag_get(ERU_CH2_REQ7_P00_4))           // 通道2中断
//  {
//      exti_flag_clear(ERU_CH2_REQ7_P00_4);
//  }
//  if(exti_flag_get(ERU_CH6_REQ9_P20_0))           // 通道6中断
//  {
//      exti_flag_clear(ERU_CH6_REQ9_P20_0);
//  }
// }

IFX_INTERRUPT(exti_ch3_ch7_isr, 0, EXTI_CH3_CH7_INT_PRIO) {
    interrupt_global_enable(0);                     // 开启中断嵌套
    if (exti_flag_get(ERU_CH3_REQ6_P02_0))           // 通道3中断
    {
        exti_flag_clear(ERU_CH3_REQ6_P02_0);
        camera_vsync_handler();                     // 摄像头触发采集统一回调函数
    }
    if (exti_flag_get(ERU_CH7_REQ16_P15_1))          // 通道7中断
    {
        exti_flag_clear(ERU_CH7_REQ16_P15_1);




    }
}
// **************************** 外部中断函数 ****************************


// **************************** DMA中断函数 ****************************
IFX_INTERRUPT(dma_ch5_isr, 0, DMA_INT_PRIO) {
    interrupt_global_enable(0);                     // 开启中断嵌套
    camera_dma_handler();                           // 摄像头采集完成统一回调函数
}
// **************************** DMA中断函数 ****************************


// **************************** 串口中断函数 ****************************
IFX_INTERRUPT(uart0_tx_isr, 0, UART0_TX_INT_PRIO) {
    interrupt_global_enable(0);                     // 开启中断嵌套
    IfxAsclin_Asc_isrTransmit(&uart0_handle);


}

IFX_INTERRUPT(uart0_rx_isr, 0, UART0_RX_INT_PRIO) {
    interrupt_global_enable(0);                     // 开启中断嵌套
    IfxAsclin_Asc_isrReceive(&uart0_handle);

#if DEBUG_UART_USE_INTERRUPT                        // 如果开启 debug 串口中断
    debug_interrupr_handler();                  // 调用 debug 串口接收处理函数 数据会被 debug 环形缓冲区读取
#endif
    // 如果修改了 DEBUG_UART_INDEX 那这段代码需要放到对应的串口中断去
}


IFX_INTERRUPT(uart0_er_isr, 0, UART0_ER_INT_PRIO) {
    interrupt_global_enable(0);                     // 开启中断嵌套
    IfxAsclin_Asc_isrError(&uart0_handle);

}

// 串口1默认连接到摄像头配置串口
IFX_INTERRUPT(uart1_tx_isr, 0, UART1_TX_INT_PRIO) {
    interrupt_global_enable(0);                     // 开启中断嵌套
    IfxAsclin_Asc_isrTransmit(&uart1_handle);




}
IFX_INTERRUPT(uart1_rx_isr, 0, UART1_RX_INT_PRIO) {
    interrupt_global_enable(0);                     // 开启中断嵌套
    IfxAsclin_Asc_isrReceive(&uart1_handle);
    camera_uart_handler();                          // 摄像头参数配置统一回调函数

}
IFX_INTERRUPT(uart1_er_isr, 0, UART1_ER_INT_PRIO) {
    interrupt_global_enable(0);                     // 开启中断嵌套
    IfxAsclin_Asc_isrError(&uart1_handle);




}
uint8 u2dat, u3dat;
uint8 stop = 0;
// 串口2默认连接到无线转串口模块
IFX_INTERRUPT(uart2_tx_isr, 0, UART2_TX_INT_PRIO) {
    interrupt_global_enable(0);                     // 开启中断嵌套
    IfxAsclin_Asc_isrTransmit(&uart2_handle);

}
IFX_INTERRUPT(uart2_rx_isr, 0, UART2_RX_INT_PRIO) {
    interrupt_global_enable(0);                     // 开启中断嵌套
    IfxAsclin_Asc_isrReceive(&uart2_handle);
    // wireless_module_uart_handler();                 // 无线模块统一回调函数
    // uart_lower_computer();
     //    uart_rx_interrupt_handler();                    // 串口接收处理，pid下位机
    uart_query_byte(UART_2, &u2dat);
    //uart_write_buffer(UART_3, &u2dat, 1);
}
IFX_INTERRUPT(uart2_er_isr, 0, UART2_ER_INT_PRIO) {
    interrupt_global_enable(0);                     // 开启中断嵌套
    IfxAsclin_Asc_isrError(&uart2_handle);

}


IFX_INTERRUPT(uart3_tx_isr, 0, UART3_TX_INT_PRIO) {
    interrupt_global_enable(0);                     // 开启中断嵌套
    IfxAsclin_Asc_isrTransmit(&uart3_handle);



}
#define BUF_SIZE 30
uint8 uart3_buff[BUF_SIZE];
uint8 buff_size = 0;
float angle_pitch = 0, angle_roll, angle_yaw, angle_yaw_total, angle_yaw_old;
IFX_INTERRUPT(uart3_rx_isr, 0, UART3_RX_INT_PRIO) {
    interrupt_global_enable(0);                     // 开启中断嵌套
    IfxAsclin_Asc_isrReceive(&uart3_handle);
    uart_query_byte(UART_3, &u3dat);
    uart_write_buffer(UART_2, &u2dat, 1);

    // u3dat = uart_read_byte(UART_3);
    // uart3_buff[buff_size++] = u3dat;
    // if (buff_size >= 2) {
    //     if (uart3_buff[0] == 0xAA && uart3_buff[1] == 0xAA) //接收到帧头
    //     {
    //         //接收到帧尾
    //         if (buff_size >= 19) {
    //             unsigned char sum = 0;
    //             for (uint8 i = 2;i < 18;i++) {
    //                 sum += uart3_buff[i];
    //             }
    //             if (uart3_buff[18] == sum) {
    //                 //此处为数据包处理逻辑
    //                 angle_yaw = uart3_buff[4] << 8 | uart3_buff[3];
    //                 angle_pitch = uart3_buff[6] << 8 | uart3_buff[5];
    //                 angle_roll = uart3_buff[8] << 8 | uart3_buff[7];
    //                 // if (angle_yaw > 18000)angle_yaw -= 65535;

    //                 // if (angle_pitch > 18000)angle_pitch -= 65535;

    //                 // if (angle_roll > 18000)angle_roll -= 65535;


    //                 angle_yaw = angle_yaw / 100.f;
    //                 angle_pitch = angle_pitch / 100.f;
    //                 angle_roll = angle_roll / 100.f;

    //                 buff_size = 0;
    //                 memset(uart3_buff, 0, BUF_SIZE);
    //             }
    //             float i = angle_yaw - angle_yaw_old;
    //             if (i < -180) {
    //                 i += 360;
    //             }
    //             else if (i > 180) {
    //                 i -= 360;
    //             }
    //             angle_yaw_total += i;
    //             angle_yaw_old = angle_yaw;
    //         }
    //     }
    //     else {
    //         buff_size = 0;
    //         memset(uart3_buff, 0, BUF_SIZE);
    //     }
    // }
    // if (buff_size >= 19) {
    //     buff_size = 0;
    //     memset(uart3_buff, 0, BUF_SIZE);
    // }
}

IFX_INTERRUPT(uart3_er_isr, 0, UART3_ER_INT_PRIO) {
    interrupt_global_enable(0);                     // 开启中断嵌套
    IfxAsclin_Asc_isrError(&uart3_handle);





}
// **************************** 串口中断函数 ****************************
