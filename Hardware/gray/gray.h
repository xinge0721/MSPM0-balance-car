#ifndef __gray_H__
#define __gray_H__
#include "system/sys/sys.h"

// 识别到黑线标志位
extern uint8_t A; 
extern int8_t turn_err_k;

int xunji(void);
void gw_gray_serial_read_simple();
int process_sensors();


#endif