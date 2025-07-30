#ifndef __gray_H__
#define __gray_H__
#include "system/sys/sys.h"

// 循迹传感器变量声明
extern uint8_t IN0, IN1, IN2, IN3, IN4, IN5, IN6, IN7, IN8, IN9, IN10, IN11;

// 识别到黑线标志位
extern uint8_t A; 
extern uint8_t A_stable; // 防误判处理后的稳定状态
extern int8_t turn_err_k;

int xunji(void);
void gw_gray_serial_read_simple();
int process_sensors();
uint8_t process_line_detection_debounce(uint8_t current_state); // 循迹板防误判处理函数


#endif