#ifndef __DELAY_H
#define __DELAY_H
#include "ti_msp_dl_config.h"
#include "Hardware/MPU6050/mspm0_clock.h"
extern volatile unsigned int delay_times;
extern volatile unsigned long tick_ms;


void delay_ms(unsigned int ms);
void delay_s(unsigned int s);
void delay_1us(int __us);
#endif