#ifndef __UART1_H
#define __UART1_H
#include "../../system/sys/sys.h"
#include "ti_msp_dl_config.h"

// 注意：OpenMV数据处理功能已完全移植到interrupt.c中
// 如需使用OpenMV数据，请包含相应头文件或添加extern声明
extern uint8_t UART_1_cont;
// UART1基础发送函数
void uart1_send_char(char ch);
void uart1_send_string(char* str);
void uart1_SendArray(uint8_t* data, uint8_t length);
void uart1_printf(const char *format, ...);

// UART1初始化函数（如果需要特殊初始化）
void uart1_init(void);



#endif 