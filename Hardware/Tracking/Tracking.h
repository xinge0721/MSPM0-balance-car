
#ifndef __TRACKING_H
#define __TRACKING_H

#include "system/sys/sys.h"

// 定义循迹状态的结构体
typedef struct {
    int deviation;      // 循迹偏差值，负数表示右偏，正数表示左偏
    int line_detected;  // 是否检测到循迹线 (1: 是, 0: 否)
} TrackingStatus;

#define IN1 DL_GPIO_readPins(Tracking_IN1_PORT, Tracking_IN1_PIN)//A08
#define IN2 DL_GPIO_readPins(Tracking_IN2_PORT, Tracking_IN2_PIN)//A09
#define IN3 DL_GPIO_readPins(Tracking_IN3_PORT, Tracking_IN3_PIN)//A12
#define IN4 DL_GPIO_readPins(Tracking_IN4_PORT, Tracking_IN4_PIN)//A13
#define IN5 DL_GPIO_readPins(Tracking_IN5_PORT, Tracking_IN5_PIN)//A14
#define IN6 DL_GPIO_readPins(Tracking_IN6_PORT, Tracking_IN6_PIN)//A21
#define IN7 DL_GPIO_readPins(Tracking_IN7_PORT, Tracking_IN7_PIN)//A26
#define IN8 DL_GPIO_readPins(Tracking_IN8_PORT, Tracking_IN8_PIN)//B03
#define IN9 DL_GPIO_readPins(Tracking_IN9_PORT, Tracking_IN9_PIN)//B06
#define IN10 DL_GPIO_readPins(Tracking_IN10_PORT, Tracking_IN10_PIN)//B12
#define IN11 DL_GPIO_readPins(Tracking_IN11_PORT, Tracking_IN11_PIN)//B13
#define IN12 DL_GPIO_readPins(Tracking_IN12_PORT, Tracking_IN12_PIN)//B14

TrackingStatus xunji(void);

#endif 