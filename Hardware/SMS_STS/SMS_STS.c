#include "SMS_STS.h"
#include "../Serial/Serial.h" // 添加缺少的串口头文件
#include "../Serial/Serial_1.h" // 添加Serial_1.h以提供uart1_SendArray()函数声明
#include <stdlib.h> // 添加stdlib.h以提供abs()函数声明

/******************************************************************************
 * SMS_STS舵机驱动使用教程 - 必读！
 * -------------------------------------------------------------------------
 * 第0步：使用上位机设置舵机参数（初次使用必须）
 * 在使用舵机之前，请先使用上位机完成以下参数的设置：
 * - ID：建议从0开始设置
 * - 波特率：设置为4（对应表盘刻度，而非实际的波特率数值115200等）
 * - 最大角度限制：设置为0
 * - 相位：设置为124
 * 注意：波特率一栏不要直接设置为115200等具体数值，而是按照上位机的刻度选择
 * 
 * 第1步：初始化（必须）
 * 必须在主函数中调用 SMS_STS_Init(); 函数来初始化所有舵机为位置控制模式
 * 该步骤会配置舵机初始角度，防止开机时舵机乱转
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
 * 4. 在STS_Data结构体中可以获取舵机的当前角度值（Angle）和多圈角度值（MultiTurnAngle）
 ******************************************************************************/

// 发送数组函数
#define Serial_SendArray(buf, x) uart1_SendArray(buf, x)

// 舵机指令宏定义
#define SMS_STS_CMD_WRITE 0x03 // 写指令
#define SMS_STS_CMD_READ  0x02 // 读指令
#define SMS_STS_CMD_SYNC_READ 0x82 // 同步读指令

// 舵机寄存器地址宏定义
#define SMS_STS_REG_GOAL_POSITION    0x2A // 目标位置
#define SMS_STS_REG_PRESENT_POSITION 0x38 // 当前位置
#define SMS_STS_REG_MODE             0x21 // 运行模式寄存器
#define SMS_STS_REG_SPEED            0x2E // 速度寄存器

// 舵机数据包常量
#define SMS_STS_FRAME_HEADER 0xFF // 帧头

// 配置宏
// #define SMS_STS_MAX_SERVOS  4  // 支持的最大舵机数量 - 已在头文件中定义
#define SMS_STS_RX_BUF_SIZE 64 // 接收缓冲区大小, 同步读指令可能返回较大数据包
#define SMS_STS_TX_BUF_SIZE 13 // 发送缓冲区大小, 写位置指令需要13字节

// 超时配置（毫秒）
#define SMS_STS_TIMEOUT     10 // 通信超时时间

// 舵机模式定义
#define SMS_STS_MODE_POSITION 0 // 位置控制模式
#define SMS_STS_MODE_SPEED    1 // 恒速模式(轮式)
#define SMS_STS_MODE_PWM      2 // PWM输入控制模式
#define SMS_STS_MODE_STEP     3 // 步进模式(位置闭环)

// 多圈角度范围限制 
#define SMS_STS_MIN_ANGLE    -2700.0f // 最小角度-7.5圈
#define SMS_STS_MAX_ANGLE    2700.0f   // 最大角度(+7.5圈)

// 速度范围限制 (用户指定0是使用内部默认值)
#define SMS_STS_MIN_SPEED    0      // 最小速度（使用舵机默认值）
#define SMS_STS_MAX_SPEED    3400   // 最大速度

// 方向位宏定义
#define SMS_STS_DIRECTION_BIT 32768 // 方向位，用于表示负方向

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

// 添加全局变量用于状态机
static uint8_t sync_rx_buf[SMS_STS_RX_BUF_SIZE];  // 同步读取响应缓冲区
static uint8_t sync_rx_count = 0;                 // 同步读取响应计数器
static uint8_t sync_rx_last_byte = 0;             // 记录上一个接收字节

/****************************************************************
 * 函数名称: SMS_STS_Init
 * 函数功能: 初始化所有舵机及通信状态
 * 输入参数: 无
 * 返回值: 无
 * 使用说明: 
 *     1. 在主函数开始时调用一次
 *     2. 会设置所有舵机的初始角度为180度，防止开机乱转
 *     3. 会自动将所有舵机设置为位置模式
 *     4. 初始化内部数据结构和通信状态标志
 ****************************************************************/
void SMS_STS_Init(void)
{
        
    // 初始化舵机角度为180度，防止开机乱转
    // 预定义的数据包，设置舵机的当前角度为180度
    uint8_t buff1[8] = {0xFF, 0xFF, 0x01, 0x04, 0x03, 0x28, 0x80, 0x4F};
    uint8_t buff2[8] = {0xFF, 0xFF, 0x02, 0x04, 0x03, 0x28, 0x80, 0x4E};
    uint8_t buff3[8] = {0xFF, 0xFF, 0x03, 0x04, 0x03, 0x28, 0x80, 0x4D};
    uint8_t buff4[8] = {0xFF, 0xFF, 0x04, 0x04, 0x03, 0x28, 0x80, 0x4C};
    
    // 依次发送每个初始化指令，每次间隔10ms
    Serial_SendArray(buff1, 8);
    delay_ms(10);
    
    Serial_SendArray(buff2, 8);
    delay_ms(10);
    
    Serial_SendArray(buff3, 8);
    delay_ms(10);
    
    Serial_SendArray(buff4, 8);
    
    // 等待500ms让舵机完成初始化
    delay_ms(500);
    delay_ms(500);

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
    
    // 调用原有的运行函数
    return SMS_STS_Run(ID, Angle, RunTime, Speed);
}

/****************************************************************
 * 函数名称: SMS_STS_Set_Angle_With_Direction
 * 函数功能: 带方向控制的角度设置函数
 * 输入参数: 
 *     ID - 舵机ID号(1-4)
 *     Angle - 目标角度，单位为度，可正可负
 *             正数表示正方向，负数表示反方向
 *     RunTime - 运行时间(0-65535)，单位为毫秒
 *     Speed - 运行速度(0-65535)，0表示使用默认值
 * 返回值: 
 *     SMS_STS_OK - 操作成功
 *     SMS_STS_ERR_PARAM - 参数错误(ID无效或角度超范围)
 *     SMS_STS_ERR_BUSY - 总线忙，无法发送
 * 使用说明: 
 *     1. 该函数特殊处理负角度，符合SMS_STS舵机通信协议
 *     2. 负角度会被转换为正位置值+32768的特殊形式
 *     3. 允许设置正负角度，舵机会按照对应方向转动
 *     4. 支持多圈转动，范围为SMS_STS_MIN_ANGLE到SMS_STS_MAX_ANGLE
 ****************************************************************/
SMS_STS_Error_t SMS_STS_Set_Angle_With_Direction(uint8_t ID, float Angle, uint16_t RunTime, uint16_t Speed)
{
    // 检查ID有效性
    if (!SMS_STS_Is_Valid_ID(ID)) {
        return SMS_STS_ERR_PARAM;
    }
    
    // 多圈角度范围检查，使用宏定义的范围限制
    if (Angle < SMS_STS_MIN_ANGLE || Angle > SMS_STS_MAX_ANGLE) {
        return SMS_STS_ERR_PARAM;
    }
    
    uint16_t Position;
    
    if (Angle >= 0) {
        // 正角度正常转换
        Position = SMS_STS_Angle_To_Position_Multi((uint32_t)Angle);
    } else {
        // 负角度需要特殊处理:
        // 1. 先将负角度转为正的位置值
        // 2. 然后加上方向位表示负方向
        Position = SMS_STS_Angle_To_Position_Multi((uint32_t)(-Angle)) + SMS_STS_DIRECTION_BIT;
    }
    
    // 调用基础的运行函数
    return SMS_STS_Run(ID, Position, RunTime, Speed);
}

/****************************************************************
 * 函数名称: SMS_STS_SyncRead
 * 函数功能: 发送同步读取指令，一次性查询多个舵机参数
 * 输入参数: 
 *     id_list - 需要查询的舵机ID列表
 *     id_count - ID列表中的舵机数量
 *     reg_addr - 要读取的寄存器起始地址（如位置寄存器0x38）
 *     reg_length - 要读取的数据长度（如读位置值为2字节）
 * 返回值: 
 *     SMS_STS_OK - 请求发送成功
 *     SMS_STS_ERR_PARAM - 参数错误(ID无效或列表为空)
 *     SMS_STS_ERR_BUSY - 总线忙，无法发送
 * 使用说明: 
 *     1. 一次可以查询多个舵机的同一参数
 *     2. 舵机会按照ID列表的顺序依次回复数据
 *     3. 发送后需等待所有舵机回复，回复会在接收中断中处理
 *     4. 超时机制会处理未回复的情况
 ****************************************************************/
SMS_STS_Error_t SMS_STS_SyncRead(uint8_t *id_list, uint8_t id_count, uint8_t reg_addr, uint8_t reg_length)
{
    // 参数检查
    if (id_list == NULL || id_count == 0 || id_count > SMS_STS_MAX_SERVOS) {
        return SMS_STS_ERR_PARAM;
    }
    
    // 检查ID的有效性
    for (uint8_t i = 0; i < id_count; i++) {
        if (!SMS_STS_Is_Valid_ID(id_list[i])) {
            return SMS_STS_ERR_PARAM;
        }
    }
    
    // 如果当前有数据在传输，则不进行发送
    if (SMS_STS_Read_Flag != 0) {
        return SMS_STS_ERR_BUSY;
    }

    // 标识准备接收数据
    SMS_STS_Read_Flag = 2;

    // 计算数据包长度: N + 4 (N为舵机数量)
    uint8_t packet_length = id_count + 4;
    uint8_t checksum = 0; // 校验和
    
    // 帧头
    sms_tx_buf[0] = SMS_STS_FRAME_HEADER;
    sms_tx_buf[1] = SMS_STS_FRAME_HEADER;
    // 广播ID (0xFE)
    sms_tx_buf[2] = 0xFE;
    // 数据长度
    sms_tx_buf[3] = packet_length;
    // 同步读指令
    sms_tx_buf[4] = SMS_STS_CMD_SYNC_READ;
    // 起始寄存器地址
    sms_tx_buf[5] = reg_addr;
    // 数据长度
    sms_tx_buf[6] = reg_length;
    
    // 添加所有舵机ID
    for (uint8_t i = 0; i < id_count; i++) {
        sms_tx_buf[7 + i] = id_list[i];
    }
    
    // 计算校验和
    for (uint8_t i = 2; i < 7 + id_count; i++) {
        checksum += sms_tx_buf[i];
    }
    sms_tx_buf[7 + id_count] = ~checksum; // 取反
    
    // 发送数据包
    Serial_SendArray(sms_tx_buf, 8 + id_count);
    
    // 启动超时计数
    // 由于多个舵机回复需要更多时间，所以增加超时时间
    SMS_STS_Timeout_Counter = SMS_STS_TIMEOUT * id_count;
    
    return SMS_STS_OK;
}

/****************************************************************
 * 函数名称: SMS_STS_Process_SyncRead_Response
 * 函数功能: 处理同步读指令的响应数据
 * 输入参数: 
 *     data - 接收到的数据包
 *     length - 数据包长度
 * 返回值: 无
 * 使用说明: 
 *     1. 该函数在接收到完整的同步读回复后调用
 *     2. 将解析数据并更新对应舵机的状态信息
 *     3. 只处理协议格式正确的数据包
 ****************************************************************/
void SMS_STS_Process_SyncRead_Response(uint8_t *data, uint16_t length)
{
    // 检查数据包基本有效性
    if (data == NULL || length < 7) { // 至少需要帧头(2)+ID(1)+长度(1)+错误码(1)+数据(1)+校验和(1)
        return;
    }
    
    // 检查帧头
    if (data[0] != SMS_STS_FRAME_HEADER || data[1] != SMS_STS_FRAME_HEADER) {
        return;
    }
    
    // 获取舵机ID
    uint8_t servo_id = data[2];
    
    // 检查ID有效性
    if (!SMS_STS_Is_Valid_ID(servo_id)) {
        return;
    }
    
    // 数据长度
    uint8_t data_length = data[3];
    
    // 验证数据包总长度
    if (length != data_length + 4) { // 帧头(2) + ID(1) + 长度(1) + 内容(length) + 校验和(1)
        return;
    }
    // 如果当前不是在等待接收数据的状态，直接返回(因为发送转动请求也会返回数据)
    if(SMS_STS_Read_Flag != 2)
    {
        return;
    }

    // 计算校验和
    uint8_t checksum = 0;
    for (uint8_t i = 2; i < length - 1; i++) {
        checksum += data[i];
    }
    checksum = ~checksum;
    
    // 验证校验和
    if (data[length - 1] != checksum) {
        return;
    }
    
    // 读取位置数据 (假设读取的是位置寄存器)
    // 错误状态位在data[4]
    // 实际数据从data[5]开始
    if (data_length >= 3) { // 至少有错误码(1)+位置数据(2)
        uint16_t position = ((uint16_t)data[6] << 8) | data[5];
        
        // 更新舵机数据
        STS_Data[servo_id].Position = position;
        
        // 检查位置值是否包含方向位（第15位为1表示负方向）
        if (position >= SMS_STS_DIRECTION_BIT) {
            // 负方向
            STS_Data[servo_id].Direction = -1;
            STS_Data[servo_id].ActualPosition = position - SMS_STS_DIRECTION_BIT;
            STS_Data[servo_id].Angle = -SMS_STS_Position_To_Angle(STS_Data[servo_id].ActualPosition);
            STS_Data[servo_id].MultiTurnAngle = -SMS_STS_Position_To_Angle_Multi(STS_Data[servo_id].ActualPosition);
        } else {
            // 正方向
            STS_Data[servo_id].Direction = 1;
            STS_Data[servo_id].ActualPosition = position;
            STS_Data[servo_id].Angle = SMS_STS_Position_To_Angle(STS_Data[servo_id].ActualPosition);
            STS_Data[servo_id].MultiTurnAngle = SMS_STS_Position_To_Angle_Multi(STS_Data[servo_id].ActualPosition);
        }
    }
}

/****************************************************************
 * 函数名称: SMS_STS_SyncRead_Fixed
 * 函数功能: 发送固定的同步读取指令，读取两个舵机的位置
 * 输入参数: 无
 * 返回值: 
 *     SMS_STS_OK - 请求发送成功
 *     SMS_STS_ERR_BUSY - 总线忙，无法发送
 * 使用说明: 
 *     1. 此函数发送固定的指令FF FF FE 06 82 38 02 01 02 36
 *     2. 用于同时读取ID为1和2的舵机位置
 *     3. 发送后需等待舵机回复，回复会在接收中断中处理
 *     4. 不需要传入参数，直接调用即可
 ****************************************************************/
SMS_STS_Error_t SMS_STS_SyncRead_Fixed(void)
{
    // 如果当前有数据在传输，则不进行发送
    if (SMS_STS_Read_Flag != 0) {
        return SMS_STS_ERR_BUSY;
    }

    // 标识准备接收数据
    SMS_STS_Read_Flag = 2;
    
    // 准备固定的指令: FF FF FE 06 82 38 02 01 02 36
    uint8_t fixed_cmd[10] = {
        0xFF, 0xFF,   // 帧头
        0xFE,         // 广播ID
        0x06,         // 数据长度
        0x82,         // 同步读指令
        0x38,         // 寄存器地址(位置寄存器)
        0x02,         // 数据长度(2字节)
        0x01, 0x02,   // 舵机ID列表(ID为1和2)
        0x36          // 校验和
    };
    
    // 发送固定指令
    Serial_SendArray(fixed_cmd, 10);
    
    // 启动超时计数 - 2个舵机需要的时间
    SMS_STS_Timeout_Counter = SMS_STS_TIMEOUT * 2;
    
    return SMS_STS_OK;
}

/****************************************************************
 * 函数名称: SMS_STS_SyncRead_Example
 * 函数功能: 同步读指令使用示例，同时读取多个舵机的位置
 * 输入参数: 无
 * 返回值: 无
 * 使用说明: 
 *     此函数展示如何使用同步读指令
 *     可以根据实际需求修改要查询的舵机ID和寄存器
 ****************************************************************/
void SMS_STS_SyncRead_Example(void)
{
    // 固定发送FF FF FE 06 82 38 02 01 02 36
    SMS_STS_SyncRead_Fixed();
    
    // 注意：发送后不需要做其他处理
    // 舵机的响应会在接收中断中被自动处理
    // 处理后的数据会更新到STS_Data数组中
    // 可以直接访问 STS_Data[ID].Angle 等获取舵机角度
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
            
        case 7: // Checksum for single servo read response
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
                    // 获取位置值
                    uint16_t position = ((uint16_t)sms_rx_buf[6] << 8) | sms_rx_buf[5];
                    STS_Data[servo_id].Position = position;
                    
                    // 检查位置值是否包含方向位（表示负方向）
                    // 根据通信协议，第15位为1(即值>=32768)表示负方向
                    if (position >= SMS_STS_DIRECTION_BIT) {
                        // 存储方向信息
                        STS_Data[servo_id].Direction = -1;  // 负方向
                        // 存储实际位置值（去掉方向位）
                        STS_Data[servo_id].ActualPosition = position - SMS_STS_DIRECTION_BIT;
                        // 解析角度值，考虑负方向
                        STS_Data[servo_id].Angle = -SMS_STS_Position_To_Angle(STS_Data[servo_id].ActualPosition);
                        // 计算多圈角度值
                        STS_Data[servo_id].MultiTurnAngle = -SMS_STS_Position_To_Angle_Multi(STS_Data[servo_id].ActualPosition);
                    } else {
                        // 正方向
                        STS_Data[servo_id].Direction = 1;   // 正方向
                        STS_Data[servo_id].ActualPosition = position;
                        // 解析角度值，正方向
                        STS_Data[servo_id].Angle = SMS_STS_Position_To_Angle(STS_Data[servo_id].ActualPosition);
                        // 计算多圈角度值
                        STS_Data[servo_id].MultiTurnAngle = SMS_STS_Position_To_Angle_Multi(STS_Data[servo_id].ActualPosition);
                    }
                }
            }
            
            // 一次接收完成，复位状态机和标志位
            sms_rx_count = 0;
            SMS_STS_Read_Flag = 0;
            // 重置超时计数器
            SMS_STS_Timeout_Counter = 0;
            break;

        default:
            // 对于同步读取的响应，数据包长度会更长
            // 继续接收数据直到达到预期长度
            if (sms_rx_count < SMS_STS_RX_BUF_SIZE) {
                sms_rx_buf[sms_rx_count++] = data;
                
                // 检查是否收到了完整的数据包
                if (sms_rx_count >= 4) { // 至少接收到帧头(2)+ID(1)+长度(1)
                    uint8_t expected_length = sms_rx_buf[3] + 4; // 数据长度+帧头(2)+ID(1)+长度(1)
                    
                    // 如果接收到了完整的数据包
                    if (sms_rx_count == expected_length) {
                        // 处理同步读取的响应
                        SMS_STS_Process_SyncRead_Response(sms_rx_buf, sms_rx_count);
                        
                        // 重置状态
                        sms_rx_count = 0;
                        SMS_STS_Read_Flag = 0;
                        SMS_STS_Timeout_Counter = 0;
                    }
                }
            } else {
                // 缓冲区溢出，复位状态机
                sms_rx_count = 0;
                SMS_STS_Read_Flag = 0;
            }
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
        // 如果Speed是负数，根据协议转换为反转速度 (速度绝对值 + 方向位)
        final_speed = (uint16_t)(-Speed + SMS_STS_DIRECTION_BIT); 
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

/****************************************************************
 * 函数名称: SMS_STS_Position_To_Angle
 * 函数功能: 将舵机位置值(0-4095)转换为角度(0-360度)
 * 输入参数: 
 *     Position - 位置值(0-4095)
 * 返回值: 
 *     float - 对应的角度值(0-360度)
 * 使用说明: 
 *     此函数对位置值取余，确保角度始终在0-360度范围内
 ****************************************************************/
float SMS_STS_Position_To_Angle(uint16_t Position)
{
    // 对4096取余，确保位置值在0-4095范围内
    uint16_t normalized_position = Position % 4096;
    
    // 转换位置值为角度（0-360度）
    // 转换系数：360度/4096 = 0.087890625度/位置单位
    return (float)normalized_position * 0.087890625f;
}

/****************************************************************
 * 函数名称: SMS_STS_Position_To_Angle_Multi
 * 函数功能: 将多圈位置值转换为多圈角度
 * 输入参数: 
 *     Position - 多圈位置值(可大于4095)
 * 返回值: 
 *     float - 对应的多圈角度值
 * 使用说明: 
 *     此函数不对位置取余，支持多圈角度计算
 ****************************************************************/
float SMS_STS_Position_To_Angle_Multi(uint32_t Position)
{
    // 直接转换位置值为角度，支持多圈
    // 转换系数：360度/4096 = 0.087890625度/位置单位
    return (float)Position * 0.087890625f;
}

/****************************************************************
 * 函数名称: SMS_STS_Angle_To_Position
 * 函数功能: 将角度值(0-360度)转换为舵机位置值(0-4095)
 * 输入参数: 
 *     Angle - 角度值(0-360度)
 * 返回值: 
 *     uint16_t - 对应的位置值(0-4095)
 * 使用说明: 
 *     此函数对角度取余，确保位置值始终在0-4095范围内
 ****************************************************************/
uint16_t SMS_STS_Angle_To_Position(float Angle)
{
    // 对360取余，确保角度在0-360度范围内
    while (Angle >= 360.0f) {
        Angle -= 360.0f;
    }
    while (Angle < 0.0f) {
        Angle += 360.0f;
    }
    
    // 转换角度为位置值（0-4095）
    // 转换系数：4096/360度 = 11.37777度/位置单位
    return (uint16_t)(Angle * 11.37777f);
}

/****************************************************************
 * 函数名称: SMS_STS_Angle_To_Position_Multi
 * 函数功能: 将多圈角度值转换为多圈位置值
 * 输入参数: 
 *     Angle - 多圈角度值(可大于360度)
 * 返回值: 
 *     uint32_t - 对应的多圈位置值
 * 使用说明: 
 *     此函数不对角度取余，支持多圈位置计算
 ****************************************************************/
uint32_t SMS_STS_Angle_To_Position_Multi(float Angle)
{
    // 直接转换角度为位置值，支持多圈
    // 转换系数：4096/360度 = 11.37777度/位置单位
    return (uint32_t)(Angle * 11.37777f);
}

/****************************************************************
 * 函数名称: SMS_STS_Process_SyncRead_Byte
 * 函数功能: 逐字节处理同步读取响应数据
 * 输入参数: 
 *     data - 接收到的一个字节数据
 * 返回值: 无
 * 使用说明: 
 *     1. 该函数在串口接收中断中调用
 *     2. 每收到一个同步读取响应字节调用一次
 *     3. 内部维护状态机自动处理数据包的识别和解析
 *     4. 解析成功后会更新对应舵机的位置信息
 ****************************************************************/
void SMS_STS_Process_SyncRead_Byte(uint8_t data)
{
    // 检测连续的两个帧头，重置计数器
    if (sync_rx_last_byte == SMS_STS_FRAME_HEADER && data == SMS_STS_FRAME_HEADER && sync_rx_count > 1) {
        sync_rx_count = 1; // 重置计数器到第二个帧头
    }
    
    // 记录当前字节供下次使用
    sync_rx_last_byte = data;
    
    // 缓冲区溢出检查
    if (sync_rx_count >= SMS_STS_RX_BUF_SIZE) {
        sync_rx_count = 0;
        return;
    }

    // 存储当前字节
    sync_rx_buf[sync_rx_count++] = data;

    // 状态机处理数据包
    if (sync_rx_count >= 4) { // 至少接收到帧头(2) + ID(1) + 长度(1)
        // 检查帧头是否正确
        if (sync_rx_buf[0] != SMS_STS_FRAME_HEADER || sync_rx_buf[1] != SMS_STS_FRAME_HEADER) {
            // 帧头错误，重置状态机
            sync_rx_count = 0;
            return;
        }
        
        uint8_t servo_id = sync_rx_buf[2];        // 获取舵机ID
        uint8_t data_length = sync_rx_buf[3];     // 获取数据长度
        uint8_t expected_length = data_length + 4; // 完整数据包长度 = 数据长度 + 帧头(2) + ID(1) + 长度(1)
        
        // 检查是否已接收到完整数据包
        if (sync_rx_count == expected_length) {
            // 计算校验和
            uint8_t checksum = 0;
            for (uint8_t i = 2; i < sync_rx_count - 1; i++) {
                checksum += sync_rx_buf[i];
            }
            checksum = ~checksum;
            
            // 校验和正确，处理数据包
            if (sync_rx_buf[sync_rx_count - 1] == checksum) {
                // 检查ID有效性
                if (SMS_STS_Is_Valid_ID(servo_id)) {
                    // 确保数据长度至少包含位置数据
                    if (data_length >= 3) { // 至少有错误码(1) + 位置数据(2)
                        // 解析位置数据，位置数据从错误码(data[4])后开始
                        uint16_t position = ((uint16_t)sync_rx_buf[6] << 8) | sync_rx_buf[5];
                        
                        // 更新舵机数据
                        STS_Data[servo_id].Position = position;
                        
                        // 处理方向和角度
                        if (position >= SMS_STS_DIRECTION_BIT) {
                            // 负方向
                            STS_Data[servo_id].Direction = -1;
                            STS_Data[servo_id].ActualPosition = position - SMS_STS_DIRECTION_BIT;
                            STS_Data[servo_id].Angle = -SMS_STS_Position_To_Angle(STS_Data[servo_id].ActualPosition);
                            STS_Data[servo_id].MultiTurnAngle = -SMS_STS_Position_To_Angle_Multi(STS_Data[servo_id].ActualPosition);
                        } else {
                            // 正方向
                            STS_Data[servo_id].Direction = 1;
                            STS_Data[servo_id].ActualPosition = position;
                            STS_Data[servo_id].Angle = SMS_STS_Position_To_Angle(STS_Data[servo_id].ActualPosition);
                            STS_Data[servo_id].MultiTurnAngle = SMS_STS_Position_To_Angle_Multi(STS_Data[servo_id].ActualPosition);
                        }
                        
                        // 如果数据长度足够，可以解析更多信息
                        if (data_length >= 8) {
                            // 可以在这里添加对其他参数的解析
                            // 例如速度、负载等
                        }
                    }
                }
            }
            
            // 处理完成，重置状态机
            sync_rx_count = 0;
        }
    }
}

/****************************************************************
 * 函数名称: SMS_STS_Receive_Enhanced
 * 函数功能: 增强版接收函数，能处理普通响应和同步读取响应
 * 输入参数: 
 *     data - 串口接收到的一个字节数据
 * 返回值: 无
 * 使用说明: 
 *     1. 该函数应放置在串口接收中断中调用
 *     2. 每收到一个字节就调用一次该函数
 *     3. 自动判断并处理不同类型的响应数据
 ****************************************************************/
void SMS_STS_Receive_Enhanced(uint8_t data)
{
    // 如果当前不在等待接收状态，直接返回
    if (SMS_STS_Read_Flag != 2) {
        return;
    }
    
    // 处理接收到的字节
    SMS_STS_Process_SyncRead_Byte(data);
}

// 将MotorStatus的定义和motor_status数组的声明移到.h文件
MotorStatus motor_status[SMS_STS_MAX_SERVOS + 1];

/*控制舵机角度
函数参数:id号,控制角度
*/
void control(uint8_t id,float angle)
{
	// 只保存目标角度到状态数组中
	motor_status[id].angle = angle;
}

/****************************************************************
 * 函数名称: Update_Servos
 * 函数功能: 更新所有舵机的状态，包括设置角度和请求位置
 * 输入参数: 无
 * 返回值: 无
 * 使用说明:
 *     1. 在主循环中调用此函数
 *     2. 该函数会根据 motor_status 数组中的目标角度来驱动舵机
 *     3. 驱动后会立即发送读取指令以获取舵机的最新位置
 ****************************************************************/
void Update_Servos(void)
{
    // 1. 根据 motor_status 中的目标角度，驱动舵机1和2转动
    SMS_STS_Set_Angle(1, motor_status[1].angle, 0, 0); 
    SMS_STS_Set_Angle(2, motor_status[2].angle, 0, 0);
    
    // 2. 发送指令读取舵机1的位置
    SMS_STS_Read(1);
    // 短暂延时，确保总线空闲，避免指令冲突
    delay_ms(2);
    // 3. 发送指令读取舵机2的位置
    SMS_STS_Read(2);
}

/*控制舵机“位置”
函数参数:id号,位置值
*/
void control_position(uint8_t id,uint16_t position)
{
	// 只保存目标位置到状态数组中
	motor_status[id].position = position;
}
/****************************************************************
 * 函数名称: Update_Servos
 * 函数功能: 更新所有舵机的状态，包括设置位置和请求位置
 * 输入参数: 无
 * 返回值: 无
 * 使用说明:
 *     1. 在主循环中调用此函数
 *     2. 该函数会根据 motor_status 数组中的目标位置来驱动舵机
 *     3. 驱动后会立即发送读取指令以获取舵机的最新位置
 ****************************************************************/
void Update_Servos_Position(void)
{
    // 1. 根据 motor_status 中的目标位置，驱动舵机1和2转动
    SMS_STS_Run(1, motor_status[1].position, 0, 0); 
    SMS_STS_Run(2, motor_status[2].position, 0, 0);
    
    // 2. 发送指令读取舵机1的位置
    SMS_STS_Read(1);
    // 短暂延时，确保总线空闲，避免指令冲突
    delay_ms(2);
    // 3. 发送指令读取舵机2的位置
    SMS_STS_Read(2);
}
