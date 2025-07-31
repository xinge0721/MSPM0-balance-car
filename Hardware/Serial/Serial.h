#ifndef __SERIAL_H
#define __SERIAL_H
#include "../../system/sys/sys.h"
#include "ti_msp_dl_config.h"

// 定义接收数据变量
extern int uart_arr[8] ;
extern volatile int comtaaa;
extern volatile int iiiii;

// OpenMV通信协议相关变量
extern uint8_t uart0_arr[8];           // UART0接收数据缓冲区
extern uint8_t packet_received_count;  // 数据包接收计数器
extern uint8_t OpenMv_status;          // OpenMV状态
extern int16_t OpenMv_X;               // X坐标值（带符号）
extern int16_t OpenMv_Y;               // Y坐标值（带符号）

void uart0_send_char(char ch);
void uart0_send_string(char* str);
void uart0_SendArray(uint8_t* data, uint8_t length);
void uart0_printf(const char *format, ...);
void uart0_process_data(uint8_t data);  // OpenMV数据处理函数

void BT_SendString(char*str);

#endif

