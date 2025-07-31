

#include "system/delay/delay.h"
#include "Servo.h"


unsigned int Servo1_Angle = 0;//舵机1PWM值(原angle变量)
unsigned int Servo2_Angle = 0;//舵机2PWM值(原angle变量)

/******************************************************************
   配置占空比 范围 0 ~ 400
   直接设置PWM占空比值，无需角度映射
******************************************************************/

/******************************************************************
 * 函 数 名 称：Set_Servo1_Angle
 * 函 数 说 明：设置舵机1的PWM值
 * 函 数 形 参：angle=要设置的PWM值，范围0到400
 * 函 数 返 回：无

******************************************************************/
void Set_Servo1_Angle(unsigned int angle)
{
    // 限制PWM值在设定范围内
    if(angle > 400)
    {
        angle = 400;
    }

    Servo1_Angle = angle;

    // 直接设置PWM占空比
    DL_TimerG_setCaptureCompareValue(PWM_SG90_INST, angle, GPIO_PWM_SG90_C0_IDX);
}

/******************************************************************
 * 函 数 名 称：Set_Servo2_Angle
 * 函 数 说 明：设置舵机2的PWM值
 * 函 数 形 参：angle=要设置的PWM值，范围0到400
 * 函 数 返 回：无

******************************************************************/
void Set_Servo2_Angle(unsigned int angle)
{
    // 限制PWM值在设定范围内
    if(angle > 400)
    {
        angle = 400;
    }

    Servo2_Angle = angle;

    // 直接设置PWM占空比
    DL_TimerG_setCaptureCompareValue(PWM_SG90_INST, angle, GPIO_PWM_SG90_C1_IDX); // 使用第二个通道C1
}

/******************************************************************
 * 函 数 名 称：Get_Servo1_Angle
 * 函 数 说 明：读取舵机1当前PWM值
 * 函 数 形 参：无
 * 函 数 返 回：当前PWM值
******************************************************************/
unsigned int Get_Servo1_Angle(void)
{
    return Servo1_Angle;
}

/******************************************************************
 * 函 数 名 称：Get_Servo2_Angle
 * 函 数 说 明：读取舵机2当前PWM值
 * 函 数 形 参：无
 * 函 数 返 回：当前PWM值
******************************************************************/
unsigned int Get_Servo2_Angle(void)
{
    return Servo2_Angle;
}

/******************************************************************
 * 函 数 名 称：Set_Servo1_MidAngle
 * 函 数 说 明：设置舵机1到中间位置
 * 函 数 形 参：无
 * 函 数 返 回：无
******************************************************************/
void Set_Servo1_MidAngle(void)
{
    Set_Servo1_Angle(SERVO1_MID_ANGLE);
}

/******************************************************************
 * 函 数 名 称：Set_Servo2_MidAngle
 * 函 数 说 明：设置舵机2到中间位置
 * 函 数 形 参：无
 * 函 数 返 回：无
******************************************************************/
void Set_Servo2_MidAngle(void)
{
    Set_Servo2_Angle(SERVO2_MID_ANGLE);
}

/******************************************************************
 * 函 数 名 称：Set_Servo_Angle
 * 函 数 说 明：兼容性函数，设置舵机1的PWM值
 * 函 数 形 参：angle=要设置的PWM值，范围0到400
 * 函 数 返 回：无
******************************************************************/
void Set_Servo_Angle(unsigned int angle)
{
    Set_Servo1_Angle(angle);
}

/******************************************************************
 * 函 数 名 称：Get_Servo_Angle
 * 函 数 说 明：兼容性函数，读取舵机1当前PWM值
 * 函 数 形 参：无
 * 函 数 返 回：当前PWM值
******************************************************************/
unsigned int Get_Servo_Angle(void)
{
    return Get_Servo1_Angle();
}