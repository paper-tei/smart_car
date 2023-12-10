#ifndef __PICO_GY_H__
#define __PICO_GY_H__

#include "pico_link_ii.h"

void sendimg(pico_uint8 *image, pico_uint8 width, pico_uint8 height);
void sendimg_A(pico_uint8 *image, pico_uint8 width, pico_uint8 height);
void sendimg_zoom(pico_uint8 *image, pico_uint8 width, pico_uint8 height, pico_uint8 dis_width, pico_uint8 dis_height);
void sendimg_binary(pico_uint8 *image, pico_uint8 width, pico_uint8 height, pico_uint8 otu);
void sendimg_binary_zoom(pico_uint8 *image, pico_uint8 width, pico_uint8 height, pico_uint8 dis_width, pico_uint8 dis_height, pico_uint8 otu);
void sendimg_binary_CHK(pico_uint8 *image, pico_uint8 width, pico_uint8 height, pico_uint8 otu, pico_uint8 chk);
void put_int32(pico_uint8 name, int dat);
void put_float(pico_uint8 name, float dat);
void sendline_clear(pico_uint8 color, pico_uint8 width, pico_uint8 height);
void sendline_xy(pico_uint8 *line_zx, pico_uint8 *line_yx, pico_uint32 len);
void sendline(pico_uint8 color, pico_uint8 *buff, pico_uint32 len);
void sendline2(pico_uint8 color, pico_uint8 *linex, pico_uint8 *liney, pico_uint32 len);
void sendpoint(pico_uint8 color, pico_uint8 x, pico_uint8 y, pico_uint8 type);

#endif