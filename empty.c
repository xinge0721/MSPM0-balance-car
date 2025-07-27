
#include "system/sys/sys.h"
#include "Hardware/OLED/OLED.h"
int main( void )
{
    // 这个函数是TI官方提供的，用来初始化芯片的各种配置，比如时钟、GPIO等
    SYSCFG_DL_init();

    // OLED初始化
    OLED_Init();

    // 这里需要传入对应的串口print的函数，是函数名，因为参数是一个函数指针
    APP_Init(uart0_printf);

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
    //使能定时器中断
    NVIC_EnableIRQ(TIMER_0_INST_INT_IRQN);

    NVIC_ClearPendingIRQ(UART_0_INST_INT_IRQN);
    //使能串口中断
    NVIC_EnableIRQ(UART_0_INST_INT_IRQN);

    float angle = 0.0f;

    while(1)
    {   
        // 1. 使用角度控制舵机1和2转动 (0-360度)
        SMS_STS_Run(1, angle, 0, 0); 
        // delay_ms(10);
        SMS_STS_Run(2, angle, 0, 0);
        
        // delay_ms(200);

        // 2. 读取舵机位置
        SMS_STS_Read(1);
        delay_ms(2); 
        SMS_STS_Read(2);
        delay_ms(1);

        // 3. 在OLED上显示信息
        // 显示原始报文
        OLED_ShowHexNum(1,1,uart_arr[0],2);
        OLED_ShowHexNum(1,4,uart_arr[1],2);
        OLED_ShowHexNum(2,1,uart_arr[2],2);
        OLED_ShowHexNum(2,4,uart_arr[3],2);
        
        // 显示目标角度
        OLED_ShowString(1, 8, "A:");
        OLED_ShowNum(1, 10, (uint16_t)angle, 3);

        // 显示舵机1和2的实际位置(0-4095)
        OLED_ShowString(3, 1, "S1_P:");
        OLED_ShowNum(3, 6, STS_Data[1].Position, 4);

        OLED_ShowString(4, 1, "S2_P:");
        OLED_ShowNum(4, 6, STS_Data[2].Position, 4);
        // delay_ms(10);

        // 4. 增加目标角度，实现循环转动
        angle += 10.0f;
        if (angle > 360.0f*7) {
            // SMS_STS_Clear_Circle(1);
            angle = 0.0f;
        }
    }
}

