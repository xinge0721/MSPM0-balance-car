#ifndef __MSPM0_CLOCK_H__
#define __MSPM0_CLOCK_H__

#include "ti_msp_dl_config.h"

int mspm0_delay_ms(unsigned long num_ms);
int mspm0_get_clock_ms(unsigned long *count);
extern volatile unsigned long tick_ms;
extern volatile uint32_t start_time;


#endif