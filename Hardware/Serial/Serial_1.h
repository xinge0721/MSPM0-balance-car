#ifndef __SERIAL_1_H
#define __SERIAL_1_H
#include "../../system/sys/sys.h"
#include "ti_msp_dl_config.h"

void uart1_send_char(char ch);
void uart1_send_string(char* str);
void uart1_SendArray(uint8_t* data, uint8_t length);
void uart1_printf(const char *format, ...);
void uart1_process_data(uint8_t data);  // OpenMV数据处理函数


extern volatile uint8_t uart1_data_arr[];
extern volatile uint8_t uart_i;

#endif

