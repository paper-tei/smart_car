/*
 * tof.c
 *
 *  Created on: 2022Äê4ÔÂ2ÈÕ
 *      Author: 24916
 */
#include "tof.h"
int distance = 0;
void Distance_measurement_Init(void)
{
    uart_init(UART_3, 115200, UART0_TX_P14_0,UART0_RX_P14_1);
}
unsigned char distance_finish=0;
static unsigned char dis_Cnt=0,dis_Len=0;
uint32 distance_buff[200];
void Distance_measurement(void)
{

    unsigned char temp=0;
    uint32 C=0;
    uint8 distance_dat;
    
    if(uart_query(UART_3, &distance_dat))
    {
        temp=distance_dat;
        if(dis_Cnt==0)
        {
            distance_buff[dis_Cnt++]=temp;
        }     
        else if((dis_Cnt==1)&(temp==0x03))
        {
            distance_buff[dis_Cnt++]=temp;
        }
            
        else if(dis_Cnt==2)
        {
            distance_buff[dis_Cnt++]=temp; dis_Len=distance_buff[2];
        }
        else if((dis_Cnt>2)&(dis_Cnt<=(dis_Len+4)))
        {
            distance_buff[dis_Cnt++]=temp;
        }
                
        if(dis_Cnt==(dis_Len+5))
        {  
            //C=ModbusCRC(distance_buff,dis_Len+3);
            //if(C==((distance_buff[dis_Len+3]<<8)|distance_buff[dis_Len+4]))
            {
                distance_finish=1;	
                dis_Cnt=0;
                dis_Len=0;
            }			
            // else 
            // {
            //     dis_Cnt=0;
            //     dis_Len=0;
            // }				
        } 
    }
   
}
void Distance_read(void)
{
    unsigned char CMD_1[8]={0x01,0x03,0x00,0x10,0x00,0x01,0x85,0xcf};
    uart_putbuff(UART_3,CMD_1,8);
	if(distance_finish==1)
	{
		distance=distance_buff[3]<<8|distance_buff[4];
	}  

}
uint32 ModbusCRC(uint32 *ptr,uint32 ucLen)//CRC??
{
	uint8 i;
	uint32 j,crc=0xffff;
	
	//i=i;
	
	for(int n=0;n<ucLen;n++)
	{
		crc=ptr[n]^crc;
		for(int i=0;i<8;i++)
		if(crc&0x01)
		{
			crc=crc>>1;
			crc=crc^0xa001;
		}
		else
		{
			crc=crc>>1;	
		}		
	}

	j=crc>>8;
	j=j|(crc<<8);
	return j;

}
