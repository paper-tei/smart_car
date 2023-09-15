// 协议版本3.6.1
#include "pico_gy.h"
#include "pico_link_ii.h"

//----------------------发送缓冲区------------------------------
#define BUFFER_SIZE (25000) // 发送缓冲区大小，建议略大于图像字节数，太大会占用过多内存
static pico_uint8 send_buffer[BUFFER_SIZE] = {0};

#define GY_SEND_BYTES(data, length) pico_send_bytes(data, length)
#define GY_SEND_BYTES_WITH_HEAD(data, length, head, head_len) pico_send_bytes_with_head_tail(data, length, head, head_len, 0, 0)

/*
 * GL-Shw智能车上位机  qq群:297809802 qq1849057843
 *
 *目录：
 *|||||||||||||||||||【以下图传协议只能选择一种】
 * 一、灰度图传
 *  ①普通灰度图传
 *  ②抗干扰灰度图传
 *  ③压缩灰度图传
 * 二、二值化图传（*该协议带有40倍左右的压缩率和校验 ）[强烈推荐]
 *  ①二值化图传
 *  ②带校验二值化图传
 *  ③修改图像大小的二值化图传
 *
 * ||||||||||||||||||【以下协议可以与图传协议并行使用】
 * 三、图传边界（寻线的边界信息，推荐搭配二值图传，实时查看赛道与寻线补线）[推荐]
 * 四、数据标签（数据示波，数据颜色标定，数据监视，仪表盘）[强烈推荐]
 *                          更新时间：2022年12月9日
 */

//--------------普通灰度图传-------------------//
//*image图像地址 width图像宽 height图像高
// 例：sendimg(mt9v03x_image_dvp[0], MT9V03X_DVP_W, MT9V03X_DVP_H);
// 数据包大小:6+width * height(图传一帧的字节数)
void sendimg(pico_uint8 *image, pico_uint8 width, pico_uint8 height)
{
    pico_uint8 dat[6] = {0x21, 0x7A, width, height, 0x21, 0x7A};
    GY_SEND_BYTES_WITH_HEAD(image, width * height, dat, 6);
}
//--------------抗干扰灰度图传-------------------//
// 当丢失数据的情况下，该协议能重新定位行来实现一定程度抗干扰能力
//*image图像地址 width图像宽 height图像高
// 例：sendimg(mt9v03x_image_dvp[0], MT9V03X_DVP_W, MT9V03X_DVP_H);
// 数据包大小:6+（width+3） * height(图传一帧的字节数)
void sendimg_A(pico_uint8 *image, pico_uint8 width, pico_uint8 height)
{
    pico_uint8 flag1[6] = {0x21, 0x7A, width, height, (width + height) / 2, 0x7A};
    pico_uint8 line = 0;
    pico_uint8 flag2[3] = {21, 0, 133};
    pico_uint32 len = 0;
    for (line = 0; line < width; line++)
    {
        flag2[1] = line;
        memcpy((send_buffer + len), flag2, 3);
        len += 3;
        memcpy((send_buffer + len), (image + line * height), height);
        len += height;
    }
    GY_SEND_BYTES_WITH_HEAD(send_buffer, len, flag1, 6);
}
//--------------压缩灰度图传-------------------//
// 发送压缩图像 例如 120x180的图像太大 传输速度慢  则可以发送 60x90的图像来提高速度
//*image图像地址 width图像宽 height图像高dis_width压缩后的图像宽 dis_height压缩后的图像高
// 例：sendimg(mt9v03x_image_dvp[0], MT9V03X_DVP_W, MT9V03X_DVP_H,MT9V03X_DVP_W/2,MT9V03X_DVP_H/2);
// 数据包大小:6+dis_width * dis_height(图传一帧的字节数)
void sendimg_zoom(pico_uint8 *image, pico_uint8 width, pico_uint8 height, pico_uint8 dis_width, pico_uint8 dis_height)
{
    pico_uint8 dat[6] = {0x21, 0x7A, dis_width, dis_height, 0x21, 0x7A};
    pico_uint8 i, j;
    for (j = 0; j < dis_height; j++)
    {
        for (i = 0; i < dis_width; i++)
        {
            send_buffer[i + j * dis_width] = *(image + (j * height / dis_height) * width + i * width / dis_width); // 读取像素点
        }
    }
    GY_SEND_BYTES_WITH_HEAD(send_buffer, (dis_width * dis_height), dat, 6);
}

//--------------二值化图传-------------------//
// image图像地址 width图像宽 height图像高 otu二值化阈值
// 例：sendimg_binary(mt9v03x_image_dvp[0], MT9V03X_DVP_W, MT9V03X_DVP_H,100);
void sendimg_binary(pico_uint8 *image, pico_uint8 width, pico_uint8 height, pico_uint8 otu)
{
    pico_uint8 dat[6] = {0x7A, 0x21, width, height, 0x7A, 0x21};
    int databool = 255;
    pico_uint8 lon = 1;
    int data = 255;
    pico_uint8 line = 0, col = 0;
    pico_uint32 len = 0;
    for (line = 0; line < width; line++)
    {
        for (col = 0; col < height; col++)
        {
            if (*(image + line * height + col) > otu)
                data = 255;
            else
                data = 0;
            if (data == databool)
            {
                lon++;
            }
            else
            {
                send_buffer[len++] = lon;
                lon = 1;
            }
            if (lon == 190)
            {
                send_buffer[len++] = lon - 1;
                send_buffer[len++] = 0;
                lon = 1;
            }
            databool = data;
        }
    }
    GY_SEND_BYTES_WITH_HEAD(send_buffer, len, dat, 6);
}
// 压缩二值图传
// uint16 dis_width, uint16 dis_height 要压缩图像大小
void sendimg_binary_zoom(pico_uint8 *image, pico_uint8 width, pico_uint8 height, pico_uint8 dis_width, pico_uint8 dis_height, pico_uint8 otu)
{
    pico_uint8 dat[6] = {0x7A, 0x21, dis_width, dis_height, 0x7A, 0x21};
    int databool = 255;
    pico_uint8 lon = 1;
    int data = 255;
    pico_uint8 i, j;
    pico_uint32 len = 0;
    for (j = 0; j < dis_height; j++)
    {
        for (i = 0; i < dis_width; i++)
        {
            if (*(image + (j * height / dis_height) * width + i * width / dis_width) > otu) // 读取像素点
                data = 255;
            else
                data = 0;
            if (data == databool)
            {
                lon++;
            }
            else
            {
                send_buffer[len++] = lon;
                lon = 1;
            }
            if (lon == 190)
            {
                send_buffer[len++] = lon - 1;
                send_buffer[len++] = 0;
                lon = 1;
            }
            databool = data;
        }
    }
    GY_SEND_BYTES_WITH_HEAD(send_buffer, len, dat, 6);
}
// 带有校验的二值图传
// chk值越大 抗干扰越强 值0-55
// 请根据实际使用情况进行调整
void sendimg_binary_CHK(pico_uint8 *image, pico_uint8 width, pico_uint8 height, pico_uint8 otu, pico_uint8 chk)
{
    chk = chk > 0 ? chk : 0;
    chk = chk < 56 ? chk : 55;
    pico_uint8 dat[7] = {0x7A, 0x21, width, height, 0x7A, 0x21, 200 + chk};
    int databool = 255;
    pico_uint8 lon = 0;
    int data = 255;
    pico_uint8 line = 0, col = 0;
    int imglon = 0;
    int imgdatlo = width * height / chk;
    pico_uint8 CHK = 0;
    pico_uint32 len = 0;
    for (line = 0; line < width; line++)
    {
        for (col = 0; col < height; col++)
        {
            imglon++;
            if (*(image + line * height + col) > otu)
                data = 255;
            else
                data = 0;
            if (data == databool)
            {
                lon++;
            }
            else
            {
                send_buffer[len++] = lon;
                lon = 1;
            }
            if (imglon == imgdatlo)
            {
                CHK++;
                send_buffer[len++] = lon;
                data = 255;
                databool = 255;
                send_buffer[len++] = 200 + CHK;
                lon = 0;
                imglon = 0;
            }
            if (lon == 190)
            {
                send_buffer[len++] = lon;
                send_buffer[len++] = 0;
                lon = 0;
            }
            databool = data;
        }
    }
    GY_SEND_BYTES_WITH_HEAD(send_buffer, len, dat, 7);
}

// 内部调用
static inline pico_uint16 swj_CRC(pico_uint8 *Buf, pico_uint8 CRC_sta, pico_uint8 CRC_CNT)
{
    pico_uint16 CRC_Temp;
    pico_uint8 i, j;
    CRC_Temp = 0xffff;

    for (i = CRC_sta; i < CRC_CNT; i++)
    {
        CRC_Temp ^= Buf[i];
        for (j = 0; j < 8; j++)
        {
            if (CRC_Temp & 0x01)
                CRC_Temp = (CRC_Temp >> 1) ^ 0xa001;
            else
                CRC_Temp = CRC_Temp >> 1;
        }
    }
    return (CRC_Temp);
}

//--------------数据标签（数据示波，数据颜色标定，数据监视，仪表盘）-------------------//
// 一个数据占一个地址,会直接出现在图传页面的右栏 点一下可以示波，右键可以设置颜色标定 设定好
// 阈值会颜色标定数据 快速监视条件有没有触发，也可以绑定表盘，速度更直观。录制时会同步录制
// 类似于黑匣子，小车什么状态一眼便知
// name数据标识(通道、地址)[0-255]  dat:数据
// 例:int a=0;put_int32(0,a);

// 带有校验的数据示波
void put_int32(pico_uint8 name, int dat)
{
    pico_uint8 datc[10] = {197, name, 1, 0, 0, 0, 0, 0, 0, 198};
    datc[3] = (pico_uint8)(dat & 0xFF);
    datc[4] = (pico_uint8)((dat & 0xFF00) >> 8);
    datc[5] = (pico_uint8)((dat & 0xFF0000) >> 16);
    datc[6] = (pico_uint8)((dat >> 24) & 0xFF);
    pico_uint8 crc[6] = {name, 1, datc[3], datc[4], datc[5], datc[6]};
    pico_uint8 CRC16 = swj_CRC(crc, 0, 6);
    datc[7] = (pico_uint8)(CRC16 & 0xff);
    datc[8] = (pico_uint8)(CRC16 >> 8);
    GY_SEND_BYTES(datc, 10);
}

void put_float(pico_uint8 name, float dat)
{
    pico_uint8 datc[10] = {197, name, 2, 0, 0, 0, 0, 0, 0, 198};
    char farray[4] = {0};
    *(float *)farray = dat;
    unsigned char *p = (unsigned char *)&dat + 3;
    datc[3] = *(p - 3);
    datc[4] = *(p - 2);
    datc[5] = *(p - 1);
    datc[6] = *p;
    pico_uint8 crc[6] = {name, 2, datc[3], datc[4], datc[5], datc[6]};
    pico_uint16 CRC16 = swj_CRC(crc, 0, 6);
    datc[7] = (pico_uint8)(CRC16 & 0xff);
    datc[8] = (pico_uint8)(CRC16 >> 8);
    GY_SEND_BYTES(datc, 10);
}

//--------------传线-------------------//
/*
 * 可以将寻线得到的左右边线，拟的中线发送到上位机查看
 * 例如：
 * 处理 图像img[H][W];得到左右边线存放在zx[H] yx[H] 拟出来的中线为w[H]
 * sendline_clear(swj_BLACK,H,W);//清屏 背景黑色
 * sendline(swj_WHITE,zx,H);//发送左边线
 * sendline(swj_WHITE,yx,H);//发送右边线
 * sendline(swj_WHITE,wx,H);//发送中线
 * 例如：
 * sendline_clear(swj_BLACK,H,W);//清屏 背景黑色
 * sendline_xy(zx,yx,H)//发送左右边线
 * sendline(swj_WHITE,wx,H);//发送中线
 *
 * 如上两个方法效果一致的，但下面可以在上位机上对应使用赛道还原功能
 * 注意：
 * ①每发送完一帧的图像边线 就要调用sendline_clear进行清屏
 * ②如果调用sendline_xy函数并使用上位机赛道还原功能时，若再调用sendline 一定放在sendline_xy后面 防止被覆盖
 * */
#define swj_BLACK 0
#define swj_WHITE 1
#define swj_RED 2
#define swj_GREEN 3
#define swj_BLUE 4
#define swj_PURPLE 5
#define swj_YELLOW 6
#define swj_CYAN 7
#define swj_ORANGE 8
// 清空线条   color清屏后的背景颜色  uint16 width uint16 height 图像的大小
void sendline_clear(pico_uint8 color, pico_uint8 width, pico_uint8 height)
{
    pico_uint8 dat[6] = {0x21, 0x7A, width, height, color, 0x21};
    GY_SEND_BYTES(dat, 6);
}
// 图传赛道边界  uint8_t *zx:左边界   uint8_t *yx:右边界, uint32_t len发送的边线长度
// 该函数与下放函数分别发送两个边线有何不同? 该函数可对应上位机还原赛道的功能*  注意先后顺序
void sendline_xy(pico_uint8 *line_zx, pico_uint8 *line_yx, pico_uint32 len)
{
    pico_uint8 dat[5] = {0x21, 9, len, 255, 255};
    memcpy((send_buffer + 0), line_zx, len);
    memcpy((send_buffer + len), line_yx, len);
    GY_SEND_BYTES_WITH_HEAD(send_buffer, (len + len), dat, 5);
}
/*默认每行一个点*/
// 绘制边线   color边线颜色  uint8_t *buff 发送的边线数组地址  len发送的边线长度
void sendline(pico_uint8 color, pico_uint8 *buff, pico_uint32 len)
{
    pico_uint8 dat[5] = {0x21, color, len, 255, 255};
    GY_SEND_BYTES_WITH_HEAD(buff, len, dat, 5);
}
/*说明:
 * 例如有三个点 a(x1,y1)b(x2,y2)c(x3,y3)
 * 则 uint8 x[3]={x1,x2,x3};uint8 y[3]={y1,y2,y3};
 *  sendline2(swj_WHITE,x,y,3);
 *  sendline函数只能按顺序每行一点发送边界点
 *  sendline2函数针对于八邻域等 每行不固定点数的边界
 *           也适用于发送特殊点 例如拐点 灵活运用
 *
 * */
// 无序绘制边线  color边线颜色 linex对应点的x坐标集合 liney对应点的y坐标集合  len发送的边线长度
void sendline2(pico_uint8 color, pico_uint8 *linex, pico_uint8 *liney, pico_uint32 len)
{
    pico_uint8 dat[5] = {0x21, color, len, 254, 255};
    memcpy((send_buffer + 0), linex, len);
    memcpy((send_buffer + len), liney, len);
    GY_SEND_BYTES_WITH_HEAD(send_buffer, (len + len), dat, 5);
}

#define swj_point_type1 1 // 小十字 3x3
#define swj_point_type2 2 // 中十字 5x5
#define swj_point_type3 3 // 大十字 7x7
#define swj_point_type4 4 // 小  X  3x3
#define swj_point_type5 5 // 中  X  5x5
#define swj_point_type6 6 // 大  X  7x7
#define swj_point_type7 7 // 全屏十字

// 标志点（列如拐点 特殊补线的点）
// 例如 点（10,11）sendpoint(swj_RED,10,11,swj_point_type1);//（10，11）处出现红色小十字
// 颜色 坐标x 坐标y 点类型（见上面的宏定义）
void sendpoint(pico_uint8 color, pico_uint8 x, pico_uint8 y, pico_uint8 type)
{
    pico_uint8 dat[7] = {0x22, color, type, 254, x, y, 255};
    GY_SEND_BYTES(dat, 7);
}

//--------------------------------------------------无线模块-----------------------------------------------//
/*
 * 无线模块 推荐使用逐飞的无线模块（usb to nrf）*购买时建议同时购买ttl，用于配置小车的无线模块
 * 波特率可以最高拉到460800 *对应小车上的程序,小车的无线模块，电脑上的无线模块，上位机波特率一致
 * 无线模块速度比wifi慢 但优点是稳定 对于灰度图传速度不太够用，但有二值化图传优化 适合边图传边示波器
 *
 */
