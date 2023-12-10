// 固件版本v2.1
#include "pico_link_ii.h"

//------------用户替换函数需要包含的头文件-----------------------
#include "zf_common_headfile.h" // 逐飞TC264库版本3.1.9

//--------------------UART 宏定义------------------------------
// UART发送多个字节，需要自己找函数定义, data:发送内容首地址, length:发送长度
#define UART_SEND_BYTES(data, length) uart_write_buffer((UART_2), (data), (length))

//----------------------SPI 宏定义-----------------------------
// SPI发送多个字节，需要自己找函数定义, data:发送内容首地址, length:发送长度
#define SPI_SEND_BYTES(data, length) spi_write_8bit_array((SPI_0), (data), (length))

//----------------------发送缓冲区------------------------------
#define BUFFER_SIZE (25000) // 发送缓冲区大小, 4字节倍数, 最大65536, 建议略大于图像字节数，太大会占用过多内存
static pico_uint8 send_buffer[BUFFER_SIZE] = { 0 };

//*********************************************************************************************************

/**
 * @description: Pico Link II串口发送多个字节
 * @param {pico_uint8} *data 数据指针
 * @param {pico_uint32} length 数据长度
 * @return {*}
 */
void pico_send_bytes_uart(pico_uint8* data, pico_uint32 length) {
    if (length > 0) {
        UART_SEND_BYTES(data, length);
    }
}

/**
 * @description: Pico Link II串口发送多个字节, 带帧头帧尾
 * @param {pico_uint8} *data 数据
 * @param {pico_uint32} length 数据长度
 * @param {pico_uint8} *head 帧头
 * @param {pico_uint32} head_len 帧头长度
 * @param {pico_uint8} *tail 帧尾
 * @param {pico_uint32} tail_len 帧尾长度
 * @return {*}
 */
void pico_send_bytes_uart_with_head_tail(pico_uint8* data, pico_uint32 length, pico_uint8* head, pico_uint32 head_len, pico_uint8* tail, pico_uint32 tail_len) {
    if (head_len > 0) {
        pico_send_bytes_uart(head, head_len);
    }
    pico_send_bytes_uart(data, length);
    if (tail_len > 0) {
        pico_send_bytes_uart(tail, tail_len);
    }
}

/**
 * @description:Pico Link II SPI发送多个字节
 * @param {pico_uint8} *data 数据指针
 * @param {pico_uint32} length 数据长度
 * @return {*}
 */
void pico_send_bytes_spi(pico_uint8* data, pico_uint32 length) {
    pico_uint8 over_len = 0;
    while (length > (BUFFER_SIZE - 1)) {
        memcpy(send_buffer, data, (BUFFER_SIZE - 1));
        send_buffer[BUFFER_SIZE - 1] = 1;
        SPI_SEND_BYTES(send_buffer, BUFFER_SIZE);
        data += (BUFFER_SIZE - 1);
        length -= (BUFFER_SIZE - 1);
    }
    if (length > 0) {
        memcpy(send_buffer, data, length);
        over_len = 4 - length % 4;
        send_buffer[length + over_len - 1] = over_len;
        SPI_SEND_BYTES(send_buffer, (length + over_len));
    }
}

/**
 * @description: Pico Link II SPI发送多个字节, 带帧头帧尾
 * @param {pico_uint8} *data 数据
 * @param {pico_uint32} length 数据长度
 * @param {pico_uint8} *head 帧头
 * @param {pico_uint32} head_len 帧头长度
 * @param {pico_uint8} *tail 帧尾
 * @param {pico_uint32} tail_len 帧尾长度
 * @return {*}
 */
void pico_send_bytes_spi_with_head_tail(pico_uint8* data, pico_uint32 length, pico_uint8* head, pico_uint32 head_len, pico_uint8* tail, pico_uint32 tail_len) {
    pico_uint8 over_len = 0;
    pico_uint32 head_tail_len = head_len + tail_len;
    pico_uint32 pkg_len = BUFFER_SIZE - head_tail_len - 1;
    if (head_len > 0) {
        memcpy(send_buffer, head, head_len);
        if (length > pkg_len) {
            over_len = 4 - (head_len + pkg_len) % 4;
            memcpy((send_buffer + head_len), data, pkg_len);
            send_buffer[head_len + pkg_len + over_len - 1] = over_len;
            SPI_SEND_BYTES(send_buffer, (head_len + pkg_len + over_len));
            data += pkg_len;
            length -= pkg_len;
        }
        else {
            over_len = 4 - (head_len + length + tail_len) % 4;
            memcpy((send_buffer + head_len), data, length);
            if (tail_len > 0) {
                memcpy((send_buffer + head_len + length), tail, tail_len);
            }
            send_buffer[head_len + length + tail_len + over_len - 1] = over_len;
            SPI_SEND_BYTES(send_buffer, (head_len + length + tail_len + over_len));
            return;
        }
    }

    over_len = 4 - pkg_len % 4;
    while (length > pkg_len) {
        memcpy(send_buffer, data, pkg_len);
        send_buffer[pkg_len + over_len - 1] = over_len;
        SPI_SEND_BYTES(send_buffer, (pkg_len + over_len));
        data += pkg_len;
        length -= pkg_len;
    }

    if (length > 0) {
        memcpy(send_buffer, data, length);
        if (tail_len > 0) {
            memcpy((send_buffer + length), tail, tail_len);
        }
        over_len = 4 - (length + tail_len) % 4;
        send_buffer[length + tail_len + over_len - 1] = over_len;
        SPI_SEND_BYTES(send_buffer, (length + tail_len + over_len));
    }
}

//*********************************************************************************************************

/**
 * @description: Pico Link II发送图像到致用上位机
 * @param {pico_uint8} *image 图像指针
 * @param {pico_uint32} image_h 图像高度
 * @param {pico_uint32} image_w 图像宽度
 * @return {*}
 */
void ipc_send_image(pico_uint8* image, pico_uint32 image_h, pico_uint32 image_w) {
    pico_send_bytes_with_head_tail(image, image_h * image_w, (pico_uint8*)"CSU", 3, (pico_uint8*)"USC", 3);
}

//*********************************************************************************************************