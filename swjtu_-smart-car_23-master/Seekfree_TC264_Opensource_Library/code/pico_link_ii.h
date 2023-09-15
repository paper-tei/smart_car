#ifndef __PICO_LINK_II_H__
#define __PICO_LINK_II_H__

#include "string.h"

#define PICO_PROTOCOL_UART
// #define PICO_PROTOCOL_SPI

#ifdef PICO_PROTOCOL_UART
#define pico_send_bytes(data, length) pico_send_bytes_uart(data, length)
#define pico_send_bytes_with_head_tail(data, length, head, head_len, tail, tail_len) pico_send_bytes_uart_with_head_tail(data, length, head, head_len, tail, tail_len)
#endif

#ifdef PICO_PROTOCOL_SPI
#define pico_send_bytes(data, length) pico_send_bytes_spi(data, length)
#define pico_send_bytes_with_head_tail(data, length, head, head_len, tail, tail_len) pico_send_bytes_spi_with_head_tail(data, length, head, head_len, tail, tail_len)
#endif

// 数据类型声明
typedef unsigned char pico_uint8;
typedef unsigned short int pico_uint16;
typedef unsigned long int pico_uint32;

void pico_send_bytes_uart(pico_uint8* data, pico_uint32 length);
void pico_send_bytes_spi(pico_uint8* data, pico_uint32 length);
void pico_send_bytes_uart_with_head_tail(pico_uint8* data, pico_uint32 length, pico_uint8* head, pico_uint32 head_len, pico_uint8* tail, pico_uint32 tail_len);
void pico_send_bytes_spi_with_head_tail(pico_uint8* data, pico_uint32 length, pico_uint8* head, pico_uint32 head_len, pico_uint8* tail, pico_uint32 tail_len);

void ipc_send_image(pico_uint8* image, pico_uint32 image_h, pico_uint32 image_w);

#endif