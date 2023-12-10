/*---------------------------------------------------------------------
                          驺吾图传工具函数
													
【平    台】RT1064
【编    写】满心欢喜
【联系方式】QQ：320388825 微信：LHD0617_
【最后更新】2021.11.30
【编译平台】MDK 5.28.0
【功    能】驺吾图传工具函数
【注意事项】本程序只供学习使用，未经作者许可，不得用于其它任何用途。
---------------------------------------------------------------------*/
/*包含头文件*/
#include "ZW_Tools.h"

/************************************************************
												 驺吾图传
								
【函数名称】ZW_Send_Image
【功    能】图像发送函数
【参    数】ZW_Image二值化图像数组（非0及1）的二值化图像
【返 回 值】无
【实    例】ZW_Send_Image(mt9v03x_csi_image);
【注意事项】①只能传入二值化图像（非0及1）
						②.h文件中宏定义尺寸必须与上位机所填写的尺寸对应否则无法使用
						③该函数将会对图像进行重新编码再向上位机发送

************************************************************/
uint8 ZW_Image[ZW_ImgSize_H][ZW_ImgSize_W];
void ZW_Send_Image(ZW_uint8* ZW_Image)
{
	ZW_uint8 ZW_data;
	ZW_uint16 ZW_i;
	ZW_uint8 ZW_j;

	for(ZW_i = 0; ZW_i < ZW_ImgSize_H*ZW_ImgSize_W; ZW_i += 8)
	{
		ZW_data = 0;
		for(ZW_j = 0; ZW_j < 8; ZW_j++)
		{
			if(*(ZW_Image + ZW_i + ZW_j))		ZW_data |= 0x01 << (7-ZW_j);
		}
		ZW_Putchar(ZW_data);
	}
}
void send_pc(void)
{
    // 发送数据
    float ch[4];
    float a = 0;
    a = (Speed_Left+Differential_speed_1);
    ch[0] =  (float) Error;
    ch[1] =  (float) yawspeed;
	ch[2] =  (float)leftline_Curvature;
    ch[3] = (float) a;

	seekfree_wireless_send_buff((char *)ch, sizeof(float) * 4);
    // 发送帧尾
    char tail[4] = {0x00, 0x00, 0x80, 0x7f};
	seekfree_wireless_send_buff(tail, 4);
}

