#include "Serial.h"
#include "ti_msp_dl_config.h"
#include <stdarg.h>
#include <stdio.h>
#include "Hardware/SMS_STS/SMS_STS.h"

int uart_arr[8] = {0};


// OpenMV通信协议相关变量
uint8_t uart0_arr[8] = {0};           // UART0接收数据缓冲区
uint8_t packet_received_count = 0;    // 数据包接收计数器
uint8_t OpenMv_status = 0;            // OpenMV状态
int16_t OpenMv_X = 0;                 // X坐标值（带符号）
int16_t OpenMv_Y = 0;                 // Y坐标值（带符号）

void BT_SendString(char*str)
{
    while(*str != '\0')
    {
        DL_UART_Main_transmitDataBlocking(UART_1_INST,*str++);
    }
}

//串口发送单个字符
void uart0_send_char(char ch)
{
    //当串口0忙的时候等待，不忙的时候再发送传进来的字符
    while(DL_UART_isBusy(UART_0_INST) == true );
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

// 
int16_t get_angle_x()
{
    int16_t temp = OpenMv_X;
    OpenMv_X = 0;
    return temp;
}
int16_t get_angle_y()
{
    int16_t temp = OpenMv_Y;
    OpenMv_Y = 0;
    return temp;
}

// UART0接收数据处理函数
void uart0_process_data(uint8_t data)
{
    // 调试输出：显示当前接收状态和数据
    // uart0_printf("接收[%d]: 0x%02X\r\n", packet_received_count, data);
    
    // 将接收到的数据存储到缓冲区
    switch (packet_received_count)
    {
    case 0://等待包头
        if(data == 0xEE)
        {
            uart0_arr[0] = data;
            packet_received_count++;
        }
        else
        {
            packet_received_count = 0;
        }
        break;
    case 1://等待状态值
        uart0_arr[1] = data;
        packet_received_count++;
        break;
    case 2://等待X坐标标志位
        uart0_arr[2] = data;
        packet_received_count++;
        break;
    case 3://等待X坐标值
        uart0_arr[3] = data;
        packet_received_count++;
        break;
    case 4://等待Y坐标标志位
        uart0_arr[4] = data;
        packet_received_count++;
        break;
    case 5://等待Y坐标值
        uart0_arr[5] = data;
        packet_received_count++;
        break;
    case 6://计算校验和
        {
            uint8_t calculated_checksum = (uart0_arr[1] + uart0_arr[2] + uart0_arr[3] + uart0_arr[4] + uart0_arr[5]) % 256;
            
            // 调试输出：显示接收到的完整数据包
            // uart0_printf("收到数据包: %02X %02X %02X %02X %02X %02X %02X\r\n",
            //             uart0_arr[0], uart0_arr[1], uart0_arr[2], uart0_arr[3],
            //             uart0_arr[4], uart0_arr[5], data);
            
            // uart0_printf("计算校验和: %02X, 接收校验和: %02X\r\n", calculated_checksum, data);
            
            if(data == calculated_checksum)
            {
                OpenMv_status = uart0_arr[1];
                // 根据标志位直接计算带符号的坐标值
                if(uart0_arr[2] == 0) {
                    OpenMv_X = (int16_t)uart0_arr[3];  // X坐标为正
                } else {
                    OpenMv_X = -(int16_t)uart0_arr[3]; // X坐标为负
                }
                
                if(uart0_arr[4] == 0) {
                    OpenMv_Y = (int16_t)uart0_arr[5];  // Y坐标为正
                } else {
                    OpenMv_Y = -(int16_t)uart0_arr[5]; // Y坐标为负
                }
                
                // uart0_printf("数据包验证成功！状态: %d, X: %d, Y: %d\r\n", OpenMv_status, OpenMv_X, OpenMv_Y);
            }
            else
            {
                // uart0_printf("校验和验证失败！\r\n");
            }
            
            // 重置接收计数器，准备接收下一个数据包
            packet_received_count = 0;
        }
        break;
    default:
        // 异常情况，重置接收计数器
        packet_received_count = 0;
        break;
    }
}
volatile int comtaaa = 0;
volatile int iiiii = 0;
// 定义接收数据变量
volatile uint8_t uart_data;
//串口的中断服务函数
void UART_0_INST_IRQHandler(void)
{
    //如果产生了串口中断
    switch( DL_UART_getPendingInterrupt(UART_0_INST) )
    {
        case DL_UART_IIDX_RX://如果是接收中断
            //将发送过来的数据保存在变量中
            uart_data = (uint8_t)DL_UART_Main_receiveData(UART_0_INST);
            // uart0_send_char(uart_data);

            // 调用OpenMV通信协议处理函数
            uart0_process_data(uart_data);
            // 原有的调试代码，可保留用于备用调试
            uart_arr[comtaaa ++] = uart_data;
            iiiii++;
            if(comtaaa >= 8) // 使用 >= 防止意外越界
            {
                comtaaa = 0;
            }
        
            break;

        default://其他的串口中断
            break;
    }
}


