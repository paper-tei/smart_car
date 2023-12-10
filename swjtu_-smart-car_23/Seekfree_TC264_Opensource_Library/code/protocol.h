
#ifndef __PROTOCOL_H__
#define __PROTOCOL_H__

/*****************************************************************************/
/* Includes                                                                  */
/*****************************************************************************/
#include "zf_common_headfile.h"
#include "zf_driver_uart.h"

#ifdef _cplusplus
extern "C" {
#endif   

    /*==========================COMMON=====================================*/
#define PID_ASSISTANT_EN (0) // 1:使用PID调试助手显示波形，0：使用串口直接打印数据


/* 数据接收缓冲区大小 */
#define PROT_FRAME_LEN_RECV  128

/* 校验数据的长度 */
#define PROT_FRAME_LEN_CHECKSUM    1

/* 数据头结构体 */
/* 数据头结构体 */
//typedef  struct __packed
//{
//  uint32_t head;    // 包头
//  uint8_t ch;       // 通道
//  uint32_t len;     // 包长度
//  uint8_t cmd;      // 命令
//} packet_head_t;

//#pragma pack(push, 1)


    typedef  struct {
        uint32_t head;    // 包头
        uint8_t ch;       // 通道
        uint32_t len;     // 包长度
        uint8_t cmd;      // 命令
    } packet_head_t;


    //#pragma pack(pop)


    /* 联合体（方便数据转换） */
    typedef union {
        float f;
        int i;
    }type_cast_t;

#define FRAME_HEADER     0x59485A53    // 帧头

    /* 通道宏定义 */
    //#define CURVES_CH1      0x01
    //#define CURVES_CH2      0x02
    //#define CURVES_CH3      0x03
    //#define CURVES_CH4      0x04
    //#define CURVES_CH5      0x05

    //typedef enum {
    //CURVES_CH1,
    //CURVES_CH2,
    //CURVES_CH3,
    //CURVES_CH4,
    //CURVES_CH5
    //} Curves_Channel;

    typedef enum {
        CURVES_CH1 = 0x01,
        CURVES_CH2 = 0x02,
        CURVES_CH3 = 0x03,
        CURVES_CH4 = 0x04,
        CURVES_CH5 = 0x05
    } Curves_Channel;




    /* 指令(下位机 -> 上位机) */
#define SEND_TARGET_CMD      0x01     // 发送上位机通道的目标值
#define SEND_FACT_CMD        0x02     // 发送通道实际值
#define SEND_P_I_D_CMD       0x03     // 发送 PID 值（同步上位机显示的值）
#define SEND_START_CMD       0x04     // 发送启动指令（同步上位机按钮状态）
#define SEND_STOP_CMD        0x05     // 发送停止指令（同步上位机按钮状态）
#define SEND_PERIOD_CMD      0x06     // 发送周期（同步上位机显示的值）

/* 指令(上位机 -> 下位机) */
#define SET_P_I_D_CMD        0x10     // 设置 PID 值
#define SET_TARGET_CMD       0x11     // 设置目标值
#define START_CMD            0x12     // 启动指令
#define STOP_CMD             0x13     // 停止指令
#define RESET_CMD            0x14     // 复位指令
#define SET_PERIOD_CMD       0x15     // 设置周期

/* 空指令 */
#define CMD_NONE             0xFF     // 空指令

/*********************************************************************************************
协议数据示例

1.下发目标值55：
    |----包头----|通道|---包长度---|命令|----参数---|校验|
    | 0  1  2  3 |  4 |  5  6  7  8|  9 |10 11 12 13| 14 | <-索引
    |53 5A 48 59 | 01 | 0F 00 00 00| 11 |37 00 00 00| A6 | <-协议帧数

2.下发PID（P=1 I=2 D=3）：
    |----包头----|通道|---包长度---|命令|---参数P---|---参数I---|---参数D---|校验|
    | 0  1  2  3 |  4 |  5  6  7  8|  9 |10 11 12 13|14 15 15 17|18 19 20 21| 22 | <-索引
    |53 5A 48 59 | 01 | 17 00 00 00| 10 |00 00 80 3F|00 00 00 40|00 00 40 40| F5 | <-协议帧数

**********************************************************************************************/

/* 索引值宏定义 */
#define HEAD_INDEX_VAL       0x3u     // 包头索引值（4字节）
#define CHX_INDEX_VAL        0x4u     // 通道索引值（1字节）
#define LEN_INDEX_VAL        0x5u     // 包长索引值（4字节）
#define CMD_INDEX_VAL        0x9u     // 命令索引值（1字节）

/* 交换高低字节（未用到） */
#define EXCHANGE_H_L_BIT(data)      ((((data) << 24) & 0xFF000000) |\
                                     (((data) <<  8) & 0x00FF0000) |\
                                     (((data) >>  8) & 0x0000FF00) |\
                                     (((data) >> 24) & 0x000000FF))     
/* 合成为一个字 */
#define COMPOUND_32BIT(data)        (((*(data-0) << 24) & 0xFF000000) |\
                                     ((*(data-1) << 16) & 0x00FF0000) |\
                                     ((*(data-2) <<  8) & 0x0000FF00) |\
                                     ((*(data-3) <<  0) & 0x000000FF))      

/**
 * @brief   接收数据处理
 * @param   *data:  要计算的数据的数组.
 * @param   data_len: 数据的大小
 * @return  void.
 */
    void protocol_data_recv(uint8_t* data, uint16_t data_len);

    /**
     * @brief   初始化接收协议
     * @param   void
     * @return  初始化结果.
     */
    int32_t protocol_init(void);

    /**
     * @brief   接收的数据处理
     * @param   void
     * @return  -1：没有找到一个正确的命令.
     */
    int8_t receiving_process(void);

    /**
      * @brief 设置上位机的值
      * @param cmd：命令
      * @param ch: 曲线通道
      * @param data：参数指针
      * @param num：参数个数
      * @retval 无
      */
    void set_computer_value(uint8_t cmd, uint8_t ch, void* data, uint8_t num);

    //下位机发送pid理想值和实际值给上位机
    void Data_uploaded_to_computer(int value, int ActualValue, Curves_Channel channel);

    //void  lower_send_PID(PID_IncTypeDef *pid,Curves_Channel channel);

#ifdef _cplusplus
}
#endif   

#endif
