#include "sys.h"



// 初始化系统中断
void system_init(void)
{
    // 使能(打开) GPIOB 端口的中断。左轮编码器的引脚接在GPIOB上。
    NVIC_EnableIRQ(GPIOB_INT_IRQn);
    // 使能(打开) GPIOA 端口的中断。右轮编码器的引脚接在GPIOA上。
    NVIC_EnableIRQ(GPIOA_INT_IRQn);
}




