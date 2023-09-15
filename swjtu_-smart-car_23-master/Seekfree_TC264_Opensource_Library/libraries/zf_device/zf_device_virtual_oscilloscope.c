/*********************************************************************************************************************
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
* 文件名称          zf_device_virtual_oscilloscope
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

#include "zf_device_virtual_oscilloscope.h"

uint8 virtual_oscilloscope_data[10];

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     CRC 校验
// 参数说明     buff            需要进行 CRC 计算的数据地址
// 参数说明     crc_cnt         需要进行 CRC 计算的数据个数
// 返回参数     uint16          CRC 校验结果
// 使用示例     crc_16  = crc_check(virtual_oscilloscope_data, 8);
// 备注信息     内部使用 用户无需关心
//-------------------------------------------------------------------------------------------------------------------
static uint16 crc_check (uint8 *buff, uint8 crc_cnt)
{
    uint16 crc_temp;
    uint8 i, j;
    crc_temp = 0xffff;

    for(i = 0; i < crc_cnt; i ++)
    {
        crc_temp ^= buff[i];
        for(j = 0; j < 8; j ++)
        {
            if (crc_temp & 0x01)
            {
                crc_temp = (crc_temp >> 1) ^ 0xa001;
            }
            else
            {
                crc_temp = crc_temp >> 1;
            }
        }
    }
    return(crc_temp);
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     虚拟示波器数据转换函数
// 参数说明     data1           要发送的第一个数据
// 参数说明     data2           要发送的第二个数据
// 参数说明     data3           要发送的第三个数据
// 参数说明     data4           要发送的第四个数据
// 返回参数     void
// 使用示例     uint8 data_buffer[10];
//              virtual_oscilloscope_data_conversion(100, 200, 300, 400, data_buffer);
//              wireless_uart_send_buff(data_buffer, 10);
// 备注信息     这个函数不带发送 他只是处理数据
//-------------------------------------------------------------------------------------------------------------------
void virtual_oscilloscope_data_conversion (const int16 data1, const int16 data2, const int16 data3, const int16 data4)
{
    uint16 crc_16 = 0;

    virtual_oscilloscope_data[0] = (uint8)((uint16)data1 & 0xff);
    virtual_oscilloscope_data[1] = (uint8)((uint16)data1 >> 8);

    virtual_oscilloscope_data[2] = (uint8)((uint16)data2 & 0xff);
    virtual_oscilloscope_data[3] = (uint8)((uint16)data2 >> 8);

    virtual_oscilloscope_data[4] = (uint8)((uint16)data3 & 0xff);
    virtual_oscilloscope_data[5] = (uint8)((uint16)data3>>8);

    virtual_oscilloscope_data[6] = (uint8)((uint16)data4 & 0xff);
    virtual_oscilloscope_data[7] = (uint8)((uint16)data4 >> 8);

    crc_16  = crc_check(virtual_oscilloscope_data, 8);
    virtual_oscilloscope_data[8] = (uint8)(crc_16 & 0xff);
    virtual_oscilloscope_data[9] = (uint8)(crc_16 >> 8);
}
