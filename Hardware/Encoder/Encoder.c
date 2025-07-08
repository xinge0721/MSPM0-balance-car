#include "Encoder.h"

// 定义一个全局变量，用来记录左边轮子编码器的计数值。
// 使用 volatile 关键字，是因为这个变量会在中断服务函数中被修改，
// 这样可以防止编译器进行一些可能导致错误的优化。
volatile int Encoder_Left_Value = 0;
// 定义一个全局变量，用来记录右边轮子编码器的计数值。
volatile int Encoder_Right_Value = 0;
volatile int Encoder_i_Value = 0;
// 这是中断服务函数(ISR)，专门处理第一组(Group 1)的中断。
// 当GPIOA或者GPIOB上我们设定好的引脚电平发生变化时，程序就会暂停正在做的事情，
// 自动跳转到这里来执行代码。
void GROUP1_IRQHandler(void)
{
    // --- 右轮编码器逻辑 (假设在 GPIOA) ---
    uint32_t gpioa_interrupt_status = DL_GPIO_getEnabledInterruptStatus(GPIOA, encoder_right_A_PIN | encoder_right_B_PIN | encoder_left_A_PIN | encoder_left_B_PIN);
    Encoder_i_Value++;
    // 判断是不是右轮的 A 相引脚触发了中断
    if(gpioa_interrupt_status & encoder_right_A_PIN)
    {
        if(DL_GPIO_readPins(GPIOA, encoder_right_A_PIN)) // 使用 GPIOA
        {
            if(DL_GPIO_readPins(GPIOA, encoder_right_B_PIN)) // 使用 GPIOA
                Encoder_Right_Value++;
            else
                Encoder_Right_Value--;
        }
        else
        {
            if(DL_GPIO_readPins(GPIOA, encoder_right_B_PIN)) // 使用 GPIOA
                Encoder_Right_Value--;
            else
                Encoder_Right_Value++;
        }
        DL_GPIO_clearInterruptStatus(GPIOA, encoder_right_A_PIN);
    }

    // 判断是不是右轮的 B 相引脚触发了中断
    if(gpioa_interrupt_status & encoder_right_B_PIN)
    {
        if(DL_GPIO_readPins(GPIOA, encoder_right_B_PIN)) // 使用 GPIOA
        {
            if(DL_GPIO_readPins(GPIOA, encoder_right_A_PIN)) // 使用 GPIOA
                Encoder_Right_Value--;
            else
                Encoder_Right_Value++;
        }
        else
        {
            if(DL_GPIO_readPins(GPIOA, encoder_right_A_PIN)) // 使用 GPIOA
                Encoder_Right_Value++;
            else
                Encoder_Right_Value--;
        }
        DL_GPIO_clearInterruptStatus(GPIOA, encoder_right_B_PIN);
    }

    // 判断是不是左轮的 A 相引脚触发了中断
    if(gpioa_interrupt_status & encoder_left_A_PIN)
    {
        if(DL_GPIO_readPins(GPIOA, encoder_left_A_PIN)) // 使用 GPIOB
        {
            if(DL_GPIO_readPins(GPIOA, encoder_left_B_PIN)) // 使用 GPIOB
                Encoder_Left_Value--;
            else
                Encoder_Left_Value++;
        }
        else
        {
            if(DL_GPIO_readPins(GPIOA, encoder_left_B_PIN)) // 使用 GPIOB
                Encoder_Left_Value++;
            else
                Encoder_Left_Value--;
        }
        DL_GPIO_clearInterruptStatus(GPIOA, encoder_left_A_PIN);
    }

    // 判断是不是左轮的 B 相引脚触发了中断
    if(gpioa_interrupt_status & encoder_left_B_PIN)
    {
        if(DL_GPIO_readPins(GPIOA, encoder_left_B_PIN)) // 使用 GPIOB
        {
            if(DL_GPIO_readPins(GPIOA, encoder_left_A_PIN)) // 使用 GPIOB
                Encoder_Left_Value++;
            else
                Encoder_Left_Value--;
        }
        else
        {
            if(DL_GPIO_readPins(GPIOA, encoder_left_A_PIN)) // 使用 GPIOB
                Encoder_Left_Value--;
            else
                Encoder_Left_Value++;
        }
        DL_GPIO_clearInterruptStatus(GPIOA, encoder_left_B_PIN);
    }
}

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
