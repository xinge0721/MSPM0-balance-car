#include "Encoder.h"

// 定义一个全局变量，用来记录左边轮子编码器的计数值。
// 使用 volatile 关键字，是因为这个变量会在中断服务函数中被修改，
// 这样可以防止编译器进行一些可能导致错误的优化。
volatile int Encoder_Left_Value = 0;
// 定义一个全局变量，用来记录右边轮子编码器的计数值。
volatile int Encoder_Right_Value = 0;
volatile int Encoder_i_Value = 0;

/**
 * 获取左轮编码器的计数值，并将计数值清零
 * @return 返回读取到的左轮编码器计数值
 */
int Get_Encoder_Left(void)
{
    // 创建一个临时变量来存储当前的计数值
    int temp = Encoder_Left_Value;
    
    // 将计数值重置为零
    Encoder_Left_Value = 0;
    
    // 返回存储的计数值
    return temp;
}

/**
 * 获取右轮编码器的计数值，并将计数值清零
 * @return 返回读取到的右轮编码器计数值
 */
int Get_Encoder_Right(void)
{
    // 创建一个临时变量来存储当前的计数值
    int temp = Encoder_Right_Value;
    
    // 将计数值重置为零
    Encoder_Right_Value = 0;
    
    // 返回存储的计数值
    return temp;
}

// 里程计算变量
uint32_t total_distance = 0;       // 总行驶距离(米)
uint32_t cur_left = 0;      // 上次左轮计数
uint32_t cur_right = 0;     // 上次右轮计数
// 里程计算函数（在定时器中断或主循环中调用）
int update_mileage(void)
{
    // 临时禁用中断防止读取时被修改
    cur_left += left_encoder_value;
    cur_right+= right_encoder_value;
    // 4. 计算平均增量（双轮平均更准确）
    uint32_t total = ((cur_left + cur_right)/2)*0.01*WHEEL_CIRCUMFERENCE;
    
    return total;
}

void clear_mileage(void)
{
    total_distance = 0;       // 总行驶距离(米)
    cur_left = 0;      // 上次左轮计数
    cur_right = 0;     // 上次右轮计数
}