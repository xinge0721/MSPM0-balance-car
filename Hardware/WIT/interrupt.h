#ifndef _INTERRUPT_H_
#define _INTERRUPT_H_

#include <stdint.h>

// OpenMV数据处理相关变量声明（定义在interrupt.c中）
extern int openmv_arr[8];
extern uint8_t OpenMv_status;
extern int16_t OpenMv_X;
extern int16_t OpenMv_Y;
extern volatile uint8_t openmv_packet_count;

// OpenMV数据处理函数声明
void process_openmv_data(uint8_t data);

#endif  /* #ifndef _INTERRUPT_H_ */