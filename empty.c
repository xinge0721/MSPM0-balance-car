#include "ti_msp_dl_config.h"
#include "./Hardware/OLED/OLED.h"
#include <stdio.h>
#include "sys.h"
// 定义一个全局变量，用来记录左边轮子编码器的计数值。
// 使用 volatile 关键字，是因为这个变量会在中断服务函数中被修改，
// 这样可以防止编译器进行一些可能导致错误的优化。
volatile int Encoder_Left_Value = 0;
// 定义一个全局变量，用来记录右边轮子编码器的计数值。
volatile int Encoder_Right_Value = 0;

int main( void )
{
    // 这个函数是TI官方提供的，用来初始化芯片的各种配置，比如时钟、GPIO等。
    SYSCFG_DL_init();
    // 初始化OLED屏幕
   OLED_Init();
    // 使能(打开) GPIOB 端口的中断。左轮编码器的引脚接在GPIOB上。
    NVIC_EnableIRQ(GPIOB_INT_IRQn);
    // 使能(打开) GPIOA 端口的中断。右轮编码器的引脚接在GPIOA上。
    NVIC_EnableIRQ(GPIOA_INT_IRQn);

    // 这是一个死循环，单片机程序的主体，代码会一直在这里运行。
   while(1)
   {
    // 通过串口打印出左右编码器的计数值，方便我们调试观察。
    printf("Left: %d, Right: %d\n", Encoder_Left_Value, Encoder_Right_Value);
    // 延时100毫秒，防止打印速度太快，刷屏看不清。
    delay_ms(100);
   }
}

// 这是中断服务函数(ISR)，专门处理第一组(Group 1)的中断。
// 当GPIOA或者GPIOB上我们设定好的引脚电平发生变化时，程序就会暂停正在做的事情，
// 自动跳转到这里来执行代码。
void GROUP1_IRQHandler(void)
{
    // 使用 switch 语句来判断具体是哪个外设触发了中断。
    // 一个中断组里可能包含多个中断源（比如GPIOA, GPIOB, 定时器等）。
    switch( DL_Interrupt_getPendingGroup(DL_INTERRUPT_GROUP_1) )
    {
        // Case 1: 如果是 GPIOA 端口触发了中断 (右轮编码器接在GPIOA)
        case DL_INTERRUPT_GROUP1_IIDX_GPIOA:
        {
            // 读取GPIOA端口上，哪些引脚的中断被触发了。
            // 我们只关心右轮编码器的A相和B相引脚。
            uint32_t gpio_interrupt_status = DL_GPIO_getEnabledInterruptStatus(GPIOA, encoder_right_A_PIN | encoder_right_B_PIN);
             
            // 判断是不是右轮的 A 相引脚触发了中断
            if(gpio_interrupt_status & encoder_right_A_PIN)
            {
                // 读取A相引脚当前的电平 (高电平/真)
                if(DL_GPIO_readPins(encoder_right_A_PORT, encoder_right_A_PIN))
                {
                    // 如果此时B相也是高电平
                    if(DL_GPIO_readPins(encoder_right_B_PORT, encoder_right_B_PIN))
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
                    if(DL_GPIO_readPins(encoder_right_B_PORT, encoder_right_B_PIN))
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
                DL_GPIO_clearInterruptStatus(encoder_right_A_PORT,encoder_right_A_PIN);
            }

            // 判断是不是右轮的 B 相引脚触发了中断
            if(gpio_interrupt_status & encoder_right_B_PIN)
            {
                // 读取B相引脚当前的电平 (高电平/真)
                if(DL_GPIO_readPins(encoder_right_B_PORT, encoder_right_B_PIN))
                {
                    // 如果此时A相也是高电平
                    if(DL_GPIO_readPins(encoder_right_A_PORT, encoder_right_A_PIN))
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
                    if(DL_GPIO_readPins(encoder_right_A_PORT, encoder_right_A_PIN))
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
                DL_GPIO_clearInterruptStatus(encoder_right_B_PORT, encoder_right_B_PIN);
            }
        }
        break;

        // Case 2: 如果是 GPIOB 端口触发了中断 (左轮编码器接在GPIOB)
        case DL_INTERRUPT_GROUP1_IIDX_GPIOB:
        {
            // 读取GPIOB端口上，哪些引脚的中断被触发了。
            // 我们只关心左轮编码器的A相和B相引脚。
            uint32_t gpio_interrupt_status = DL_GPIO_getEnabledInterruptStatus(GPIOB, encoder_left_A_PIN | encoder_left_B_PIN);

            // 判断是不是左轮的 A 相引脚触发了中断
             if(gpio_interrupt_status & encoder_left_A_PIN)
            {
                // 读取A相引脚当前的电平 (高电平/真)
                if(DL_GPIO_readPins(encoder_left_A_PORT, encoder_left_A_PIN))
                {
                    // 如果此时B相也是高电平
                    if(DL_GPIO_readPins(encoder_left_B_PORT, encoder_left_B_PIN))
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
                    if(DL_GPIO_readPins(encoder_left_B_PORT, encoder_left_B_PIN))
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
                DL_GPIO_clearInterruptStatus(encoder_left_A_PORT,encoder_left_A_PIN);
            }

            // 判断是不是左轮的 B 相引脚触发了中断
            if(gpio_interrupt_status & encoder_left_B_PIN)
            {
                // 读取B相引脚当前的电平 (高电平/真)
                if(DL_GPIO_readPins(encoder_left_B_PORT, encoder_left_B_PIN))
                {
                    // 如果此时A相也是高电平
                    if(DL_GPIO_readPins(encoder_left_A_PORT, encoder_left_A_PIN))
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
                    if(DL_GPIO_readPins(encoder_left_A_PORT, encoder_left_A_PIN))
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
                DL_GPIO_clearInterruptStatus(encoder_left_B_PORT,encoder_left_B_PIN);
            }
        }
        break;

    }
}
