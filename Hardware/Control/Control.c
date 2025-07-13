#include "ti_msp_dl_config.h"
#include "Control.h"
#include <stdlib.h>

// 控制AIN1和AIN2的引脚

// 电机方向控制宏定义
#define MOTOR_left_DIRECTION 1  // 1为正常方向，0为反向
#define MOTOR_right_DIRECTION 0  // 1为正常方向，0为反向

// 左电机方向控制
// 注释：因为不是所有人的接线方式都一样，所以需要根据实际情况来选择
// 参数中的AIN1开，和参数中的AIN2开，和参数中的AIN1关，和参数中的AIN2关，都是根据实际情况来选择的
#if MOTOR_left_DIRECTION == 1
// 正常方向
// AIN1开
#define Control_AIN1_ON() (DL_GPIO_setPins(Control_AIN1_PORT, Control_AIN1_PIN))
// AIN1关
#define Control_AIN1_OFF() (DL_GPIO_clearPins(Control_AIN1_PORT, Control_AIN1_PIN))
// AIN2开
#define Control_AIN2_ON() (DL_GPIO_setPins(Control_AIN2_PORT, Control_AIN2_PIN))
// AIN2关
#define Control_AIN2_OFF() (DL_GPIO_clearPins(Control_AIN2_PORT, Control_AIN2_PIN))
#else
// 反向
// AIN1开
#define Control_AIN1_ON() (DL_GPIO_setPins(Control_AIN2_PORT, Control_AIN2_PIN))
// AIN1关
#define Control_AIN1_OFF() (DL_GPIO_clearPins(Control_AIN2_PORT, Control_AIN2_PIN))
// AIN2开
#define Control_AIN2_ON() (DL_GPIO_setPins(Control_AIN1_PORT, Control_AIN1_PIN))
// AIN2关
#define Control_AIN2_OFF() (DL_GPIO_clearPins(Control_AIN1_PORT, Control_AIN1_PIN))
#endif


#if MOTOR_right_DIRECTION == 1
// 正常方向
// BIN1开
#define Control_BIN1_ON() DL_GPIO_setPins(Control_BIN1_PORT, Control_BIN1_PIN)
// BIN1关
#define Control_BIN1_OFF() DL_GPIO_clearPins(Control_BIN1_PORT, Control_BIN1_PIN)
// BIN2开
#define Control_BIN2_ON() DL_GPIO_setPins(Control_BIN2_PORT, Control_BIN2_PIN)
// BIN2关
#define Control_BIN2_OFF() DL_GPIO_clearPins(Control_BIN2_PORT, Control_BIN2_PIN)
#else
// 反向
// BIN1开
#define Control_BIN1_ON() DL_GPIO_setPins(Control_BIN2_PORT, Control_BIN2_PIN)
// BIN1关
#define Control_BIN1_OFF() DL_GPIO_clearPins(Control_BIN2_PORT, Control_BIN2_PIN)
// BIN2开
#define Control_BIN2_ON() DL_GPIO_setPins(Control_BIN1_PORT, Control_BIN1_PIN)
// BIN2关
#define Control_BIN2_OFF() DL_GPIO_clearPins(Control_BIN1_PORT, Control_BIN1_PIN)
#endif

//  前进    
void Control_forward(void)
{
    Control_AIN1_ON();
    Control_AIN2_OFF();
    Control_BIN1_ON();
    Control_BIN2_OFF();
}

//  后退
void Control_back(void)
{
    Control_AIN1_OFF();
    Control_AIN2_ON();
    Control_BIN1_OFF();
    Control_BIN2_ON();
}

//  停止
void Control_stop(void)
{
    Control_AIN1_OFF();
    Control_AIN2_OFF();
    Control_BIN1_OFF();
    Control_BIN2_OFF();
}

void Control_speed(int left_speed,int right_speed)
{
    if(left_speed>0)
    {
        Control_AIN1_ON();
        Control_AIN2_OFF();
        DL_TimerA_setCaptureCompareValue(PWM_INST,left_speed,DL_TIMER_CC_0_INDEX);

    }
    else if(left_speed<0)
    {
        Control_AIN1_OFF();
        Control_AIN2_ON();
        DL_TimerA_setCaptureCompareValue(PWM_INST,-(left_speed),DL_TIMER_CC_0_INDEX);
    }
    else
    {
        Control_AIN1_OFF();
        Control_AIN2_OFF();
        DL_TimerA_setCaptureCompareValue(PWM_INST,0,DL_TIMER_CC_0_INDEX);
    }
    
    if(right_speed>0)
    {
        Control_BIN1_ON();
        Control_BIN2_OFF();
        DL_TimerA_setCaptureCompareValue(PWM_INST,right_speed,DL_TIMER_CC_1_INDEX);
    }
    else if(right_speed<0)
    {
        Control_BIN1_OFF();
        Control_BIN2_ON();
        DL_TimerA_setCaptureCompareValue(PWM_INST,-(right_speed),DL_TIMER_CC_1_INDEX);
    }
    else
    {
        Control_BIN1_OFF();
        Control_BIN2_OFF();
        DL_TimerA_setCaptureCompareValue(PWM_INST,0,DL_TIMER_CC_1_INDEX);
    }
}
