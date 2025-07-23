#include "delay.h"

// 定义一个全局变量，用来记录延时的时间�?
volatile unsigned int delay_times = 0;


// ****** 普通定时器 *******

//搭配滴答定时器实现的精确ms延时
void delay_ms(unsigned int ms)
{
        delay_times = ms;
        while( delay_times != 0 );
}

//搭配滴答定时器实现的精确s延时
void delay_s(unsigned int s)
{
        delay_times = s * 1000;
        while( delay_times != 0 );
}


void delay_1us(int __us) { delay_cycles( (CPUCLK_FREQ / 1000 / 1000)*__us); }
