#include "Serial.h"
#include "ti_msp_dl_config.h"
#include <stdarg.h>
#include <stdio.h>

// 定义接收数据变量
uint8_t uart_data;
int g_angle_deviation = 0; // 用于存储处理后的角度偏差

// 用于串口数据处理的状态机变量
static int s_receive_state = 0; // 0: 等待标志位, 1: 等待数值位
static int s_sign = 1;          // 存储符号, 1表示正数, -1表示负数

//串口发送单个字符
void uart0_send_char(char ch)
{
    //当串口0忙的时候等待，不忙的时候再发送传进来的字符
    while( DL_UART_isBusy(UART_0_INST) == true );
    //发送单个字符
    DL_UART_Main_transmitData(UART_0_INST, ch);
}
//串口发送字符串
void uart0_send_string(char* str)
{
    //当前字符串地址不在结尾 并且 字符串首地址不为空
    while(*str!=0&&str!=0)
    {
        //发送字符串首地址中的字符，并且在发送完成之后首地址自增
        uart0_send_char(*str++);
    }
}
void uart0_SendArray(uint8_t* data, uint8_t length)
{
    for(uint8_t i = 0; i < length; i++)
    {
        uart0_send_char(data[i]);
    }
}

// 串口printf函数实现
void uart0_printf(const char *format, ...)
{
    char buffer[256]; // 缓冲区大小可以根据需要调整
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    
    uart0_send_string(buffer);
}

void process_received_data(uint8_t data)
{
    if (s_receive_state == 0) // 等待标志位
    {
        if (data == 0xee) // 负数
        {
            s_sign = -1;
            s_receive_state = 1; // 切换到等待数值位状态
        }
        else if (data == 0x00) // 正数
        {
            s_sign = 1;
            s_receive_state = 1; // 切换到等待数值位状态
        }
        // 如果不是标志位，则忽略，继续等待标志位
    }
    else // s_receive_state == 1, 等待数值位
    {
        // 第二个字节是绝对值
        g_angle_deviation = s_sign * data;
        
        // 重置状态机，准备接收下一组数据
        s_receive_state = 0;
    }
}

//串口的中断服务函数
void UART_0_INST_IRQHandler(void)
{
    //如果产生了串口中断
    switch( DL_UART_getPendingInterrupt(UART_0_INST) )
    {
        case DL_UART_IIDX_RX://如果是接收中断
            //将发送过来的数据保存在变量中
            uart_data = DL_UART_Main_receiveData(UART_0_INST);
            uart0_send_char(uart_data);
            process_received_data(uart_data);
            //将保存的数据再发送出去
            break;

        default://其他的串口中断
            break;
    }
}