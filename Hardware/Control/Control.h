#ifndef __CONTROL_H
#define __CONTROL_H
#include "../../system/sys/sys.h"

void Control_forward(void);
void Control_back(void);
void Control_stop(void);
void Control_speed(int left_speed,int right_speed);

#endif