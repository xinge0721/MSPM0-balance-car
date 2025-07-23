
#include "system/sys/sys.h"



int main( void )
{
    char oled_buffer[40];
    printf("开始\n");
    // 这个函数是TI官方提供的，用来初始化芯片的各种配置，比如时钟、GPIO等
    SYSCFG_DL_init();

    // OLED初始化
    OLED_Init();

    // 这里需要传入对应的串口print的函数，是函数名，因为参数是一个函数指针
    // APP_Init(uart0_printf);

    // 俩个陀螺仪 二选一
    // 维特陀螺仪初始化
    WIT_Init();
    // MPU6050 初始化
    // mpu6050_init();

    // 初始化系统中断
    // 使能(打开) 编码器 端口的中断。左轮编码器的引脚接在GPIOA上
    NVIC_EnableIRQ(GPIOA_INT_IRQn);
    NVIC_EnableIRQ(GPIOB_INT_IRQn);

    NVIC_ClearPendingIRQ(TIMER_0_INST_INT_IRQN);
    // //使能定时器中断
    NVIC_EnableIRQ(TIMER_0_INST_INT_IRQN);

    NVIC_ClearPendingIRQ(UART_0_INST_INT_IRQN);
    //使能串口中断
    NVIC_EnableIRQ(UART_0_INST_INT_IRQN);


    while(1)
    {
        // uint32_t Value = (int)SR04_GetLength();
        // 在第二行显示右轮编码器
        OLED_ShowString(1, 1, "R:");

        OLED_ShowSignedNum(1, 3,right.now_speed, 4);
        
        OLED_ShowString(2, 1, "L:");

        OLED_ShowSignedNum(2, 3,left.now_speed, 4);
        OLED_ShowString(3, 1, "Value:");

        OLED_ShowSignedNum(3, 7,turn.now_speed, 4);

        OLED_ShowString(4, 1, "yaw:");
        OLED_ShowFloatNum(4, 7, wit_data.yaw,3);


        // Set_Servo_Angle(i);
        // 添加适当的延时，避免刷新过快
        // delay_ms(10);
    }
}

