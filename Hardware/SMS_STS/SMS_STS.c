#include "SMS_STS.h"
#include "../Serial/Serial.h" // 添加缺少的串口头文件
#include <stdlib.h> // 添加stdlib.h以提供abs()函数声明

/******************************************************************************
 * SMS_STS舵机驱动使用教程 - 必读！
 * -------------------------------------------------------------------------
 * 第1步：初始化
 * 在主函数中调用 SMS_STS_Init(); 函数来初始化所有舵机为位置控制模式
 * 
 * 第2步：修改串口发送函数
 * 如果你使用不同的串口或发送函数，请修改下面的宏定义：
 * #define Serial_SendArray(buf, x) uart0_SendArray(buf, x)
 * 改为你自己的串口发送函数，例如：
 * #define Serial_SendArray(buf, x) uart1_SendArray(buf, x)
 * 
 * 第3步：串口接收配置
 * 方法A - 普通中断接收：
 *   在串口接收中断中添加：SMS_STS_Receive(接收到的字节);
 *   示例：
 *   void UART0_IRQHandler(void) {
 *       uint8_t data = UART0->RXD; // 获取接收到的数据
 *       SMS_STS_Receive(data);     // 传给舵机处理函数
 *   }
 * 
 * 方法B - DMA接收：
 *   在DMA传输完成中断中调用：SMS_STS_ReceiveArray(DMA接收缓冲区, 接收长度);
 *   示例：
 *   void DMA_IRQHandler(void) {
 *       SMS_STS_ReceiveArray(dma_buffer, DMA_BUFFER_SIZE);
 *   }
 *
 * 第4步：超时处理
 *   在定时器中断(建议10ms)中添加：SMS_STS_Timeout_Handler();
 *   (或者说滴答定时器)
 * 注意事项：
 * 1. 发送和接收不能同时进行，舵机只有一条数据线
 * 2. 舵机ID范围是1-4，如需更多舵机，修改SMS_STS.h中的SMS_STS_MAX_SERVOS
 * 3. 所有控制函数都会返回错误码，可以通过检查返回值判断操作是否成功
 ******************************************************************************/

// 发送数组函数
#define Serial_SendArray(buf, x) uart0_SendArray(buf, x)

// 舵机指令宏定义
#define SMS_STS_CMD_WRITE 0x03 // 写指令
#define SMS_STS_CMD_READ  0x02 // 读指令

// 舵机寄存器地址宏定义
#define SMS_STS_REG_GOAL_POSITION    0x2A // 目标位置
#define SMS_STS_REG_PRESENT_POSITION 0x38 // 当前位置
#define SMS_STS_REG_MODE             0x21 // 运行模式寄存器
#define SMS_STS_REG_SPEED            0x2E // 速度寄存器

// 舵机数据包常量
#define SMS_STS_FRAME_HEADER 0xFF // 帧头

// 配置宏
// #define SMS_STS_MAX_SERVOS  4  // 支持的最大舵机数量 - 已在头文件中定义
#define SMS_STS_RX_BUF_SIZE 10 // 接收缓冲区大小, 读位置指令返回8字节
#define SMS_STS_TX_BUF_SIZE 13 // 发送缓冲区大小, 写位置指令需要13字节

// 超时配置（毫秒）
#define SMS_STS_TIMEOUT     10 // 通信超时时间

// 舵机模式定义
#define SMS_STS_MODE_POSITION 0 // 位置控制模式
#define SMS_STS_MODE_SPEED    1 // 恒速模式(轮式)
#define SMS_STS_MODE_PWM      2 // PWM输入控制模式
#define SMS_STS_MODE_STEP     3 // 步进模式(位置闭环)

// 多圈角度范围限制 (±7.5圈，用户提到最多支持7圈半)
#define SMS_STS_MIN_ANGLE    -2700.0f  // 最小角度(-7.5圈)
#define SMS_STS_MAX_ANGLE    2700.0f   // 最大角度(+7.5圈)

// 速度范围限制 (用户指定0是使用内部默认值)
#define SMS_STS_MIN_SPEED    0      // 最小速度（使用舵机默认值）
#define SMS_STS_MAX_SPEED    3400   // 最大速度

// 错误代码定义已移至SMS_STS.h文件中

// 当前数据位状态标志位
// 0 数据线空闲
// 1 标识在发数据
// 2 标识准备收数据
// 注意：舵机只有一条数据线，不可以同时发和收数据
static volatile uint8_t SMS_STS_Read_Flag = 0; // 添加volatile确保线程安全

// 超时计数器
static volatile uint32_t SMS_STS_Timeout_Counter = 0;

// 全局变量
SMS_STS_Data_t STS_Data[SMS_STS_MAX_SERVOS + 1]; // 舵机数据存储, 数组大小加1，方便使用ID直接索引
static uint8_t sms_tx_buf[SMS_STS_TX_BUF_SIZE];  // 发送缓冲区
static uint8_t sms_rx_buf[SMS_STS_RX_BUF_SIZE];  // 接收缓冲区

/****************************************************************
 * 函数名称: SMS_STS_Init
 * 函数功能: 初始化所有舵机及通信状态
 * 输入参数: 无
 * 返回值: 无
 * 使用说明: 
 *     1. 在主函数开始时调用一次
 *     2. 会自动将所有舵机设置为位置模式
 *     3. 初始化内部数据结构和通信状态标志
 ****************************************************************/
void SMS_STS_Init(void)
{
    // 初始化舵机数据结构
    for (int i = 0; i <= SMS_STS_MAX_SERVOS; i++) {
        STS_Data[i].ID = i;
        STS_Data[i].Position = 0;
    }
    
    // 确保标志位初始化为空闲状态
    SMS_STS_Read_Flag = 0;
    SMS_STS_Timeout_Counter = 0;
    
    // 这里可以添加串口初始化，但通常系统已经初始化过串口
    // 如果需要特殊配置，可以在此处添加
    
    // 初始化所有舵机为位置模式（避免之前被设置为其他模式）
    // 为避免总线冲突，在每次设置之间添加适当延时
    for (uint8_t id = 1; id <= SMS_STS_MAX_SERVOS; id++) {
        // 设置为位置模式 (MODE_POSITION = 0)
        SMS_STS_Set_Mode(id, SMS_STS_MODE_POSITION);
        // 短暂延时，避免总线冲突
        delay_ms(1);
    }
}

/****************************************************************
 * 函数名称: SMS_STS_Is_Valid_ID
 * 函数功能: 检查舵机ID是否在有效范围内
 * 输入参数: 
 *     ID - 需要检查的舵机ID号(1-4)
 * 返回值: 
 *     1(真) - ID有效
 *     0(假) - ID无效
 * 使用说明: 内部函数，用户一般不需要直接调用
 ****************************************************************/
static uint8_t SMS_STS_Is_Valid_ID(uint8_t ID)
{
    return (ID > 0 && ID <= SMS_STS_MAX_SERVOS);
}

/****************************************************************
 * 函数名称: SMS_STS_Run
 * 函数功能: 控制舵机转动到指定位置
 * 输入参数: 
 *     ID - 舵机ID号(1-4)
 *     Position - 目标位置值(0-4095)
 *     RunTime - 运行时间(0-65535)，单位为毫秒
 *     Speed - 运行速度(0-65535)，0表示使用默认值
 * 返回值: 
 *     SMS_STS_OK - 操作成功
 *     SMS_STS_ERR_PARAM - 参数错误(ID无效)
 *     SMS_STS_ERR_BUSY - 总线忙，无法发送
 * 使用说明: 
 *     1. 舵机需在位置模式下使用此函数
 *     2. Position为0-4095，对应0-360度
 *     3. 该指令发送后不需要等待应答
 ****************************************************************/
SMS_STS_Error_t SMS_STS_Run(uint8_t ID, uint16_t Position, uint16_t RunTime, uint16_t Speed)
{
    // 检查ID有效性
    if (!SMS_STS_Is_Valid_ID(ID)) {
        return SMS_STS_ERR_PARAM;
    }
    
    // 如果当前有数据在传输，则不进行发送
    if (SMS_STS_Read_Flag != 0) {
        return SMS_STS_ERR_BUSY;
    }
    // 标识当前在发送数据（该指令不需要应答）
    SMS_STS_Read_Flag = 1;

    uint8_t checksum = 0; // 校验和

    // 帧头
    sms_tx_buf[0] = SMS_STS_FRAME_HEADER;
    sms_tx_buf[1] = SMS_STS_FRAME_HEADER;
    // 舵机ID
    sms_tx_buf[2] = ID;
    // 数据长度
    sms_tx_buf[3] = 0x09;
    // 写指令
    sms_tx_buf[4] = SMS_STS_CMD_WRITE;
    // 起始地址（位置寄存器地址）
    sms_tx_buf[5] = SMS_STS_REG_GOAL_POSITION;
    // 位置
    sms_tx_buf[6] = Position & 0xFF;        // 低字节
    sms_tx_buf[7] = (Position >> 8) & 0xFF; // 高字节
    // 运行时间
    sms_tx_buf[8] = RunTime & 0xFF;         // 低字节
    sms_tx_buf[9] = (RunTime >> 8) & 0xFF;  // 高字节
    // 运行速度
    sms_tx_buf[10] = Speed & 0xFF;          // 低字节
    sms_tx_buf[11] = (Speed >> 8) & 0xFF;   // 高字节

    // 计算校验和
    for (int i = 2; i < 12; i++) {
        checksum += sms_tx_buf[i];
    }
    sms_tx_buf[12] = ~checksum; // 取反

    // 发送数据
    Serial_SendArray(sms_tx_buf, 13);

    // 数据发送完毕，标识当前数据线空闲
    SMS_STS_Read_Flag = 0;
    
    return SMS_STS_OK;
}

/****************************************************************
 * 函数名称: SMS_STS_Read
 * 函数功能: 发送读取舵机当前位置的请求
 * 输入参数: 
 *     ID - 需要读取的舵机ID号(1-4)
 * 返回值: 
 *     SMS_STS_OK - 请求发送成功
 *     SMS_STS_ERR_PARAM - 参数错误(ID无效)
 *     SMS_STS_ERR_BUSY - 总线忙，无法发送
 * 使用说明: 
 *     1. 调用此函数后，舵机会返回数据
 *     2. 返回的数据会在接收中断中处理
 *     3. 处理后的位置值会更新到STS_Data数组中
 *     4. 如果长时间没有应答，系统会触发超时处理
 ****************************************************************/
SMS_STS_Error_t SMS_STS_Read(uint8_t ID)
{
    // 检查ID有效性
    if (!SMS_STS_Is_Valid_ID(ID)) {
        return SMS_STS_ERR_PARAM;
    }
    
    // 如果当前有数据在传输，则不进行发送
    if (SMS_STS_Read_Flag != 0) {
        return SMS_STS_ERR_BUSY;
    }

    // 标识准备接收数据
    SMS_STS_Read_Flag = 2;

    uint8_t checksum = 0; // 校验和

    // 帧头
    sms_tx_buf[0] = SMS_STS_FRAME_HEADER;
    sms_tx_buf[1] = SMS_STS_FRAME_HEADER;
    // 舵机ID
    sms_tx_buf[2] = ID;
    // 数据长度
    sms_tx_buf[3] = 0x04;
    // 读指令
    sms_tx_buf[4] = SMS_STS_CMD_READ;
    // 起始地址（位置寄存器地址）
    sms_tx_buf[5] = SMS_STS_REG_PRESENT_POSITION;
    // 数据长度
    sms_tx_buf[6] = 0x02;

    // 计算校验和
    for (int i = 2; i < 7; i++) {
        checksum += sms_tx_buf[i];
    }
    sms_tx_buf[7] = ~checksum; // 取反

    // 发送数据
    Serial_SendArray(sms_tx_buf, 8);

    // 发送读取指令后，等待舵机应答，不清除 SMS_STS_Read_Flag
    // 启动超时计数
    SMS_STS_Timeout_Counter = SMS_STS_TIMEOUT;
    
    return SMS_STS_OK;
}

/****************************************************************
 * 函数名称: SMS_STS_Set_Angle
 * 函数功能: 通过角度值控制舵机转动
 * 输入参数: 
 *     ID - 舵机ID号(1-4)
 *     Angle - 目标角度，单位为度，范围(-2700.0到+2700.0)
 *             负数表示反方向，支持多圈(±7.5圈)
 *     RunTime - 运行时间(0-65535)，单位为毫秒
 *     Speed - 运行速度(0-65535)，0表示使用默认值
 * 返回值: 
 *     SMS_STS_OK - 操作成功
 *     SMS_STS_ERR_PARAM - 参数错误(ID无效或角度超范围)
 *     SMS_STS_ERR_BUSY - 总线忙，无法发送
 * 使用说明: 
 *     1. 该函数比SMS_STS_Run更直观，直接使用角度值
 *     2. 内部会自动将角度转换为位置值(0-4095)
 *     3. 支持多圈转动，最多±7.5圈
 ****************************************************************/
SMS_STS_Error_t SMS_STS_Set_Angle(uint8_t ID, float Angle, uint16_t RunTime, uint16_t Speed)
{
    // 检查ID有效性
    if (!SMS_STS_Is_Valid_ID(ID)) {
        return SMS_STS_ERR_PARAM;
    }
    
    // 多圈角度范围检查
    if (Angle < SMS_STS_MIN_ANGLE || Angle > SMS_STS_MAX_ANGLE) {
        return SMS_STS_ERR_PARAM;
    }
    
    // 角度转换为位置值
    uint16_t Position = (uint16_t)(Angle * 11.375f);
    
    // 调用原有的运行函数
    return SMS_STS_Run(ID, Position, RunTime, Speed);
}

/****************************************************************
 * 函数名称: SMS_STS_Receive
 * 函数功能: 处理接收到的串口数据
 * 输入参数: 
 *     data - 串口接收到的一个字节数据
 * 返回值: 无
 * 使用说明: 
 *     1. 该函数应放置在串口接收中断中调用
 *     2. 每收到一个字节就调用一次该函数
 *     3. 函数内部会自动处理数据包的识别和解析
 *     4. 解析成功后会更新对应舵机的位置信息
 *     5. 切勿在该函数中加入串口发送代码，会导致通信阻塞
 ****************************************************************/
uint8_t sms_rx_count = 0;
static uint8_t last_byte = 0; // 添加变量记录上一个字节

void SMS_STS_Receive(uint8_t data)
{
    // ------------ 关于DMA接收数据可能出现的错位问题详细解释 ------------
    // 问题背景：当使用DMA接收舵机返回数据时可能出现数据错位现象
    // 1. 当我们发送转动请求后，舵机会先返回一串确认数据
    // 2. 随后我们可能会发送读取位置请求，此时也会有数据返回
    // 3. 如果使用DMA接收，这两部分数据可能会连续存入缓冲区
    // 4. 当我们处理缓冲区数据时，第一部分数据(前7字节)会被错误地当作读取位置的响应进行解析
    // 5. 这会导致真正的位置数据(后8字节)被错位解析，从而得到错误结果
    // 
    // 解决方案：检测到连续的两个帧头(0xFF)时，重置接收计数器到第二个帧头位置
    // 这样可以确保我们始终从一个完整的数据包开始解析，避免数据错位
    // 注：普通串口中断模式下一般不会出现此问题，因为数据接收间隔较大
    if (last_byte == SMS_STS_FRAME_HEADER && data == SMS_STS_FRAME_HEADER && sms_rx_count > 1) {
        sms_rx_count = 1; // 重置计数器
        // 切记不要重置成0，否则会丢失第一个字节
    }
    
    // 记录当前字节，供下次调用使用
    last_byte = data;
    
    // 如果当前不是在等待接收数据的状态，直接返回
    if (SMS_STS_Read_Flag != 2) {
        return;
    }
    
    // 缓冲区溢出检查
    if (sms_rx_count >= SMS_STS_RX_BUF_SIZE) {
        sms_rx_count = 0;
        SMS_STS_Read_Flag = 0; // 重置标志位
        return;
    }

    // 状态机处理接收数据
    switch (sms_rx_count) {
        case 0:
            if (data == SMS_STS_FRAME_HEADER) {
                sms_rx_buf[sms_rx_count++] = data;
            } else {
                sms_rx_count = 0;
            }
            break;
            
        case 1:
            if (data == SMS_STS_FRAME_HEADER) {
                sms_rx_buf[sms_rx_count++] = data;
            } else {
                sms_rx_count = 0;
                SMS_STS_Read_Flag = 0; // 帧头错误，复位
            }
            break;
            
        case 2: // ID
            sms_rx_buf[sms_rx_count++] = data;
            break;
            
        case 3: // Length
            sms_rx_buf[sms_rx_count++] = data;
            break;
            
        case 4: // Status
            sms_rx_buf[sms_rx_count++] = data;
            break;
            
        case 5: // Position L
            sms_rx_buf[sms_rx_count++] = data;
            break;
            
        case 6: // Position H
            sms_rx_buf[sms_rx_count++] = data;
            break;
            
        case 7: // Checksum
            sms_rx_buf[sms_rx_count] = data;
            uint8_t checksum = 0;
            
            // 计算校验和: Check Sum = ~ (ID + Length + Status + Param1 + ... + Param N)
            for (int i = 2; i < 7; i++) {
                checksum += sms_rx_buf[i];
            }
            

            // 验证校验和
            if (sms_rx_buf[7] == (uint8_t)(~checksum)) {
                uint8_t servo_id = sms_rx_buf[2];
                // 检查舵机ID是否在有效范围内
                if (SMS_STS_Is_Valid_ID(servo_id)) {
                    STS_Data[servo_id].Position = ((uint16_t)sms_rx_buf[6] << 8) | sms_rx_buf[5];
                }
            }
            
            // 一次接收完成，复位状态机和标志位
            sms_rx_count = 0;
            SMS_STS_Read_Flag = 0;
            // 重置超时计数器
            SMS_STS_Timeout_Counter = 0;
            break;

        default:
            // 异常情况，复位状态机
            sms_rx_count = 0;
            SMS_STS_Read_Flag = 0;
            break;
    }
}

/****************************************************************
 * 函数名称: SMS_STS_Clear_Circle
 * 函数功能: 清除舵机内部记录的多圈角度信息
 * 输入参数: 
 *     ID - 舵机ID号(1-4)
 * 返回值: 
 *     SMS_STS_OK - 操作成功
 *     SMS_STS_ERR_PARAM - 参数错误(ID无效)
 *     SMS_STS_ERR_BUSY - 总线忙，无法发送
 * 使用说明: 
 *     1. 用于多圈角度模式下，将累计角度清零
 *     2. 清零后舵机当前位置会被认为是0度位置
 *     3. 该指令发送后不需要等待应答
 ****************************************************************/
SMS_STS_Error_t SMS_STS_Clear_Circle(uint8_t ID)
{
    // 检查ID有效性
    if (!SMS_STS_Is_Valid_ID(ID)) {
        return SMS_STS_ERR_PARAM;
    }
    
    // 步骤1：检查总线是否空闲
    // 如果标志位不为0，说明总线正在被占用（正在发送或等待接收），此时直接退出，避免数据冲突。
    if (SMS_STS_Read_Flag != 0) {
        return SMS_STS_ERR_BUSY;
    }
    // 步骤2：占用总线
    // 将标志位设为1，表示"我"要开始发送数据了。因为这个指令舵机不会返回信息，所以发完就可以释放总线。
    SMS_STS_Read_Flag = 1;

    // 步骤3：准备一个变量，用来累加校验和
    uint8_t checksum = 0; 

    // --- 步骤4：开始组装要发送的数据包（也叫"指令帧"） ---
    // 第1个字节: 固定的帧头，必须是0xFF
    sms_tx_buf[0] = SMS_STS_FRAME_HEADER;
    // 第2个字节: 固定的帧头，必须是0xFF
    sms_tx_buf[1] = SMS_STS_FRAME_HEADER;
    // 第3个字节: 舵机的ID号，告诉舵机总线上哪个ID的舵机来听这条指令
    sms_tx_buf[2] = ID;
    // 第4个字节: 数据包的"长度"，指的是从下一个字节（指令）到校验和之前的所有字节的总数。这里是2（指令+ID）。
    sms_tx_buf[3] = 0x02;
    // 第5个字节: 指令代码。0x0A 是"清除多圈角度"的专用指令。
    sms_tx_buf[4] = 0x0A; 

    // --- 步骤5：计算校验和 ---
    // 校验和的计算方法是：从第3个字节(ID)开始，一直加到最后一个参数字节。
    for (int i = 2; i < 5; i++) {
        checksum += sms_tx_buf[i];
    }
    // 最后对计算出的总和按位取反，这就是最终的校验和。
    sms_tx_buf[5] = ~checksum; 

    // --- 步骤6：通过串口把组装好的数据包发送出去 ---
    // 参数1: 要发送的数据数组 (sms_tx_buf)
    // 参数2: 数据包的总长度 (这里是6个字节)
    Serial_SendArray(sms_tx_buf, 6);

    // --- 步骤7：释放总线 ---
    // 数据发送完毕，将总线标志位清零，让其他操作可以继续。
    SMS_STS_Read_Flag = 0;
    
    return SMS_STS_OK;
}

/****************************************************************
 * 函数名称: SMS_STS_Set_Mode
 * 函数功能: 设置舵机的工作模式
 * 输入参数: 
 *     ID - 舵机ID号(1-4)
 *     Mode - 工作模式:
 *            0 = 位置控制模式(默认)
 *            1 = 恒速模式(轮式)
 *            2 = PWM输入控制模式
 *            3 = 步进模式(位置闭环)
 * 返回值: 
 *     SMS_STS_OK - 操作成功
 *     SMS_STS_ERR_PARAM - 参数错误(ID无效或Mode无效)
 *     SMS_STS_ERR_BUSY - 总线忙，无法发送
 * 使用说明: 
 *     1. 默认模式为位置控制模式(0)
 *     2. 改变模式后，不同模式下使用不同的控制函数
 *     3. 位置模式下用SMS_STS_Run或SMS_STS_Set_Angle
 *     4. 恒速模式下用SMS_STS_Set_Speed
 ****************************************************************/
SMS_STS_Error_t SMS_STS_Set_Mode(uint8_t ID, uint8_t Mode)
{
    // 检查ID有效性
    if (!SMS_STS_Is_Valid_ID(ID)) {
        return SMS_STS_ERR_PARAM;
    }
    
    // 检查模式参数有效性
    if (Mode > SMS_STS_MODE_STEP) {
        return SMS_STS_ERR_PARAM;
    }
    
    // 步骤1：检查总线是否空闲
    if (SMS_STS_Read_Flag != 0) {
        return SMS_STS_ERR_BUSY;
    }
    // 步骤2：占用总线
    SMS_STS_Read_Flag = 1;

    // 步骤3：准备校验和变量
    uint8_t checksum = 0; 

    // --- 步骤4：开始组装数据包 ---
    // 第1、2字节: 固定的帧头 (0xFF 0xFF)
    sms_tx_buf[0] = SMS_STS_FRAME_HEADER;
    sms_tx_buf[1] = SMS_STS_FRAME_HEADER;
    // 第3字节: 舵机的ID号
    sms_tx_buf[2] = ID;
    // 第4字节: 数据包的长度，指从ID到最后一个参数的字节数。这里是4 (ID + 指令 + 地址 + 参数)
    sms_tx_buf[3] = 0x04; 
    // 第5字节: 指令代码 (0x03代表"写"操作)
    sms_tx_buf[4] = SMS_STS_CMD_WRITE;
    // 第6字节: 要写入的寄存器地址 (舵机内部地址0x21代表"运行模式"寄存器)
    sms_tx_buf[5] = SMS_STS_REG_MODE;
    // 第7字节: 要写入的参数，也就是模式值 (0, 1, 2, 或 3)
    sms_tx_buf[6] = Mode;

    // --- 步骤5：计算校验和 ---
    // 计算范围是从第3个字节(ID)到最后一个参数字节(Mode)
    for (int i = 2; i < 7; i++) {
        checksum += sms_tx_buf[i];
    }
    // 对总和按位取反
    sms_tx_buf[7] = ~checksum; 

    // --- 步骤6：通过串口发送数据包 ---
    // 参数1: 数据数组
    // 参数2: 数据包总长度 (8字节)
    Serial_SendArray(sms_tx_buf, 8);

    // --- 步骤7：释放总线 ---
    SMS_STS_Read_Flag = 0;
    
    return SMS_STS_OK;
}

/****************************************************************
 * 函数名称: SMS_STS_Set_Speed
 * 函数功能: 在恒速模式下控制舵机的转动方向和速度
 * 输入参数: 
 *     ID - 舵机ID号(1-4)
 *     Speed - 速度值:
 *             0 = 停止
 *             正数(50~3400) = 顺时针转动
 *             负数(-50~-3400) = 逆时针转动
 * 返回值: 
 *     SMS_STS_OK - 操作成功
 *     SMS_STS_ERR_PARAM - 参数错误(ID无效或Speed超出范围)
 *     SMS_STS_ERR_BUSY - 总线忙，无法发送
 * 使用说明: 
 *     1. 使用前必须先将舵机设为恒速模式(Mode=1)
 *     2. 速度范围: ±50~±3400
 *     3. 数值越大，转速越快
 *     4. 负数表示反方向转动
 *     5. 0表示停止转动
 ****************************************************************/
SMS_STS_Error_t SMS_STS_Set_Speed(uint8_t ID, int16_t Speed)
{
    // 检查ID有效性
    if (!SMS_STS_Is_Valid_ID(ID)) {
        return SMS_STS_ERR_PARAM;
    }
    
    // 速度范围检查，支持0和±50~±3400
    // 0是使用默认值，其他要在范围内
    if (Speed != 0 && (abs(Speed) > SMS_STS_MAX_SPEED)) {
        return SMS_STS_ERR_PARAM;
    }
    
    // 步骤1：检查总线是否空闲
    if (SMS_STS_Read_Flag != 0) {
        return SMS_STS_ERR_BUSY;
    }
    // 步骤2：占用总线
    SMS_STS_Read_Flag = 1;

    // 步骤3：准备校验和变量和最终速度变量
    uint8_t checksum = 0;
    uint16_t final_speed = 0; // 使用一个无符号16位变量来存储最终要发送到舵机的值

    // --- 核心逻辑：根据传入的Speed值，计算出符合协议的final_speed ---
    if (Speed > 0) {
        // 如果Speed是正数，直接作为顺时针转的速度
        final_speed = Speed;
    } else if (Speed < 0) {
        // 如果Speed是负数，根据协议转换为反转速度 (速度绝对值 + 32768)
        final_speed = (uint16_t)(-Speed + 32768); 
    }
    // 如果 Speed 等于 0，final_speed 会保持初始值 0，即停止

    // --- 步骤4：开始组装数据包 ---
    // 第1、2字节: 固定的帧头
    sms_tx_buf[0] = SMS_STS_FRAME_HEADER;
    sms_tx_buf[1] = SMS_STS_FRAME_HEADER;
    // 第3字节: 舵机ID
    sms_tx_buf[2] = ID;
    // 第4字节: 数据包的长度。这里是5 (ID + 指令 + 地址 + 速度低字节 + 速度高字节)
    sms_tx_buf[3] = 0x05; 
    // 第5字节: 指令代码 (0x03代表"写"操作)
    sms_tx_buf[4] = SMS_STS_CMD_WRITE;
    // 第6字节: 要写入的寄存器地址 (舵机内部地址0x2E代表"目标速度"寄存器)
    sms_tx_buf[5] = SMS_STS_REG_SPEED;
    
    // 第7、8字节: 16位的速度值参数，需要拆成两个8位的字节来发送
    // 第7字节: 发送最终速度值的低8位
    sms_tx_buf[6] = final_speed & 0xFF;
    // 第8字节: 发送最终速度值的高8位
    sms_tx_buf[7] = (final_speed >> 8) & 0xFF;

    // --- 步骤5：计算校验和 ---
    // 计算范围是从第3个字节(ID)到最后一个参数字节(速度高字节)
    for (int i = 2; i < 8; i++) {
        checksum += sms_tx_buf[i];
    }
    // 对总和按位取反
    sms_tx_buf[8] = ~checksum; 

    // --- 步骤6：通过串口发送数据包 ---
    // 参数1: 数据数组
    // 参数2: 数据包总长度 (9字节)
    Serial_SendArray(sms_tx_buf, 9);

    // --- 步骤7：释放总线 ---
    SMS_STS_Read_Flag = 0;
    
    return SMS_STS_OK;
}

/****************************************************************
 * 函数名称: SMS_STS_ReceiveArray
 * 函数功能: 批量处理DMA接收到的数据数组
 * 输入参数: 
 *     data - 数据数组指针
 *     length - 数组长度
 * 返回值: 无
 * 使用说明: 
 *     1. 该函数用于DMA接收模式
 *     2. 在DMA传输完成中断中调用
 *     3. 将传入整个DMA接收缓冲区和长度
 *     4. 函数内部会逐字节调用SMS_STS_Receive处理
 ****************************************************************/
void SMS_STS_ReceiveArray(uint8_t *data, uint16_t length)
{
    // 参数检查
    if (data == NULL || length == 0) {
        return;
    }
    
    for (uint16_t i = 0; i < length; ++i) {
        SMS_STS_Receive(data[i]);
    }
}

/****************************************************************
 * 函数名称: SMS_STS_Timeout_Handler
 * 函数功能: 处理舵机通信超时情况
 * 输入参数: 无
 * 返回值: 无
 * 使用说明: 
 *     1. 该函数应放在定时器中断中调用(建议10ms周期)
 *     2. 用于检测读取操作是否超时
 *     3. 如果超时，会自动重置通信状态
 *     4. 避免由于通信故障导致系统卡死
 ****************************************************************/
void SMS_STS_Timeout_Handler(void)
{
    if (SMS_STS_Read_Flag == 2 && SMS_STS_Timeout_Counter > 0) {
        // 10ms一次，所以直接减1
        SMS_STS_Timeout_Counter--;
        if (SMS_STS_Timeout_Counter == 0) {
            sms_rx_count = 0;
            // 超时，复位状态
            SMS_STS_Read_Flag = 0;
        }
    }
}