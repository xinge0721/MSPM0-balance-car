#ifndef __SYS_H_
#define __SYS_H_
#include "ti_msp_dl_config.h"
#include <math.h>
#include <stdio.h>
#include "Hardware/Servo/Servo.h"
#include "system/delay/delay.h"
#include "Hardware/Encoder/Encoder.h"
#include "Hardware/OLED/OLED.h"
#include "Hardware/Serial/Serial.h"
#include "Hardware/PID/pid.h"
#include "Hardware/Control/Control.h"
#include "Hardware/MPU6050/mpu6050.h"
#include "Hardware/APP/APP.h"   
#include "wit.h"
#include "Hardware/HCSR04/HCSR04.h"
/* 基本数据类型定义 */
typedef unsigned char  uint8_t;    /* 无符��?8位整��? */
typedef signed   char  int8_t;     /* 有符��?8位整��? */
typedef unsigned short uint16_t;   /* 无符��?16位整��? */
typedef signed   short int16_t;    /* 有符��?16位整��? */
typedef unsigned int   uint32_t;   /* 无符��?32位整��? */
typedef signed   int   int32_t;    /* 有符��?32位整��? */
typedef unsigned long long uint64_t;  /* 无符��?64位整��? */
typedef signed   long long int64_t;   /* 有符��?64位整��? */

/* 常用类型重定��? */
typedef unsigned char  u8;         /* 无符��?8位整��? */
typedef unsigned short u16;        /* 无符��?16位整��? */
typedef unsigned int   u32;        /* 无符��?32位整��? */

typedef signed char    s8;         /* 有符��?8位整��? */
typedef signed short   s16;        /* 有符��?16位整��? */
typedef signed int     s32;        /* 有符��?32位整��? */

typedef volatile unsigned char  vu8;  /* 易变无符��?8位整��? */
typedef volatile unsigned short vu16; /* 易变无符��?16位整��? */
typedef volatile unsigned int   vu32; /* 易变无符��?32位整��? */


void system_init(void);
int date_i;
// 左编码器��?
extern float left_encoder_value;
// 右编码器��?
extern float right_encoder_value;

// 右轮PID
extern pid right;
// 左轮PID
extern pid left;
// 旋转PID
extern pid turn;
#endif /* __SYS_H */ 