#include "stepper.h"

#define stepping_pul(x)		PA_out(1,x)
#define stepping_dir(x)		PB_out(8,x)
#define stepping_ena(x)		PA_out(5,x)


/**
  * @brief  软件PWM
	* @param :angle ： 步进电机脉冲数		
        speed :  步进电机旋转速度（实际上就是修改了PWM的频率）
  * @retval 无
  */
void stepping_PWM(uint32_t angle,uint32_t speed)
{
	for(int i = 0 ; i < angle ; i++)
	{
		stepping_pul(1);
		delay_1us(speed);
		stepping_pul(0);
		delay_1us(speed);
	}
}
/**
  * @brief  方向反转
  * @param  无
  * @retval 无
  */
void stepping_reversal(void)
{
	static _Bool stepping_reversal_i;
	if(stepping_reversal_i)
			 {stepping_dir(0);stepping_reversal_i = 0;}
	else {stepping_dir(1);stepping_reversal_i = 1;}
}


void sp_pwm(int angle,uint32_t speed)
{
	if(angle > 0)
	{
		stepping_dir(1);
		stepping_PWM(angle,speed);
	}
	else 
	{
		stepping_dir(0);
		stepping_PWM(-angle,speed);
	}
}
/**
  * @brief  中断反转
  * @param  无
  * @retval 无
  */
void stepping_End(void)
{
	static _Bool stepping_End_i;
	if(stepping_End_i)
			{stepping_ena(1);stepping_End_i = 0;}
	else {stepping_ena(0);stepping_End_i = 1;}
}

/**
  * @brief  每一度的脉冲
  * @param  角度
  * @retval 脉冲值
 */

void stepping_return_agle(int16_t angle,uint16_t speed)
{
	int returnagle=0;
	returnagle=((1600.0/360.0)*angle);//1600/360=每一度脉冲*角度
	
	if(returnagle > 0)
	{
		stepping_dir(1);
		stepping_PWM(returnagle,speed);
	}
	else 
	{
		stepping_dir(0);
		stepping_PWM(-returnagle,speed);
	}
}
