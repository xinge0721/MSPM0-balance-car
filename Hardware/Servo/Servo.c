

#include "system/delay/delay.h"
#include "Servo.h"


unsigned int Servo1_Angle = 0;//舵机1角度
unsigned int Servo2_Angle = 0;//舵机2角度

/******************************************************************
   配置占空比 范围 0 ~ (per-1)
   对于270度舵机:
   t = 0.5ms-------------------舵机会转动 0 °
   t = 1.0ms-------------------舵机会转动 67.5°
   t = 1.5ms-------------------舵机会转动 135°
   t = 2.0ms-------------------舵机会转动 202.5°
   t = 2.5ms-------------------舵机会转动 270°
******************************************************************/

/******************************************************************
 * 函 数 名 称：Set_Servo1_Angle
 * 函 数 说 明：设置舵机1的角度
 * 函 数 形 参：angle=要设置的角度，范围SERVO1_MIN_ANGLE到SERVO1_MAX_ANGLE
 * 函 数 返 回：无

******************************************************************/
void Set_Servo1_Angle(unsigned int angle)
{
    uint32_t period = 400;

    // 限制角度在设定范围内
    if(angle < SERVO1_MIN_ANGLE)
    {
        angle = SERVO1_MIN_ANGLE;
    }
    else if(angle > SERVO1_MAX_ANGLE)
    {
        angle = SERVO1_MAX_ANGLE;
    }

    Servo1_Angle = angle;

    // 计算PWM占空比
    // 0.5ms对应的计数 = 10
    // 2.5ms对应的计数 = 50
    float min_count = 10.0f;
    float max_count = 50.0f;
    float range = max_count - min_count;
    float ServoAngle = min_count + (((float)angle / SERVO_MAX_ANGLE) * range);

    DL_TimerG_setCaptureCompareValue(PWM_SG90_INST, (unsigned int)(ServoAngle + 0.5f), GPIO_PWM_SG90_C0_IDX);
}

/******************************************************************
 * 函 数 名 称：Set_Servo2_Angle
 * 函 数 说 明：设置舵机2的角度
 * 函 数 形 参：angle=要设置的角度，范围SERVO2_MIN_ANGLE到SERVO2_MAX_ANGLE
 * 函 数 返 回：无

******************************************************************/
void Set_Servo2_Angle(unsigned int angle)
{
    uint32_t period = 400;

    // 限制角度在设定范围内
    if(angle < SERVO2_MIN_ANGLE)
    {
        angle = SERVO2_MIN_ANGLE;
    }
    else if(angle > SERVO2_MAX_ANGLE)
    {
        angle = SERVO2_MAX_ANGLE;
    }

    Servo2_Angle = angle;

    // 计算PWM占空比
    // 0.5ms对应的计数 = 10
    // 2.5ms对应的计数 = 50
    float min_count = 10.0f;
    float max_count = 50.0f;
    float range = max_count - min_count;
    float ServoAngle = min_count + (((float)angle / SERVO_MAX_ANGLE) * range);

    DL_TimerG_setCaptureCompareValue(PWM_SG90_INST, (unsigned int)(ServoAngle + 0.5f), GPIO_PWM_SG90_C1_IDX); // 使用第二个通道C1
}

/******************************************************************
 * 函 数 名 称：Get_Servo1_Angle
 * 函 数 说 明：读取舵机1当前角度
 * 函 数 形 参：无
 * 函 数 返 回：当前角度
******************************************************************/
unsigned int Get_Servo1_Angle(void)
{
    return Servo1_Angle;
}

/******************************************************************
 * 函 数 名 称：Get_Servo2_Angle
 * 函 数 说 明：读取舵机2当前角度
 * 函 数 形 参：无
 * 函 数 返 回：当前角度
******************************************************************/
unsigned int Get_Servo2_Angle(void)
{
    return Servo2_Angle;
}



