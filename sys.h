#ifndef __SYS_H
#define __SYS_H

/* 基本数据类型定义 */
typedef unsigned char  uint8_t;    /* 无符号8位整型 */
typedef signed   char  int8_t;     /* 有符号8位整型 */
typedef unsigned short uint16_t;   /* 无符号16位整型 */
typedef signed   short int16_t;    /* 有符号16位整型 */
typedef unsigned int   uint32_t;   /* 无符号32位整型 */
typedef signed   int   int32_t;    /* 有符号32位整型 */
typedef unsigned long long uint64_t;  /* 无符号64位整型 */
typedef signed   long long int64_t;   /* 有符号64位整型 */

/* 数据类型最值定义 */
#define INT8_MAX    0x7F           /* int8_t最大值 */
#define INT8_MIN    (-INT8_MAX-1)  /* int8_t最小值 */
#define UINT8_MAX   0xFF           /* uint8_t最大值 */
#define INT16_MAX   0x7FFF         /* int16_t最大值 */
#define INT16_MIN   (-INT16_MAX-1) /* int16_t最小值 */
#define UINT16_MAX  0xFFFF         /* uint16_t最大值 */
#define INT32_MAX   0x7FFFFFFF     /* int32_t最大值 */
#define INT32_MIN   (-INT32_MAX-1) /* int32_t最小值 */
#define UINT32_MAX  0xFFFFFFFF     /* uint32_t最大值 */

/* 常用类型重定义 */
typedef unsigned char  u8;         /* 无符号8位整型 */
typedef unsigned short u16;        /* 无符号16位整型 */
typedef unsigned int   u32;        /* 无符号32位整型 */

typedef signed char    s8;         /* 有符号8位整型 */
typedef signed short   s16;        /* 有符号16位整型 */
typedef signed int     s32;        /* 有符号32位整型 */

typedef volatile unsigned char  vu8;  /* 易变无符号8位整型 */
typedef volatile unsigned short vu16; /* 易变无符号16位整型 */
typedef volatile unsigned int   vu32; /* 易变无符号32位整型 */



#endif /* __SYS_H */ 