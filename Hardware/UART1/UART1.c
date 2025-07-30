#include "UART1.h"
#include "ti_msp_dl_config.h"
#include <stdarg.h>
#include <stdio.h>



// UART1发送单个字符
void uart1_send_char(char ch)
{
    // 当串口1忙的时候等待，不忙的时候再发送传进来的字符
    while(DL_UART_isBusy(UART_1_INST) == true);
    // 发送单个字符
    DL_UART_Main_transmitData(UART_1_INST, ch);
}

// UART1发送字符串
void uart1_send_string(char* str)
{
    // 当前字符串地址不在结尾 并且 字符串首地址不为空
    while(*str != 0 && str != 0)
    {
        // 发送字符串首地址中的字符，并且在发送完成之后首地址自增
        uart1_send_char(*str++);
    }
}

// UART1发送数组
void uart1_SendArray(uint8_t* data, uint8_t length)
{
    for(uint8_t i = 0; i < length; i++)
    {
        uart1_send_char(data[i]);
    }
}



// UART1 printf函数实现
void uart1_printf(const char *format, ...)
{
    char buffer[256]; // 缓冲区大小可以根据需要调整
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    
    uart1_send_string(buffer);
}

// 数据包处理相关变量定义
// 状态值
uint8_t OpenMv_status = 0;
// 坐标值
uint8_t OpenMv_X = 0;
uint8_t OpenMv_Y = 0;
// 数据包接收标志
volatile uint8_t packet_received_count = 0;

// UART1相关变量定义
int uart1_arr[8] = {0};

// UART1接收数据处理函数
void uart1_process_data(uint8_t data)
{
    // 将接收到的数据存储到缓冲区
    switch (packet_received_count)
    {
    case 0://等待第一个包头
        if(data == 0xEE)
        {
            uart1_arr[0] = data;
            packet_received_count++;
        }
        else
        {
            packet_received_count = 0;
        }
        break;
    case 1://等待第二个包头
        if(data == 0xEE)
        {
            uart1_arr[1] = data;
            packet_received_count++;
        }
        else
        {
            packet_received_count = 0;
        }
        break;
    case 2://等待状态值
        uart1_arr[2] = data;
        packet_received_count++;
        break;
    case 3://等待坐标值1
        uart1_arr[3] = data;
        packet_received_count++;
        break;
    case 4://等待坐标值2
        uart1_arr[4] = data;
        packet_received_count++;
        break;
    case 5://计算校验和
        if(data == (uart1_arr[2] + uart1_arr[3] + uart1_arr[4]) % 256)
        {
            OpenMv_status = uart1_arr[2];
            OpenMv_X = uart1_arr[3];
            OpenMv_Y = uart1_arr[4];
        }
        packet_received_count = 0;//到了这一步，无论对错，都重置计数器
        break;
    }

}

// UART1的中断服务函数
void UART_1_INST_IRQHandler(void)
{
    // 如果产生了串口中断
    switch(DL_UART_getPendingInterrupt(UART_1_INST))
    {
        case DL_UART_IIDX_RX: // 如果是接收中断
        {
            // 将发送过来的数据保存在变量中
            uint8_t uart1_data = (uint8_t)DL_UART_Main_receiveData(UART_1_INST);
            uart1_send_char(uart1_data);
            // 调用数据处理函数
            uart1_process_data(uart1_data);
            
            break;
        }

        case DL_UART_IIDX_TX: // 如果是发送中断
            // 发送完成处理（如果需要）
            break;

        default: // 其他的串口中断
            break;
    }
} 