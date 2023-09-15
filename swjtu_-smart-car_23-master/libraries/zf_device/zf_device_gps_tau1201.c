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
* 文件名称          zf_device_gps_tau1201
* 公司名称          成都逐飞科技有限公司
* 版本信息          查看 libraries/doc 文件夹内 version 文件 版本说明
* 开发环境          ADS v1.8.0
* 适用平台          TC264D
* 店铺链接          https://seekfree.taobao.com/
*
* 修改记录
* 日期              作者                备注
* 2022-09-15       pudding             first version
********************************************************************************************************************/
/********************************************************************************************************************
* 接线定义：
*                  ------------------------------------
*                  模块管脚             单片机管脚
*                  RX                  查看 zf_device_gps_tau1201.h 中 GPS_TAU1201_RX 宏定义
*                  TX                  查看 zf_device_gps_tau1201.h 中 GPS_TAU1201_TX 宏定义
*                  VCC                 3.3V电源
*                  GND                 电源地
*                  ------------------------------------
********************************************************************************************************************/

#include "zf_common_function.h"
#include "zf_common_fifo.h"
#include "zf_driver_delay.h"
#include "zf_driver_uart.h"
#include "zf_device_type.h"
#include "zf_device_gps_tau1201.h"

#define GPS_TAU1201_BUFFER_SIZE     (128)

uint8                       gps_tau1201_flag;                                       // 1：采集完成等待处理数据 0：没有采集完成
gps_info_struct             gps_tau1201;                                            // GPS解析之后的数据

static  uint8               gps_tau1201_state = 0;                                  // 1：GPS初始化完成
static  fifo_struct         gps_tau1201_receiver_fifo;                              //
static  uint8               gps_tau1201_receiver_buffer[GPS_TAU1201_BUFFER_SIZE];   // 数据存放数组

gps_state_enum              gps_gga_state;                                          // gga语句状态
gps_state_enum              gps_rmc_state;                                          // rmc语句状态

static  uint8               gps_gga_buffer[GPS_TAU1201_BUFFER_SIZE];
static  uint8               gps_rmc_buffer[GPS_TAU1201_BUFFER_SIZE];

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     获取指定 ',' 后面的索引
// 参数说明     num             第几个逗号
// 参数说明     *str            字符串
// 返回参数     uint8           返回索引
// 使用示例     get_parameter_index(1, s);
// 备注信息     内部使用
//-------------------------------------------------------------------------------------------------------------------
static uint8 get_parameter_index (uint8 num, char *str)
{
    uint8 i, j = 0;
    char *temp;
    uint8 len = 0, len1;

    temp = strchr(str, '\n');
    if(NULL != temp)
    {
        len = (uint8)((uint32)temp - (uint32)str + 1);
    }

    for(i = 0; i < len; i ++)
    {
        if(str[i] == ',')
        {
            j ++;
        }
        if(j == num)
        {
            len1 =  i + 1;
            break;
        }
    }

    return len1;
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     给定字符串第一个 ',' 之前的数据转换为int
// 参数说明     *s              字符串
// 返回参数     float           返回数值
// 使用示例     get_int_number(&buf[get_parameter_index(7, buf)]);
// 备注信息     内部使用
//-------------------------------------------------------------------------------------------------------------------
static int get_int_number (char *s)
{
    char buf[10];
    uint8 i;
    int return_value;
    i = get_parameter_index(1, s);
    i = i - 1;
    strncpy(buf, s, i);
    buf[i] = 0;
    return_value = func_str_to_int(buf);
    return return_value;
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     给定字符串第一个 ',' 之前的数据转换为float
// 参数说明     *s              字符串
// 返回参数     float           返回数值
// 使用示例     get_float_number(&buf[get_parameter_index(8, buf)]);
// 备注信息     内部使用
//-------------------------------------------------------------------------------------------------------------------
static float get_float_number (char *s)
{
    uint8 i;
    char buf[15];
    float return_value;

    i = get_parameter_index(1, s);
    i = i - 1;
    strncpy(buf, s, i);
    buf[i] = 0;
    return_value = (float)func_str_to_double(buf);
    return return_value;
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     给定字符串第一个 ',' 之前的数据转换为double
// 参数说明     *s              字符串
// 返回参数     double          返回数值
// 使用示例     get_double_number(&buf[get_parameter_index(3, buf)]);
// 备注信息     内部使用
//-------------------------------------------------------------------------------------------------------------------
static double get_double_number (char *s)
{
    uint8 i;
    char buf[15];
    double return_value;

    i = get_parameter_index(1, s);
    i = i - 1;
    strncpy(buf, s, i);
    buf[i] = 0;
    return_value = func_str_to_double(buf);
    return return_value;
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     世界时间转换为北京时间
// 参数说明     *time           保存的时间
// 返回参数     void
// 使用示例     utc_to_btc(&gps->time);
// 备注信息     内部使用
//-------------------------------------------------------------------------------------------------------------------
static void utc_to_btc (gps_time_struct *time)
{
    uint8 day_num;

    time->hour = time->hour + 8;
    if(time->hour > 23)
    {
        time->hour -= 24;
        time->day += 1;

        if(2 == time->month)
        {
            day_num = 28;
            if((time->year % 4 == 0 && time->year % 100 != 0) || time->year % 400 == 0) // 判断是否为闰年
            {
                day_num ++;                                                     // 闰月 2月为29天
            }
        }
        else
        {
            day_num = 31;                                                       // 1 3 5 7 8 10 12这些月份为31天
            if(4  == time->month || 6  == time->month || 9  == time->month || 11 == time->month )
            {
                day_num = 30;
            }
        }

        if(time->day > day_num)
        {
            time->day = 1;
            time->month ++;
            if(time->month > 12)
            {
                time->month -= 12;
                time->year ++;
            }
        }
    }
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     RMC语句解析
// 参数说明     *line           接收到的语句信息
// 参数说明     *gps            保存解析后的数据
// 返回参数     uint8           1：解析成功 0：数据有问题不能解析
// 使用示例     gps_gnrmc_parse((char *)data_buffer, &gps_tau1201);
// 备注信息     内部使用
//-------------------------------------------------------------------------------------------------------------------
static uint8 gps_gnrmc_parse (char *line, gps_info_struct *gps)
{
    uint8 state, temp;

    double  latitude;                                                           // 纬度
    double  longitude;                                                          // 经度

    float lati_cent_tmp, lati_second_tmp;
    float long_cent_tmp, long_second_tmp;
    float speed_tmp;
    char *buf = line;
    uint8 return_state = 0;

    state = buf[get_parameter_index(2, buf)];

    gps->state = 0;
    if (state == 'A')                                                           // 如果数据有效 则解析数据
    {
        return_state = 1;
        gps->state = 1;
        gps -> ns               = buf[get_parameter_index(4, buf)];
        gps -> ew               = buf[get_parameter_index(6, buf)];

        latitude                = get_double_number(&buf[get_parameter_index(3, buf)]);
        longitude               = get_double_number(&buf[get_parameter_index(5, buf)]);

        gps->latitude_degree    = (int)latitude / 100;                          // 纬度转换为度分秒
        lati_cent_tmp           = (latitude - gps->latitude_degree * 100);
        gps->latitude_cent      = (int)lati_cent_tmp;
        lati_second_tmp         = (lati_cent_tmp - gps->latitude_cent) * 10000;
        gps->latitude_second    = (int)lati_second_tmp;

        gps->longitude_degree   = (int)longitude / 100;                         // 经度转换为度分秒
        long_cent_tmp           = (longitude - gps->longitude_degree * 100);
        gps->longitude_cent     = (int)long_cent_tmp;
        long_second_tmp         = (long_cent_tmp - gps->longitude_cent) * 10000;
        gps->longitude_second   = (int)long_second_tmp;

        gps->latitude   = gps->latitude_degree + (double)gps->latitude_cent / 60 + (double)gps->latitude_second / 600000;
        gps->longitude  = gps->longitude_degree + (double)gps->longitude_cent / 60 + (double)gps->longitude_second / 600000;

        speed_tmp       = get_float_number(&buf[get_parameter_index(7, buf)]);  // 速度(海里/小时)
        gps->speed      = speed_tmp * 1.85f;                                    // 转换为公里/小时
        gps->direction  = get_float_number(&buf[get_parameter_index(8, buf)]);  // 角度
    }

    // 在定位没有生效前也是有时间数据的，可以直接解析
    gps->time.hour    = (buf[7] - '0') * 10 + (buf[8] - '0');                   // 时间
    gps->time.minute  = (buf[9] - '0') * 10 + (buf[10] - '0');
    gps->time.second  = (buf[11] - '0') * 10 + (buf[12] - '0');
    temp = get_parameter_index(9, buf);
    gps->time.day     = (buf[temp + 0] - '0') * 10 + (buf[temp + 1] - '0');     // 日期
    gps->time.month   = (buf[temp + 2] - '0') * 10 + (buf[temp + 3] - '0');
    gps->time.year    = (buf[temp + 4] - '0') * 10 + (buf[temp + 5] - '0') + 2000;

    utc_to_btc(&gps->time);

    return return_state;
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     GGA语句解析
// 参数说明     *line           接收到的语句信息
// 参数说明     *gps            保存解析后的数据
// 返回参数     uint8           1：解析成功 0：数据有问题不能解析
// 使用示例     gps_gngga_parse((char *)data_buffer, &gps_tau1201);
// 备注信息     内部使用
//-------------------------------------------------------------------------------------------------------------------
static uint8 gps_gngga_parse (char *line, gps_info_struct *gps)
{
    uint8 state;
    char *buf = line;
    uint8 return_state = 0;

    state = buf[get_parameter_index(2, buf)];

    if (state != ',')
    {
        gps->satellite_used = (uint8)get_int_number(&buf[get_parameter_index(7, buf)]);
        gps->height         = get_float_number(&buf[get_parameter_index(9, buf)]) + get_float_number(&buf[get_parameter_index(11, buf)]);  // 高度 = 海拔高度 + 地球椭球面相对大地水准面的高度
        return_state = 1;
    }

    return return_state;
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     计算从第一个点到第二个点的距离
// 参数说明     latitude1       第一个点的纬度
// 参数说明     longitude1      第一个点的经度
// 参数说明     latitude2       第二个点的纬度
// 参数说明     longitude2      第二个点的经度
// 返回参数     double          返回两点距离
// 使用示例     get_two_points_distance(latitude1_1, longitude1, latitude2, longitude2);
// 备注信息
//-------------------------------------------------------------------------------------------------------------------
double get_two_points_distance (double latitude1, double longitude1, double latitude2, double longitude2)
{
    const double EARTH_RADIUS = 6378137;                                        // 地球半径(单位：m)
    double rad_latitude1;
    double rad_latitude2;
    double rad_longitude1;
    double rad_longitude2;
    double distance;
    double a;
    double b;

    rad_latitude1 = ANGLE_TO_RAD(latitude1);                                    // 根据角度计算弧度
    rad_latitude2 = ANGLE_TO_RAD(latitude2);
    rad_longitude1 = ANGLE_TO_RAD(longitude1);
    rad_longitude2 = ANGLE_TO_RAD(longitude2);

    a = rad_latitude1 - rad_latitude2;
    b = rad_longitude1 - rad_longitude2;

    distance = 2 * asin(sqrt(pow(sin(a / 2), 2) + cos(rad_latitude1) * cos(rad_latitude2) * pow(sin(b / 2), 2)));   // google maps 里面实现的算法
    distance = distance * EARTH_RADIUS;

    return distance;
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     计算从第一个点到第二个点的方位角
// 参数说明     latitude1       第一个点的纬度
// 参数说明     longitude1      第一个点的经度
// 参数说明     latitude2       第二个点的纬度
// 参数说明     longitude2      第二个点的经度
// 返回参数     double          返回方位角（0至360）
// 使用示例     get_two_points_azimuth(latitude1_1, longitude1, latitude2, longitude2);
// 备注信息
//-------------------------------------------------------------------------------------------------------------------
double get_two_points_azimuth (double latitude1, double longitude1, double latitude2, double longitude2)
{
    latitude1 = ANGLE_TO_RAD(latitude1);
    latitude2 = ANGLE_TO_RAD(latitude2);
    longitude1 = ANGLE_TO_RAD(longitude1);
    longitude2 = ANGLE_TO_RAD(longitude2);

    double x = sin(longitude2 - longitude1) * cos(latitude2);
    double y = cos(latitude1) * sin(latitude2) - sin(latitude1) * cos(latitude2) * cos(longitude2 - longitude1);
    double angle = RAD_TO_ANGLE(atan2(x, y));
    return ((angle > 0) ? angle : (angle + 360));
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     解析GPS数据
// 参数说明     void
// 返回参数     uint8           0-解析成功 1-解析失败 可能数据包错误
// 使用示例     gps_data_parse();
// 备注信息
//-------------------------------------------------------------------------------------------------------------------
uint8 gps_data_parse (void)
{
    uint8 return_state = 0;
    uint8 check_buffer[5] = {'0', 'x', 0x00, 0x00, 0x00};
    uint8 bbc_xor_origin = 0;
    uint8 bbc_xor_calculation = 0;
    uint32 data_len;

    do
    {
        if(GPS_STATE_RECEIVED == gps_rmc_state)
        {
            gps_rmc_state = GPS_STATE_PARSING;
            strncpy((char *)&check_buffer[2], strchr((const char *)gps_rmc_buffer, '*')+1, 2);
            bbc_xor_origin = (uint8)func_str_to_hex((char *)check_buffer);
            for(bbc_xor_calculation = gps_rmc_buffer[1], data_len = 2; gps_rmc_buffer[data_len] != '*'; data_len ++)
            {
                bbc_xor_calculation ^= gps_rmc_buffer[data_len];
            }
            if(bbc_xor_calculation != bbc_xor_origin)
            {
                // 数据校验失败
                return_state = 1;
                break;
            }

            gps_gnrmc_parse((char *)gps_rmc_buffer, &gps_tau1201);
        }
        gps_rmc_state = GPS_STATE_RECEIVING;

        if(GPS_STATE_RECEIVED == gps_gga_state)
        {
            gps_gga_state = GPS_STATE_PARSING;
            strncpy((char *)&check_buffer[2], strchr((const char *)gps_gga_buffer, '*')+1, 2);
            bbc_xor_origin = (uint8)func_str_to_hex((char *)check_buffer);

            for(bbc_xor_calculation = gps_gga_buffer[1], data_len = 2; gps_gga_buffer[data_len] != '*'; data_len ++)
            {
                bbc_xor_calculation ^= gps_gga_buffer[data_len];
            }
            if(bbc_xor_calculation != bbc_xor_origin)
            {
                // 数据校验失败
                return_state = 1;
                break;
            }

            gps_gngga_parse((char *)gps_gga_buffer, &gps_tau1201);
        }
        gps_gga_state = GPS_STATE_RECEIVING;

    }while(0);
    return return_state;
}


//-------------------------------------------------------------------------------------------------------------------
// 函数简介     GPS串口回调函数
// 参数说明     void
// 返回参数     void
// 使用示例     gps_uart_callback();
// 备注信息     此函数需要在串口接收中断内进行调用
//-------------------------------------------------------------------------------------------------------------------
void gps_uart_callback (void)
{
    uint8 temp_gps[6];
    uint32 temp_length;

    if(gps_tau1201_state)
    {
        uint8 dat;
        while(uart_query_byte(GPS_TAU1201_UART, &dat))
        {
            fifo_write_buffer(&gps_tau1201_receiver_fifo, &dat, 1);
        }

        if('\n' == dat)
        {
            // 读取前6个数据 用于判断语句类型
            temp_length = 6;
            fifo_read_buffer(&gps_tau1201_receiver_fifo, temp_gps, &temp_length, FIFO_READ_ONLY);

            // 根据不同类型将数据拷贝到不同的缓冲区
            if(0 == strncmp((char *)&temp_gps[3], "RMC", 3))
            {
                // 如果没有在解析数据则更新缓冲区的数据
                if(GPS_STATE_PARSING != gps_rmc_state)
                {
                    gps_rmc_state = GPS_STATE_RECEIVED;
                    temp_length = fifo_used(&gps_tau1201_receiver_fifo);
                    fifo_read_buffer(&gps_tau1201_receiver_fifo, gps_rmc_buffer, &temp_length, FIFO_READ_AND_CLEAN);
                }
            }
            else if(0 == strncmp((char *)&temp_gps[3], "GGA", 3))
            {
                // 如果没有在解析数据则更新缓冲区的数据
                if(GPS_STATE_PARSING != gps_gga_state)
                {
                    gps_gga_state = GPS_STATE_RECEIVED;
                    temp_length = fifo_used(&gps_tau1201_receiver_fifo);
                    fifo_read_buffer(&gps_tau1201_receiver_fifo, gps_gga_buffer, &temp_length, FIFO_READ_AND_CLEAN);
                }
            }

            // 统一将FIFO清空
            fifo_clear(&gps_tau1201_receiver_fifo);

            gps_tau1201_flag = 1;
        }
    }
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     GPS初始化
// 参数说明     void
// 返回参数     void
// 使用示例     gps_init();
// 备注信息
//-------------------------------------------------------------------------------------------------------------------
uint8 gps_init (void)
{
    const uint8 set_rate[]      = {0xF1, 0xD9, 0x06, 0x42, 0x14, 0x00, 0x00, 0x0A, 0x05, 0x00, 0x64, 0x00, 0x00, 0x00, 0x60, 0xEA, 0x00, 0x00, 0xD0, 0x07, 0x00, 0x00, 0xC8, 0x00, 0x00, 0x00, 0xB8, 0xED};
    const uint8 open_gga[]      = {0xF1, 0xD9, 0x06, 0x01, 0x03, 0x00, 0xF0, 0x00, 0x01, 0xFB, 0x10};
    const uint8 open_rmc[]      = {0xF1, 0xD9, 0x06, 0x01, 0x03, 0x00, 0xF0, 0x05, 0x01, 0x00, 0x1A};

    const uint8 close_gll[]     = {0xF1, 0xD9, 0x06, 0x01, 0x03, 0x00, 0xF0, 0x01, 0x00, 0xFB, 0x11};
    const uint8 close_gsa[]     = {0xF1, 0xD9, 0x06, 0x01, 0x03, 0x00, 0xF0, 0x02, 0x00, 0xFC, 0x13};
    const uint8 close_grs[]     = {0xF1, 0xD9, 0x06, 0x01, 0x03, 0x00, 0xF0, 0x03, 0x00, 0xFD, 0x15};
    const uint8 close_gsv[]     = {0xF1, 0xD9, 0x06, 0x01, 0x03, 0x00, 0xF0, 0x04, 0x00, 0xFE, 0x17};
    const uint8 close_vtg[]     = {0xF1, 0xD9, 0x06, 0x01, 0x03, 0x00, 0xF0, 0x06, 0x00, 0x00, 0x1B};
    const uint8 close_zda[]     = {0xF1, 0xD9, 0x06, 0x01, 0x03, 0x00, 0xF0, 0x07, 0x00, 0x01, 0x1D};
    const uint8 close_gst[]     = {0xF1, 0xD9, 0x06, 0x01, 0x03, 0x00, 0xF0, 0x08, 0x00, 0x02, 0x1F};
    const uint8 close_txt[]     = {0xF1, 0xD9, 0x06, 0x01, 0x03, 0x00, 0xF0, 0x40, 0x00, 0x3A, 0x8F};
    const uint8 close_txt_ant[] = {0xF1, 0xD9, 0x06, 0x01, 0x03, 0x00, 0xF0, 0x20, 0x00, 0x1A, 0x4F};

    fifo_init(&gps_tau1201_receiver_fifo, FIFO_DATA_8BIT, gps_tau1201_receiver_buffer, GPS_TAU1201_BUFFER_SIZE);
    system_delay_ms(500);                                                           // 等待GPS启动后开始初始化
    uart_init(GPS_TAU1201_UART, 115200, GPS_TAU1201_RX, GPS_TAU1201_TX);

    set_wireless_type(GPS_TAU1201, gps_uart_callback);

    uart_write_buffer(GPS_TAU1201_UART, (uint8 *)set_rate, sizeof(set_rate));       // 设置GPS更新速率为10hz 如果不调用此语句则默认为1hz
    system_delay_ms(200);

    uart_write_buffer(GPS_TAU1201_UART, (uint8 *)open_rmc, sizeof(open_rmc));       // 开启rmc语句
    system_delay_ms(50);
    uart_write_buffer(GPS_TAU1201_UART, (uint8 *)open_gga, sizeof(open_gga));       // 开启gga语句
    system_delay_ms(50);
    uart_write_buffer(GPS_TAU1201_UART, (uint8 *)close_gll, sizeof(close_gll));
    system_delay_ms(50);
    uart_write_buffer(GPS_TAU1201_UART, (uint8 *)close_gsa, sizeof(close_gsa));
    system_delay_ms(50);
    uart_write_buffer(GPS_TAU1201_UART, (uint8 *)close_grs, sizeof(close_grs));
    system_delay_ms(50);
    uart_write_buffer(GPS_TAU1201_UART, (uint8 *)close_gsv, sizeof(close_gsv));
    system_delay_ms(50);
    uart_write_buffer(GPS_TAU1201_UART, (uint8 *)close_vtg, sizeof(close_vtg));
    system_delay_ms(50);
    uart_write_buffer(GPS_TAU1201_UART, (uint8 *)close_zda, sizeof(close_zda));
    system_delay_ms(50);
    uart_write_buffer(GPS_TAU1201_UART, (uint8 *)close_gst, sizeof(close_gst));
    system_delay_ms(50);
    uart_write_buffer(GPS_TAU1201_UART, (uint8 *)close_txt, sizeof(close_txt));
    system_delay_ms(50);
    uart_write_buffer(GPS_TAU1201_UART, (uint8 *)close_txt_ant, sizeof(close_txt_ant));
    system_delay_ms(50);

    gps_tau1201_state = 1;
    uart_rx_interrupt(GPS_TAU1201_UART, 1);

    return 0;
}
