#ifndef __LINE_FOLLOWER_H__
#define __LINE_FOLLOWER_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "system\sys\sys.h"

extern int8_t A;

// 循迹板引脚宏定义
// 引脚1
#define IN1 PA_get(8)
// 引脚2
#define IN2 PA_get(9)
// 引脚3
#define IN3 PA_get(12)
// 引脚4
#define IN4 PA_get(13)
// 引脚5
#define IN5 PA_get(14)
// 引脚6
#define IN6 PA_get(21)
// 引脚7
#define IN7 PA_get(26)
// 引脚8
#define IN8 PB_get(3)
// 引脚9
#define IN9 PB_get(6)
// 引脚10
#define IN10 PB_get(12)
// 引脚11
#define IN11 PB_get(13)
// 引脚12   
#define IN12 PB_get(14)

uint16_t line_follower_read(void);
int16_t line_follower_get_error(void);

#endif