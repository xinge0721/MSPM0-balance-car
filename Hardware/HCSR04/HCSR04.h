#ifndef __HCSR04_H__
#define __HCSR04_H__
#include "system/sys/sys.h"

#define SR04_TRIG(x)  ( x ? DL_GPIO_setPins(SR04_PORT,SR04_Trig_PIN) : DL_GPIO_clearPins(SR04_PORT,SR04_Trig_PIN) )
#define SR04_ECHO()   ( ( ( DL_GPIO_readPins(SR04_PORT,SR04_Echo_PIN) & SR04_Echo_PIN ) > 0 ) ? 1 : 0 )

// 定义超声波测量状态
typedef enum {
    SR04_IDLE,
    SR04_WAIT_FALLING_EDGE
} SR04_State;

extern volatile uint32_t msHcCount; // 用于在定时器中断中累计毫秒数
extern float distance; // 存储计算出的距离值，单位是厘米
extern volatile uint8_t SR04_Flag; // 一个标志位，为1时表示一次成功的超声波测距已经完成

// 超声波模块的状态,外部不可访问
extern volatile SR04_State sr04_state;
// 超声波模块的初始时间,外部不可访问
extern volatile uint32_t HCSR04_StartTime;



void SR04_Init(void);//超声波初始化
float SR04_GetLength(void );//获取超声波测距的距离

#endif
