#include "Encoder.h"

// 定义一个全局变量，用来记录左边轮子编码器的计数值。
// 使用 volatile 关键字，是因为这个变量会在中断服务函数中被修改，
// 这样可以防止编译器进行一些可能导致错误的优化。
volatile int Encoder_Left_Value = 0;
// 定义一个全局变量，用来记录右边轮子编码器的计数值。
volatile int Encoder_Right_Value = 0;

// 这是中断服务函数(ISR)，专门处理第一组(Group 1)的中断。
// 当GPIOA或者GPIOB上我们设定好的引脚电平发生变化时，程序就会暂停正在做的事情，
// 自动跳转到这里来执行代码。
void GROUP1_IRQHandler(void)
{

    // Case 1: 如果是 GPIOA 端口触发了中断 (右轮编码器接在GPIOA)

    // 读取GPIOA端口上，哪些引脚的中断被触发了。
    // 我们只关心右轮编码器的A相和B相引脚。
    uint32_t gpio_interrupt_status = DL_GPIO_getEnabledInterruptStatus(GPIOA, encoder_right_A_PIN | encoder_right_B_PIN | encoder_left_A_PIN | encoder_left_B_PIN );
        
    // 判断是不是右轮的 A 相引脚触发了中断
    if(gpio_interrupt_status & encoder_right_A_PIN)
    {
        // 读取A相引脚当前的电平 (高电平/真)
        if(DL_GPIO_readPins(encoder_PORT, encoder_right_A_PIN))
        {
            // 如果此时B相也是高电平
            if(DL_GPIO_readPins(encoder_PORT, encoder_right_B_PIN))
            {
                // 判定为正转，计数值加1
                Encoder_Right_Value++;
            }
            else // 如果此时B相是低电平
            {
                // 判定为反转，计数值减1
                Encoder_Right_Value--;
            }
        }
        else // 读取A相引脚当前的电平 (低电平/假)
        {
            // 如果此时B相是高电平
            if(DL_GPIO_readPins(encoder_PORT, encoder_right_B_PIN))
            {
                // 判定为反转，计数值减1
                Encoder_Right_Value--;
            }
            else // 如果此时B相是低电平
            {
                // 判定为正转，计数值加1
                Encoder_Right_Value++;
            }
        }
        // 处理完A相的中断了，必须清除这个中断标志，否则程序会一直卡在中断里
        DL_GPIO_clearInterruptStatus(encoder_PORT,encoder_right_A_PIN);
    }

    // 判断是不是右轮的 B 相引脚触发了中断
    if(gpio_interrupt_status & encoder_right_B_PIN)
    {
        // 读取B相引脚当前的电平 (高电平/真)
        if(DL_GPIO_readPins(encoder_PORT, encoder_right_B_PIN))
        {
            // 如果此时A相也是高电平
            if(DL_GPIO_readPins(encoder_PORT, encoder_right_A_PIN))
            {
                // 判定为反转，计数值减1
                Encoder_Right_Value--;
            }
            else // 如果此时A相是低电平
            {
                // 判定为正转，计数值加1
                Encoder_Right_Value++;
            }
        }
        else // 读取B相引脚当前的电平 (低电平/假)
        {
            // 如果此时A相是高电平
            if(DL_GPIO_readPins(encoder_PORT, encoder_right_A_PIN))
            {
                // 判定为正转，计数值加1
                Encoder_Right_Value++;
            }
            else // 如果此时A相是低电平
            {
                // 判定为反转，计数值减1
                Encoder_Right_Value--;
            }
        }
        // 处理完B相的中断了，同样要清除中断标志
        DL_GPIO_clearInterruptStatus(encoder_PORT, encoder_right_B_PIN);
    }

        
    // 读取GPIOB端口上，哪些引脚的中断被触发了。
    // 我们只关心左轮编码器的A相和B相引脚。
    // 判断是不是左轮的 A 相引脚触发了中断
    if(gpio_interrupt_status & encoder_left_A_PIN)
    {
        // 读取A相引脚当前的电平 (高电平/真)
        if(DL_GPIO_readPins(encoder_PORT, encoder_left_A_PIN))
        {
            // 如果此时B相也是高电平
            if(DL_GPIO_readPins(encoder_PORT, encoder_left_B_PIN))
            {
                // 判定为反转，计数值减1 (这里的正反转定义可能和右轮相反，取决于电机安装方向)
                Encoder_Left_Value--;
            }
            else // 如果此时B相是低电平
            {
                // 判定为正转，计数值加1
                Encoder_Left_Value++;
            }
        }
    else // 读取A相引脚当前的电平 (低电平/假)
        {
            // 如果此时B相是高电平
            if(DL_GPIO_readPins(encoder_PORT, encoder_left_B_PIN))
            {
                // 判定为正转，计数值加1
                Encoder_Left_Value++;
            }
            else // 如果此时B相是低电平
            {
                // 判定为反转，计数值减1
                Encoder_Left_Value--;
            }
        }
        // 处理完A相的中断了，必须清除这个中断标志
        DL_GPIO_clearInterruptStatus(encoder_PORT,encoder_left_A_PIN);
    }

    // 判断是不是左轮的 B 相引脚触发了中断
    if(gpio_interrupt_status & encoder_left_B_PIN)
    {
        // 读取B相引脚当前的电平 (高电平/真)
    if(DL_GPIO_readPins(encoder_PORT, encoder_left_B_PIN))
        {
            // 如果此时A相也是高电平
            if(DL_GPIO_readPins(encoder_PORT, encoder_left_A_PIN))
            {
                // 判定为正转，计数值加1
                Encoder_Left_Value++;
            }
            else // 如果此时A相是低电平
            {
                // 判定为反转，计数值减1
                Encoder_Left_Value--;
            }
        }
    else // 读取B相引脚当前的电平 (低电平/假)
        {
            // 如果此时A相是高电平
            if(DL_GPIO_readPins(encoder_PORT, encoder_left_A_PIN))
            {
                // 判定为反转，计数值减1
                Encoder_Left_Value--;
            }
            else // 如果此时A相是低电平
            {
                // 判定为正转，计数值加1
                Encoder_Left_Value++;
            }
        }
        // 处理完B相的中断了，同样要清除中断标志
        DL_GPIO_clearInterruptStatus(encoder_PORT,encoder_left_B_PIN);
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
