#ifndef __UART1_H
#define __UART1_H
#include "../../system/sys/sys.h"
#include "ti_msp_dl_config.h"

// UART1相关变量声明
extern int uart1_arr[8];
// 数据包处理相关变量定义
// 状态值
extern uint8_t OpenMv_status;
// 坐标值
extern uint8_t OpenMv_X;
extern uint8_t OpenMv_Y;

// UART1基础发送函数
void uart1_send_char(char ch);
void uart1_send_string(char* str);
void uart1_SendArray(uint8_t* data, uint8_t length);
void uart1_printf(const char *format, ...);

// UART1初始化函数（如果需要特殊初始化）
void uart1_init(void);

// UART1接收数据处理函数
void uart1_process_data(uint8_t data);

#endif 