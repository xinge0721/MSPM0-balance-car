#include "Serial.h"
#include "ti_msp_dl_config.h"
#include <stdarg.h>
#include <stdio.h>
#include "Hardware/SMS_STS/SMS_STS.h"

//串口发送单个字符
void uart1_send_char(char ch)
{
    //当串口1忙的时候等待，不忙的时候再发送传进来的字符
    while(DL_UART_isBusy(UART_1_INST) == true );
    //发送单个字符
    DL_UART_Main_transmitData(UART_1_INST, ch);
}

//串口发送字符串
void uart1_send_string(char* str)
{
    //当前字符串地址不在结尾 并且 字符串首地址不为空
    while(*str!=0&&str!=0)
    {
        //发送字符串首地址中的字符，并且在发送完成之后首地址自增
        uart1_send_char(*str++);
    }

}

void uart1_SendArray(uint8_t* data, uint8_t length)
{
    for(uint8_t i = 0; i < length; i++)
    {
        uart1_send_char(data[i]);
    }
}

// 串口printf函数实现
void uart1_printf(const char *format, ...)
{
    char buffer[256]; // 缓冲区大小可以根据需要调整
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    
    uart1_send_string(buffer);
}

volatile uint8_t uart1_data_arr[7];
volatile uint8_t uart_i;

// 定义接收数据变量
volatile uint8_t uart_data;
//串口的中断服务函数
void UART_1_INST_IRQHandler(void)
{
    //如果产生了串口中断
    switch( DL_UART_getPendingInterrupt(UART_1_INST) )
    {
        case DL_UART_IIDX_RX://如果是接收中断
            //将发送过来的数据保存在变量中
            uart_data = (uint8_t)DL_UART_Main_receiveData(UART_1_INST);

            SMS_STS_Process_SyncRead_Byte(uart_data);
            break;

        default://其他的串口中断
            break;
    }
}


