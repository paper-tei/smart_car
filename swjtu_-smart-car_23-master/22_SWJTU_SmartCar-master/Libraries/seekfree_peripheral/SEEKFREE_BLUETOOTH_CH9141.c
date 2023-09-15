/*********************************************************************************************************************
 * COPYRIGHT NOTICE
 * Copyright (c) 2021,逐飞科技
 * All rights reserved.
 * 技术讨论QQ群：一群：179029047(已满)  二群：244861897
 *
 * 以下所有内容版权均属逐飞科技所有，未经允许不得用于商业用途，
 * 欢迎各位使用并传播本程序，修改内容时必须保留逐飞科技的版权声明。
 *
 * @file       		逐飞科技蓝牙转串口模块
 * @company	   		成都逐飞科技有限公司
 * @author     		逐飞科技(QQ3184284598)
 * @version    		查看doc内version文件 版本说明
 * @Software 		IAR 8.3 or MDK 5.33
 * @Taobao   		https://seekfree.taobao.com/
 * @date       		2021-08-27
 * @note		
					接线定义：
					------------------------------------ 
					    蓝牙转串口      单片机                        
    					RX              查看SEEKFREE_BLUETOOTH_CH9141.h文件中的BLUETOOTH_CH9141_UART_TX宏定义
    					TX              查看SEEKFREE_BLUETOOTH_CH9141.h文件中的BLUETOOTH_CH9141_UART_RX宏定义
    					RTS             查看SEEKFREE_BLUETOOTH_CH9141.h文件中的BLUETOOTH_CH9141_RTS_PIN宏定义
                        CTS             悬空
    					CMD             悬空或者上拉
					------------------------------------ 
 ********************************************************************************************************************/
#include "zf_stm_systick.h"
#include "zf_gpio.h"
#include "zf_uart.h"
#include "zf_assert.h"
#include "SEEKFREE_BLUETOOTH_CH9141.h"

uint8 uart_flag;
uint8 uart_data;

vuint8 at_mode = 0;         //0:蓝牙透传模式 1:AT模式 2:模块复位中
vuint8 at_mode_num;         //at模式时用于指示数据接收的数量
vuint8 at_mode_data[30];    //接收at命令的缓存
vuint8 at_mode_cmd_flag;    //OK应答命令接收成功的标志位

uint8 mac_address[17];      //本机mac地址


uint8   bluetooth_ch9141_rx_buffer;


void bluetooth_ch9141_check_response(void);

//-------------------------------------------------------------------------------------------------------------------
//  @brief      蓝牙转串口模块 串口中断回调函数
//  @param      NULL
//  @return     void					
//  @since      v1.0
//  Sample usage:	
//  @note       该函数在ISR文件 串口8中断程序被调用
//-------------------------------------------------------------------------------------------------------------------
void bluetooth_ch9141_uart_callback()
{
    while(uart_query(BLUETOOTH_CH9141_UART, &bluetooth_ch9141_rx_buffer))
    {
        if(1 == at_mode)
        {
            //进入AT模式 接收应答信号 此处if语句内代码用户不要改动
            at_mode_data[at_mode_num++] = bluetooth_ch9141_rx_buffer;
            bluetooth_ch9141_check_response();
        }
        else if(2 == at_mode)
        {
            //模块正在复位中 此处if语句内代码用户不要改动
            at_mode_num++;
        }
        else
        {
            //透传模式 用户在此处接收配对的蓝牙发送过来的额数据
            //接到一个字节后单片机将会进入此处，通过在此处读取bluetooth_ch9141_rx_buffer可以取走数据

            // 读取无线串口的数据 并且置位接收标志
            uart_flag = 1;
            uart_data = bluetooth_ch9141_rx_buffer;
        }
        
    }
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      蓝牙转串口模块检查OK应答信号
//  @param      NULL
//  @return     void					
//  @since      v1.0
//  Sample usage:	
//  @note       用户无需关心
//-------------------------------------------------------------------------------------------------------------------
void bluetooth_ch9141_check_response(void)
{
    if(4 <= at_mode_num)
    {
        if(0 == strncmp("OK\r\n", (int8 *)&at_mode_data[at_mode_num-4], 4))
        {
            at_mode_cmd_flag = 1;
        }
    }
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      蓝牙转串口模块发送一个没有参数的命令并等待应答信号
//  @param      *str    需要发送的命令 完整字符串
//  @return     void					
//  @since      v1.0
//  Sample usage:	
//  @note       用户无需关心
//-------------------------------------------------------------------------------------------------------------------
void bluetooth_ch9141_send_at_command(const int8 *str)
{
    at_mode_num = 0;        //接收数量清零
    uart_putstr(BLUETOOTH_CH9141_UART, str);
    uart_putstr(BLUETOOTH_CH9141_UART, "\r\n");
    
    //等待收到应答信号
    while(!at_mode_cmd_flag);
    at_mode_cmd_flag = 0;
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      蓝牙转串口模块发送一个带有参数的命令并等待应答信号
//  @param      *cmd    需要发送的命令名称
//  @param      *data   需要发送的数据
//  @return     void					
//  @since      v1.0
//  Sample usage:	
//  @note       用户无需关心
//-------------------------------------------------------------------------------------------------------------------
void bluetooth_ch9141_send_at_command_parameter(const int8 *cmd, const int8 *data)
{
    at_mode_num = 0;        //接收数量清零
    uart_putstr(BLUETOOTH_CH9141_UART, "AT+");
    uart_putstr(BLUETOOTH_CH9141_UART, cmd);
    uart_putstr(BLUETOOTH_CH9141_UART, "=");
    uart_putstr(BLUETOOTH_CH9141_UART, data);
    uart_putstr(BLUETOOTH_CH9141_UART, "\r\n");
    
    //等待收到应答信号
    while(!at_mode_cmd_flag);
    at_mode_cmd_flag = 0;
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      蓝牙转串口模块进入AT模式
//  @param      NULL
//  @return     void					
//  @since      v1.0
//  Sample usage:	
//  @note       
//-------------------------------------------------------------------------------------------------------------------
void bluetooth_ch9141_enter_at_mode(void)
{
    systick_delay_ms(STM0, 550);  //发送进入AT模式的命令前需要保证模块在550ms内没有接收过任何数据
    at_mode = 1;            //进入AT模式
    bluetooth_ch9141_send_at_command("AT...");
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      蓝牙转串口模块退出AT模式
//  @param      NULL
//  @return     void					
//  @since      v1.0
//  Sample usage:	
//  @note       
//-------------------------------------------------------------------------------------------------------------------
void bluetooth_ch9141_exit_at_mode(void)
{
    bluetooth_ch9141_send_at_command("AT+EXIT");
    at_mode = 0;            //进入透传模式
    systick_delay_ms(STM0, 300);  //等待成功进入AT模式
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      蓝牙转串口模块复位
//  @param      NULL
//  @return     void					
//  @since      v1.0
//  Sample usage:	
//  @note       
//-------------------------------------------------------------------------------------------------------------------
void bluetooth_ch9141_reset(void)
{
    bluetooth_ch9141_send_at_command("AT+RESET");
    at_mode = 2;            //进入重启成功检测
    at_mode_num = 0;
    while(7 > at_mode_num); //等待蓝牙模块完成复位
    at_mode = 0;            //复位之后模块自动进入透传模式
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      蓝牙转串口模块获取本机MAC地址
//  @param      NULL
//  @return     void					
//  @since      v1.0
//  Sample usage:	
//  @note       调用此函数需要先调用bluetooth_ch9141_enter_at_mode或者拉低CMD引脚 进入AT模式
//              需要特别注意bluetooth_ch9141_enter_at_mode函数内部有500ms的延时
//-------------------------------------------------------------------------------------------------------------------
void bluetooth_ch9141_get_mac_address(void)
{
    bluetooth_ch9141_send_at_command("AT+MAC?");
    
    //mac地址为小段格式，mac_address[0]保存的是mac地址最低位
    memcpy(mac_address, (uint8 *)at_mode_data, 17);
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      蓝牙转串口模块设置发送功率
//  @param      tx_power    设置发送功率，可设置选项查看CH9141_TX_POWEER_enum枚举成员
//  @return     void					
//  @since      v1.0
//  Sample usage:	
//  @note       
//-------------------------------------------------------------------------------------------------------------------
void bluetooth_ch9141_set_tx_power(CH9141_TX_POWEER_enum tx_power)
{
    int8 tx_power_data;
    
    tx_power_data = (uint8)tx_power + '0';
    bluetooth_ch9141_send_at_command_parameter("TPL", &tx_power_data);
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      蓝牙转串口模块设置模式
//  @param      mode    模式设置，可设置选项查看CH9141_MODE_enum枚举成员
//  @return     void					
//  @since      v1.0
//  Sample usage:	
//  @note       
//-------------------------------------------------------------------------------------------------------------------
void bluetooth_ch9141_set_mode(CH9141_MODE_enum mode)
{
    int8 mode_data;
    
    mode_data = (uint8)mode + '0';
    bluetooth_ch9141_send_at_command_parameter("BLEMODE", &mode_data);
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      蓝牙转串口模块获取状态
//  @param      mode    模式设置，可设置选项查看CH9141_MODE_enum枚举成员
//  @return     CH9141_STATUS_enum  返回状态信息
//  @since      v1.0
//  Sample usage:	
//  @note       
//-------------------------------------------------------------------------------------------------------------------
CH9141_STATUS_enum bluetooth_ch9141_get_status(CH9141_MODE_enum mode)
{
    CH9141_STATUS_enum ch9141_status;
    int8 mode_data;
    
    mode_data = (uint8)mode + '0';
    bluetooth_ch9141_send_at_command_parameter("BLEMODE", &mode_data);
    
    bluetooth_ch9141_send_at_command("AT+BLESTA?");
    
    ch9141_status = (at_mode_data[0] - '0') * 10 + (at_mode_data[1] - '0');
    if(SLAVE_MODE == mode)
    {
        ch9141_status += SLAVE_NO_INIT;
    }
    
    return ch9141_status;
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      蓝牙转串口模块设置设备名称
//  @param      *str    蓝牙名称
//  @return     void					
//  @since      v1.0
//  Sample usage:	
//  @note       名称长度不能超过18个字符 且只能为英文与数字
//-------------------------------------------------------------------------------------------------------------------
void bluetooth_ch9141_set_name(const int8 *str)
{
    bluetooth_ch9141_send_at_command_parameter("NAME", str);
    bluetooth_ch9141_send_at_command_parameter("PNAME", str);
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      蓝牙转串口模块密码设置
//  @param      enable      使能密码 0：不使用密码，1：使用密码才能连接本设备
//  @param      *password   密码的字符串 必须为6个字符
//  @return     void					
//  @since      v1.0
//  Sample usage:	
//  @note       
//-------------------------------------------------------------------------------------------------------------------
void bluetooth_ch9141_set_password(uint8 enable, const int8 *password)
{
    if(0 == enable)
    {
        //关闭密码
        bluetooth_ch9141_send_at_command_parameter("PASEN", "OFF");
    }
    else
    {
        //设置密码并使能
        bluetooth_ch9141_send_at_command_parameter("PASEN", "ON");
        bluetooth_ch9141_send_at_command_parameter("PASS", password);
    }
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      蓝牙转串口模块指定MAC地址并立即进行连接
//  @param      *mac_and_password      需要连接的设备mac地址与密码
//  @return     void					
//  @since      v1.0
//  Sample usage:	
//  @note       bluetooth_ch9141_connect("58:B7:33:E4:C2:84,000000");
//              58:B7:33:E4:C2:84为mac地址 ,为分隔符 000000为从机蓝牙密码
//              ===================特别注意==================
//              如果使用手机查看蓝牙的mac地址，则使用本函数连接的时候请将mac倒置一下
//              例如手机查看到的mac地址为61:62:63:64:65:66，则使用本函数连接的时候应该写
//              bluetooth_ch9141_connect("66:65:64:63:62:61,000000");
//-------------------------------------------------------------------------------------------------------------------
void bluetooth_ch9141_connect(const int8 *mac_and_password)
{
    bluetooth_ch9141_send_at_command_parameter("CONN", mac_and_password);
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      蓝牙转串口模块默认连接参数设置（设置好后，每次开机蓝牙会自动链接这个设备）
//  @param      *mac_and_password      需要连接的设备mac地址与密码
//  @return     void					
//  @since      v1.0
//  Sample usage:	
//  @note       bluetooth_ch9141_default_connect("58:B7:33:E4:C2:84,000000");
//              58:B7:33:E4:C2:84为mac地址 ,为分隔符 000000为从机蓝牙密码
//              ===================特别注意==================
//              如果使用手机查看CH9141的mac地址，将CH9141设置为从机，使用手机连接
//              则使用本函数连接的时候请将mac倒置一下
//              例如手机查看到的mac地址为61:62:63:64:65:67，则使用本函数连接的时候应该写
//              bluetooth_ch9141_default_connect("67:65:64:63:62:61,000000");
//-------------------------------------------------------------------------------------------------------------------
void bluetooth_ch9141_default_connect(const int8 *mac_and_password)
{
    bluetooth_ch9141_send_at_command_parameter("CONADD", mac_and_password);
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      蓝牙转串口模块获取rssi(信号强度)
//  @param      void      
//  @return     int8    返回信号强度0到-127
//  @since      v1.0
//  Sample usage:	
//  @note       调用此函数需要先调用bluetooth_ch9141_enter_at_mode或者拉低CMD引脚 进入AT模式
//              需要特别注意bluetooth_ch9141_enter_at_mode函数内部有500ms的延时
//-------------------------------------------------------------------------------------------------------------------
int16 bluetooth_ch9141_get_rssi(void)
{
    uint8 i;
    size_t length;
    int16 rssi;
    bluetooth_ch9141_send_at_command_parameter("RSSI", "ON,0");
    length = strlen((int8 *)at_mode_data);
    length -= 12;//计算RSSI 有多少位
    
    rssi = 0;
    for(i=0; i<length; i++)
    {
        rssi = rssi*10 + (at_mode_data[0] - '0');
    }
    
    return -rssi;
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      无线转串口模块 发送函数
//  @param      buff        需要发送的数据地址
//  @param      len         发送长度
//  @return     uint32      剩余未发送的字节数   
//  @since      v1.0
//  Sample usage:	
//  @note       
//-------------------------------------------------------------------------------------------------------------------
uint32 bluetooth_ch9141_send_buff(uint8 *buff, uint32 len)
{
    while(len)
    {
        //流控检查 RTS为高表示蓝牙模块内部缓冲已满无法继续接收数据
        
        //RTS为高处理方式一：如果检测到RTS为高则后面的数据不再继续发送，避免出现等待
        if(gpio_get(BLUETOOTH_CH9141_RTS_PIN))  
        {
            break;
        }
        
        //RTS为高处理方式二：如果检测到RTS为高则等待RTS为低之后继续发送数据
        //while(gpio_get(BLUETOOTH_CH9141_RTS_PIN));  //如果RTS为低电平，则继续发送数据
        
        //发送数据
        uart_putchar(BLUETOOTH_CH9141_UART, *buff);

        buff++;
        len--; 
    }

    return len;
}





//-------------------------------------------------------------------------------------------------------------------
//  @brief      蓝牙转串口模块初始化
//  @param      mode    蓝牙模式 MASTER_MODE(主机)或者SLAVE_MODE(从机)
//  @return     void					
//  @since      v1.0
//  Sample usage:	
//  @note       
//				如果使用手机查看蓝牙的mac地址，则使用本函数连接的时候请将mac倒置一下
//				例如手机查看到的mac地址为61:62:63:64:65:66，则使用本函数连接的时候应该写
//				bluetooth_ch9141_connect("66:65:64:63:62:61,000000");
//				58:B7:33:E4:C2:84为mac地址 ,为分隔符 000000为从机蓝牙密码
//				从机MAC地址与密码
//				int8 salve_mac_password[] = "80:90:2C:E4:C2:84,000000";
//-------------------------------------------------------------------------------------------------------------------
void bluetooth_ch9141_init(CH9141_MODE_enum mode, int8 *salve_mac_password)
{
    int8 *str = salve_mac_password;

    if(str != NULL)
    {
       while(*str)
       {
           if(*str++ == 0x20)
           {
               //不允许有空格
               ZF_ASSERT(0);
               return;
           }
       }
    }
    boolean irq_status;
    //本函数使用的波特率为115200，为蓝牙转串口模块的默认波特率，如需其他波特率请使用上位机修改模块参数
    //初始化流控引脚
    wireless_type = WIRELESS_CH9141;

    gpio_init(BLUETOOTH_CH9141_RTS_PIN, GPI, 0, PULLDOWN);
    //初始化串口
    uart_init (BLUETOOTH_CH9141_UART, BLUETOOTH_CH9141_UART_BAUD, BLUETOOTH_CH9141_UART_TX, BLUETOOTH_CH9141_UART_RX);	//初始换串口    
    irq_status = areInterruptsEnabled();
    enableInterrupts();
    
    //蓝牙分为主机与从机模式，两个蓝牙想要连接成功就必须有一个为主机，有一个为从机，所以调用初始化的时候需要合理的填写函数参数才能成功的连接
    //蓝牙分为主机与从机模式，两个蓝牙想要连接成功就必须有一个为主机，有一个为从机，所以调用初始化的时候需要合理的填写函数参数才能成功的连接
    //上电顺序：最好从机先上电，然后主机再上电

    if(MASTER_MODE == mode)
    {
        //1.将蓝牙设置为主机模式，然后连接指定mac地址的从机设备
        bluetooth_ch9141_enter_at_mode();  //进入AT模式
        bluetooth_ch9141_set_mode(mode);   //设置蓝牙模式
        bluetooth_ch9141_get_mac_address();//获取本机MAC地址
		bluetooth_ch9141_reset();                  //设置完成后需要复位，设置才会生效
		bluetooth_ch9141_enter_at_mode();          //进入AT模式

		//设置完模式之后需要复位然后再次进入AT模式才能继续设置其他参数，否则模式设置不成功
        bluetooth_ch9141_set_tx_power(TX_POWER_4DB);//设置蓝牙发送功率
        //===================特别注意==================
        //如果使用手机查看蓝牙的mac地址，则使用本函数连接的时候请将mac倒置一下
        //例如手机查看到的mac地址为61:62:63:64:65:66，则使用本函数连接的时候应该写
        //bluetooth_ch9141_connect("66:65:64:63:62:61,000000");
        //58:B7:33:E4:C2:84为mac地址 ,为分隔符 000000为从机蓝牙密码
        //从机MAC地址与密码
        //int8 salve_mac_password[] = "80:90:2C:E4:C2:84,000000";
        
        bluetooth_ch9141_default_connect(salve_mac_password);  //配置默认连接参数，即使下次不配置也会自动连接从机
        bluetooth_ch9141_connect(salve_mac_password);          //立即连接设置的从机地址
        
        //等待连接成功
        while(MASTER_CONNECTED != bluetooth_ch9141_get_status(mode));
        bluetooth_ch9141_exit_at_mode();                       //退出AT模式
    }
    else if(SLAVE_MODE == mode)
    {
        //2.蓝牙设置为从机并等待连接
        bluetooth_ch9141_enter_at_mode();  //进入AT模式
        bluetooth_ch9141_set_mode(mode);   //设置蓝牙模式
        bluetooth_ch9141_get_mac_address();//获取本机MAC地址
		bluetooth_ch9141_reset();                  //设置完成后需要复位，设置才会生效
		bluetooth_ch9141_enter_at_mode();          //进入AT模式

		//设置完模式之后需要复位然后再次进入AT模式才能继续设置其他参数，否则模式设置不成功
        bluetooth_ch9141_set_tx_power(TX_POWER_4DB);//设置蓝牙发送功率
        bluetooth_ch9141_set_name("ble");
        bluetooth_ch9141_set_password(1, "000000");  //000000为蓝牙密码可以自己修改
        bluetooth_ch9141_reset();                  //设置完成后需要复位，设置才会生效
        bluetooth_ch9141_enter_at_mode();          //进入AT模式
        //等待连接成功
        while(SLAVE_CONNECTED != bluetooth_ch9141_get_status(mode));
        bluetooth_ch9141_exit_at_mode();           //退出AT模式
    }
    
    //如果想获取无线信号强度可以按照下面的示例调用
    //bluetooth_ch9141_enter_at_mode();
    //int16 rssi = bluetooth_ch9141_get_rssi();
    
    disableInterrupts();
    restoreInterrupts(irq_status);
}


