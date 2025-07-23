#ifndef __SERIAL_H
#define __SERIAL_H
#include "../../system/sys/sys.h"
#include "ti_msp_dl_config.h"

// 定义接收数据变量
extern int uart_data;

void uart0_send_char(char ch);
void uart0_send_string(char* str);
void uart0_SendArray(uint8_t* data, uint8_t length);
void uart0_printf(const char *format, ...);

#endif

