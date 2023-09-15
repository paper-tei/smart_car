/*********************************************************************************************************************
 * COPYRIGHT NOTICE
 * Copyright (c) 2020,逐飞科技
 * All rights reserved.
 * 技术讨论QQ群：三群：824575535
 *
 * 以下所有内容版权均属逐飞科技所有，未经允许不得用于商业用途，
 * 欢迎各位使用并传播本程序，修改内容时必须保留逐飞科技的版权声明。
 *
 * @file       		headfile
 * @company	   		成都逐飞科技有限公司
 * @author     		逐飞科技(QQ3184284598)
 * @version    		查看doc内version文件 版本说明
 * @Software 		ADS v1.2.2
 * @Target core		TC364DP
 * @Taobao   		https://seekfree.taobao.com/
 * @date       		2020-11-23
 ********************************************************************************************************************/
 
#ifndef _headfile_h
#define _headfile_h




#include "SEEKFREE_PRINTF.h"

#include "zf_assert.h"
#include "stdio.h"
#include "math.h"
//官方头文件
#include "ifxAsclin_reg.h"
#include "SysSe/Bsp/Bsp.h"
#include "IfxCcu6_Timer.h"
#include "IfxScuEru.h"

//------逐飞科技单片机外设驱动头文件
#include "zf_gpio.h"
#include "zf_gtm_pwm.h"
#include "zf_uart.h"
#include "zf_ccu6_pit.h"
#include "zf_stm_systick.h"
#include "zf_spi.h"
#include "zf_eru.h"
#include "zf_eru_dma.h"
#include "zf_vadc.h"
#include "zf_gpt12.h"
#include "zf_eeprom.h"

//------逐飞科技产品驱动头文件
#include "SEEKFREE_FONT.h"
#include "SEEKFREE_FUN.h"
#include "SEEKFREE_IIC.h"
#include "SEEKFREE_OLED.h"
#include "SEEKFREE_VIRSCO.h"
#include "SEEKFREE_MT9V03X.h"
#include "SEEKFREE_ICM20602.h"


#include "SEEKFREE_WIRELESS.h"
#include "SEEKFREE_IPS200_PARALLEL8.h"
#include "SEEKFREE_BLUETOOTH_CH9141.h"

//个人头文件
#include "image.h"
#include "key.h"
#include "init.h"
#include "pid.h"
#include "motor.h"
#include "time.h"
#include "error.h"
#include "Three_Forks_Road.h"
#include "ring.h"
#include "ZW_Tools.h"
#include "fuzzy.h"
#include "filter.h"
#include "In_out_bound.h"
#include "icm.h"
#include "cross.h"
#include "tof.h"
#include "home.h"
#include "back_loop.h"
extern float Speed_Left,Speed_Left_set;
extern float Speed_Right,Speed_Right_set;

extern uint16 duty;
#endif

