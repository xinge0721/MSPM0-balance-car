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
