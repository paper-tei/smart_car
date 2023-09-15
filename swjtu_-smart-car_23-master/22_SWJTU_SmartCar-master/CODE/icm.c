/*
 * ICM.c
 *
 *  Created on: 2022Äê2ÔÂ18ÈÕ
 *      Author: paper
 */
#include "icm.h"
float icm_yawspeed=0;
float er_speed=0;
float T2p =  0.025   ,  T2d   =  0.000;
int16 icm_Error=0;
void icm_int(void)
{
    icm20602_init_spi();
}
void D2M_Control(void)
{
  icm_yawspeed = - 0.06*icm_gyro_x;
  float error_P = icm_yawspeed - yawspeed , error_D = icm_yawspeed - yawspeed - icm_Error;
  icm_Error= icm_yawspeed - yawspeed;
  er_speed=T2p*error_P+T2d*error_D;

  if(er_speed>50) er_speed=50;
  if(er_speed<-50) er_speed=-50;

  yawspeed = yawspeed -er_speed;
}

