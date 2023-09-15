#include "bsp_pid.h"
#include "capture.h"
#include "protocol.h"
#include "common.h"

/*定义位置PID与速度PID结构体型的全局变量*/
PID pid_location;
PID pid_speed;

/**
  * @brief  PID参数初始化
  *	@note 	无
  * @retval 无
  */
void PID_param_init()
{
	/* 位置相关初始化参数 */
	pid_location.target_val = TOTAL_RESOLUTION*10;				
	pid_location.output_val = 0.0;
	pid_location.err = 0.0;
	pid_location.err_last = 0.0;
	pid_location.integral = 0.0;

	pid_location.Kp = 0.05;
	pid_location.Ki = 0;
	pid_location.Kd = 0;

	/* 速度相关初始化参数 */
	pid_speed.target_val=10.0;				
	pid_speed.output_val=0.0;
	pid_speed.err=0.0;
	pid_speed.err_last=0.0;
	pid_speed.integral=0.0;

	pid_speed.Kp = 80.0;
	pid_speed.Ki = 2.0;
	pid_speed.Kd = 100.0;

#if defined(PID_ASSISTANT_EN)
	{
		float pid_temp[3] = {0};
		
		/*给通道1发送位置PID值*/
		pid_temp[0] = pid_location.Kp;
		pid_temp[1] = pid_location.Ki;
		pid_temp[2] = pid_location.Kd;
		set_computer_value(SEND_P_I_D_CMD, CURVES_CH1, pid_temp, 3);     
		
		/*给通道2发送速度PID值*/
		pid_temp[0] = pid_speed.Kp;
		pid_temp[1] = pid_speed.Ki;
		pid_temp[2] = pid_speed.Kd;
		set_computer_value(SEND_P_I_D_CMD, CURVES_CH2, pid_temp, 3);  
	}	
#endif
}

extern float det_plus;
/**
  * @brief  设置目标值
  * @param  val		目标值
  *	@note 	无
  * @retval 无
  */
void set_pid_target(PID *pid, float temp_val)
{  
	pid->target_val = temp_val;    // 设置当前的目标值
}

/**
  * @brief  获取目标值
  * @param  无
  *	@note 	无
  * @retval 目标值
  */
float get_pid_target(PID *pid)
{
  return pid->target_val;    // 获取当前的目标值
}


/**
  * @brief  设置比例、积分、微分系数
  * @param  p：比例系数 P
  * @param  i：积分系数 i
  * @param  d：微分系数 d
  *	@note 	无
  * @retval 无
  */
void set_p_i_d(PID *pid, float p, float i, float d)
{
	pid->Kp = p;    // 设置比例系数 P
	pid->Ki = i;    // 设置积分系数 I
	pid->Kd = d;    // 设置微分系数 D
}




/**
  * @brief  位置PID算法实现
  * @param  actual_val:实际值
  *	@note 	无
  * @retval 通过PID计算后的输出
  */
#define LOC_DEAD_ZONE 60 /*位置环死区*/
#define LOC_INTEGRAL_START_ERR 200 /*积分分离时对应的误差范围*/
#define LOC_INTEGRAL_MAX_VAL 800   /*积分范围限定，防止积分饱和*/
float location_pid_realize(PID *pid, float actual_val)
{
	/*计算目标值与实际值的误差*/
	pid->err = pid->target_val - actual_val;

	/* 设定闭环死区 */
	if((pid->err >= -LOC_DEAD_ZONE) && (pid->err <= LOC_DEAD_ZONE))
	{
		pid->err = 0;
		pid->integral = 0;
		pid->err_last = 0;
	}

	/*积分项，积分分离，偏差较大时去掉积分作用*/
	if(pid->err > -LOC_INTEGRAL_START_ERR && pid->err < LOC_INTEGRAL_START_ERR)
	{
		pid->integral += pid->err;  
        /*积分范围限定，防止积分饱和*/
		if(pid->integral > LOC_INTEGRAL_MAX_VAL)
		{
			pid->integral = LOC_INTEGRAL_MAX_VAL;
		}
		else if(pid->integral < -LOC_INTEGRAL_MAX_VAL)
		{
			pid->integral = -LOC_INTEGRAL_MAX_VAL;
		}
	}	

	/*PID算法实现*/
	pid->output_val = pid->Kp * pid->err +
	                  pid->Ki * pid->integral +
	                  pid->Kd * (pid->err - pid->err_last);

	/*误差传递*/
	pid->err_last = pid->err;

	/*返回当前实际值*/
	return pid->output_val;
}

/**
  * @brief  速度PID算法实现
  * @param  actual_val:实际值
  *	@note 	无
  * @retval 通过PID计算后的输出
  */
#define SPE_DEAD_ZONE 5.0f /*速度环死区*/
#define SPE_INTEGRAL_START_ERR 100 /*积分分离时对应的误差范围*/
#define SPE_INTEGRAL_MAX_VAL 260   /*积分范围限定，防止积分饱和*/
float speed_pid_realize(PID *pid, float actual_val)
{
	/*计算目标值与实际值的误差*/
	pid->err = pid->target_val - actual_val;

	/* 设定闭环死区 */
	if( (pid->err>-SPE_DEAD_ZONE) && (pid->err<SPE_DEAD_ZONE ) )
	{
		pid->err = 0;
		pid->integral = 0;
		pid->err_last = 0;
	}

#if 0
	/*积分项*/
	pid->integral += pid->err;
#else	
	/*积分项，积分分离，偏差较大时去掉积分作用*/
	if(pid->err > -SPE_INTEGRAL_START_ERR && pid->err < SPE_INTEGRAL_START_ERR)
	{
		pid->integral += pid->err;  
        /*积分范围限定，防止积分饱和*/
		if(pid->integral > SPE_INTEGRAL_MAX_VAL)
		{
			pid->integral = SPE_INTEGRAL_MAX_VAL;
		}
		else if(pid->integral < -SPE_INTEGRAL_MAX_VAL)
		{
			pid->integral = -SPE_INTEGRAL_MAX_VAL;
		}
	}	
#endif
	
	/*PID算法实现*/
	pid->output_val = pid->Kp * pid->err +
	                  pid->Ki * pid->integral +
	                  pid->Kd *(pid->err - pid->err_last);

	/*误差传递*/
	pid->err_last = pid->err;

	/*返回当前实际值*/
	return pid->output_val;
}


