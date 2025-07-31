#include "ti_msp_dl_config.h"
#include "interrupt.h"
#include "mpu6050.h"
// #include "bno08x_uart_rvc.h"
#include "wit.h"
#include "system/delay/delay.h"
#include "Hardware/Serial/Serial.h"

/*
 * DMA傻瓜式解释：
 * 什么是DMA？
 * DMA (Direct Memory Access) = 直接内存访问
 * 简单说就是：让硬件自动搬运数据，不需要CPU一个字节一个字节的搬
 * 
 * 为什么要用DMA？
 * 1. CPU不用一直等着接收数据，可以干别的事
 * 2. 数据传输速度更快
 * 3. 减少CPU负担
 * 
 * DMA工作流程：
 * 1. 设定搬运起点（数据从哪来）
 * 2. 设定搬运终点（数据到哪去）
 * 3. 设定搬运数量（搬多少个字节）
 * 4. 启动DMA，硬件自动搬运
 * 5. 搬完了产生中断，告诉CPU"搬完了！"
 */

// UART中断服务函数 - 当UART接收到数据时自动调用
/*
 * BNO08X传感器的UART+DMA中断处理函数
 * 当BNO08X传感器通过UART发送数据时，DMA自动接收，接收完毕后触发这个中断
 */
#if defined UART_BNO08X_INST_IRQHandler
void UART_BNO08X_INST_IRQHandler(void)
{
    uint8_t checkSum = 0;                           // 校验和，用来检查数据是否正确
    extern uint8_t bno08x_dmaBuffer[19];            // DMA接收缓冲区，存放从传感器收到的数据

    /* 步骤1：暂停DMA传输，准备处理数据 */
    DL_DMA_disableChannel(DMA, DMA_BNO08X_CHAN_ID); // 关闭DMA通道，防止在处理数据时被打断
    
    /* 步骤2：计算实际接收到多少字节数据 */
    // 原本设定要传输18字节，剩余多少就是已经传输了多少
    uint8_t rxSize = 18 - DL_DMA_getTransferSize(DMA, DMA_BNO08X_CHAN_ID);

    /* 步骤3：检查UART缓冲区是否还有数据没被DMA搬完 */
    if(DL_UART_isRXFIFOEmpty(UART_BNO08X_INST) == false)
        bno08x_dmaBuffer[rxSize++] = DL_UART_receiveData(UART_BNO08X_INST);  // 手动取出剩余数据

    /* 步骤4：计算校验和，确保数据没有传输错误 */
    for(int i=2; i<=14; i++)
        checkSum += bno08x_dmaBuffer[i];                // 把数据包中间部分加起来

    /* 步骤5：验证数据包格式和校验和 */
    // 检查：数据长度对不对(19字节) + 包头对不对(0xAA 0xAA) + 校验和对不对
    if((rxSize == 19) && (bno08x_dmaBuffer[0] == 0xAA) && (bno08x_dmaBuffer[1] == 0xAA) && (checkSum == bno08x_dmaBuffer[18]))
    {
        /* 步骤6：解析数据，把字节数据转换成有意义的数值 */
        bno08x_data.index = bno08x_dmaBuffer[2];        // 数据包索引
        
        // 角度数据：两个字节组合成一个16位数，然后除以100得到度数
        bno08x_data.yaw = (int16_t)((bno08x_dmaBuffer[4]<<8)|bno08x_dmaBuffer[3]) / 100.0;     // 偏航角
        bno08x_data.pitch = (int16_t)((bno08x_dmaBuffer[6]<<8)|bno08x_dmaBuffer[5]) / 100.0;   // 俯仰角
        bno08x_data.roll = (int16_t)((bno08x_dmaBuffer[8]<<8)|bno08x_dmaBuffer[7]) / 100.0;    // 横滚角
        
        // 加速度数据：两个字节组合成一个16位数
        bno08x_data.ax = (bno08x_dmaBuffer[10]<<8)|bno08x_dmaBuffer[9];   // X轴加速度
        bno08x_data.ay = (bno08x_dmaBuffer[12]<<8)|bno08x_dmaBuffer[11];  // Y轴加速度
        bno08x_data.az = (bno08x_dmaBuffer[14]<<8)|bno08x_dmaBuffer[13];  // Z轴加速度
    }
    
    /* 步骤7：清空UART接收缓冲区的残留数据 */
    uint8_t dummy[4];
    DL_UART_drainRXFIFO(UART_BNO08X_INST, dummy, 4);   // 把可能的垃圾数据清掉

    /* 步骤8：重新启动DMA，准备接收下一包数据 */
    DL_DMA_setDestAddr(DMA, DMA_BNO08X_CHAN_ID, (uint32_t) &bno08x_dmaBuffer[0]);  // 设定DMA目标地址：数据要存到哪
    DL_DMA_setTransferSize(DMA, DMA_BNO08X_CHAN_ID, 18);                           // 设定DMA传输大小：要传输多少字节
    DL_DMA_enableChannel(DMA, DMA_BNO08X_CHAN_ID);                                 // 启动DMA，开始下一轮接收
}
#endif

/*
 * OpenMV数据的UART+DMA中断处理函数
 * OpenMV发送数据包，每个包长度8字节
 * 使用DMA自动接收，接收完毕后触发这个中断，然后逐字节处理
 */
#if defined UART_WIT_INST_IRQHandler
void UART_WIT_INST_IRQHandler(void)
{
    uint8_t openmv_dmaBuffer[16];                   // OpenMV DMA接收缓冲区，最多存放16字节(大约2个数据包)

    /* 步骤1：暂停DMA传输，准备处理数据 */
    DL_DMA_disableChannel(DMA, DMA_WIT_CHAN_ID);     // 关闭DMA通道，防止在处理数据时被打断
    
    /* 步骤2：计算实际接收到的数据大小 */
    uint8_t rxSize = 16 - DL_DMA_getTransferSize(DMA, DMA_WIT_CHAN_ID);

    /* 步骤3：检查UART缓冲区是否还有数据没被DMA搬完 */
    if(DL_UART_isRXFIFOEmpty(UART_WIT_INST) == false)
        openmv_dmaBuffer[rxSize++] = DL_UART_receiveData(UART_WIT_INST);  // 手动取出剩余数据

    /* 步骤4：逐字节处理接收到的OpenMV数据 */
    // 先将DMA接收到的原始数据回传到串口0（十六进制格式）
    if(rxSize > 0)
    {
        uart0_printf("DMA接收[%d字节]: ", rxSize);
        for(uint8_t i = 0; i < rxSize; i++)
        {
            uart0_printf("%02X ", openmv_dmaBuffer[i]);
        }
        uart0_printf("\r\n");
    }
    
    // 将DMA缓冲区中的每个字节传递给process_openmv_data函数处理
    for(uint8_t i = 0; i < rxSize; i++)
    {
        process_openmv_data(openmv_dmaBuffer[i]);         // 调用OpenMV数据处理函数
    }
    
    /* 步骤5：清空UART接收缓冲区的残留数据 */
    uint8_t dummy[4];
    DL_UART_drainRXFIFO(UART_WIT_INST, dummy, 4);      // 把可能的垃圾数据清掉

    /* 步骤6：重新启动DMA，准备接收下一批数据 */
    DL_DMA_setDestAddr(DMA, DMA_WIT_CHAN_ID, (uint32_t) &openmv_dmaBuffer[0]);     // 设定DMA目标地址
    DL_DMA_setTransferSize(DMA, DMA_WIT_CHAN_ID, 16);                              // 设定DMA传输大小：16字节
    DL_DMA_enableChannel(DMA, DMA_WIT_CHAN_ID);                                    // 启动DMA，开始下一轮接收
}
#endif

/*
 * ===========================================
 * MSPM0G3507 DMA使用总结 - 傻瓜式指南
 * ===========================================
 * 
 * 什么时候需要DMA？
 * 1. 当需要高速接收传感器数据时
 * 2. 当CPU需要同时做其他事情时
 * 3. 当数据量比较大，一个字节一个字节接收太慢时
 * 
 * 当前支持的DMA处理：
 * 1. BNO08X传感器 - 使用UART_BNO08X_INST_IRQHandler
 * 2. OpenMV数据 - 使用UART_WIT_INST_IRQHandler (原WIT传感器通道)
 * 
 * OpenMV DMA处理流程：
 * 1. DMA自动接收OpenMV发送的数据到缓冲区
 * 2. 接收完成后触发UART_WIT_INST_IRQHandler中断
 * 3. 中断函数逐字节调用process_openmv_data()处理数据
 * 4. process_openmv_data()解析数据包，更新OpenMv_X, OpenMv_Y, OpenMv_status
 * 5. 重新启动DMA等待下一批数据
 * 
 * DMA配置步骤(在其他地方已经配置好了)：
 * 1. 配置DMA通道 (DMA_BNO08X_CHAN_ID 或 DMA_WIT_CHAN_ID)
 * 2. 设置源地址 (UART接收寄存器)
 * 3. 设置目标地址 (数据缓冲区)
 * 4. 设置传输大小
 * 5. 启用中断
 * 
 * 如何使用OpenMV数据？
 * 1. 包含头文件: #include "Hardware/WIT/interrupt.h"
 * 2. 直接读取全局变量:
 *    - int16_t x = OpenMv_X;     // X坐标
 *    - int16_t y = OpenMv_Y;     // Y坐标  
 *    - uint8_t status = OpenMv_status; // 状态
 * 3. 所有数据处理都由DMA+中断自动完成
 * 
 * 注意事项：
 * - 在中断函数中不要做太复杂的运算，会影响实时性
 * - 记得校验数据的正确性(包头、校验和等)
 * - 处理完数据后一定要重新启动DMA
 * - 缓冲区大小要根据实际的数据包大小设定
 */

// OpenMV数据处理相关变量定义（完全移植到interrupt.c，不再依赖UART）
int openmv_arr[8] = {0};
uint8_t OpenMv_status = 0;
int16_t OpenMv_X = 0;
int16_t OpenMv_Y = 0;
volatile uint8_t openmv_packet_count = 0;

// OpenMV数据处理函数（完全独立，不依赖任何UART模块）
void process_openmv_data(uint8_t data)
{
    // 调试输出：显示当前接收状态和数据
    uart0_printf("接收[%d]: 0x%02X\r\n", openmv_packet_count, data);
    
    // 将接收到的数据存储到缓冲区
    switch (openmv_packet_count)
    {
    case 0://等待第一个包头
        if(data == 0xEE)
        {
            openmv_arr[0] = data;
            openmv_packet_count++;
        }
        else
        {
            openmv_packet_count = 0;
        }
        break;
    case 1://等待第二个包头
        if(data == 0xEE)
        {
            openmv_arr[1] = data;
            openmv_packet_count++;
        }
        else
        {
            openmv_packet_count = 0;
        }
        break;
    case 2://等待状态值
        openmv_arr[2] = data;
        openmv_packet_count++;
        break;
    case 3://等待X坐标标志位
        openmv_arr[3] = data;
        openmv_packet_count++;
        break;
    case 4://等待X坐标值
        openmv_arr[4] = data;
        openmv_packet_count++;
        break;
    case 5://等待Y坐标标志位
        openmv_arr[5] = data;
        openmv_packet_count++;
        break;
    case 6://等待Y坐标值
        openmv_arr[6] = data;
        openmv_packet_count++;
        break;
    case 7://计算校验和
        {
            uint8_t calculated_checksum = (openmv_arr[2] + openmv_arr[3] + openmv_arr[4] + openmv_arr[5] + openmv_arr[6]) % 256;
            
            // 调试输出：显示接收到的完整数据包
            uart0_printf("收到数据包: %02X %02X %02X %02X %02X %02X %02X %02X\r\n",
                        openmv_arr[0], openmv_arr[1], openmv_arr[2], openmv_arr[3],
                        openmv_arr[4], openmv_arr[5], openmv_arr[6], data);
            
            uart0_printf("计算校验和: %02X, 接收校验和: %02X\r\n", calculated_checksum, data);
            
            if(data == calculated_checksum)
            {
                OpenMv_status = openmv_arr[2];
                // 根据标志位直接计算带符号的坐标值
                if(openmv_arr[3] == 0) {
                    OpenMv_X = (int16_t)openmv_arr[4];  // X坐标为正
                } else {
                    OpenMv_X = -(int16_t)openmv_arr[4]; // X坐标为负
                }
                
                if(openmv_arr[5] == 0) {
                    OpenMv_Y = (int16_t)openmv_arr[6];  // Y坐标为正
                } else {
                    OpenMv_Y = -(int16_t)openmv_arr[6]; // Y坐标为负
                }
                
                // 调试输出：显示解析结果
                uart0_printf("数据解析成功! 状态:%d, X:%d, Y:%d\r\n", OpenMv_status, OpenMv_X, OpenMv_Y);
            }
            else
            {
                uart0_printf("校验和错误!\r\n");
            }
        }
        openmv_packet_count = 0;//到了这一步，无论对错，都重置计数器
        break;
    }
}
