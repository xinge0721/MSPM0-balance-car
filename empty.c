#include "ti_msp_dl_config.h"
#include "system/sys/sys.h"
#include "system/delay/delay.h"
#include "Hardware/Encoder/Encoder.h"
#include <stdio.h>
#include "./Hardware/OLED/OLED.h"
// 左编码器值
int left_encoder_value = 0;
// 右编码器值
int right_encoder_value = 0;
/**
  * @brief  AIN_1引脚输出
  * @param  x: 0或1
  * @retval 无
  */
 void AIN_1_Send_SDA(bool x)
 {
     (x == 1) ? DL_GPIO_setPins(Control_AIN1_PORT, Control_AIN1_PIN) : DL_GPIO_clearPins(Control_AIN1_PORT, Control_AIN1_PIN);
 }
 
 /**
   * @brief  AIN_2引脚输出
   * @param  x: 0或1
   * @retval 无
   */
 void AIN_2_Send_SCL(bool x)
 {
     (x == 1) ? DL_GPIO_setPins(Control_AIN1_PORT, Control_AIN2_PIN) : DL_GPIO_clearPins(Control_AIN1_PORT, Control_AIN2_PIN);
 }

//  前进    
void forward(void)
{
    AIN_1_Send_SDA(1);
    AIN_2_Send_SCL(0);
}

// 后退
void back(void)
{
    AIN_1_Send_SDA(0);
    AIN_2_Send_SCL(1);
}
char str[30]={0};
int i = 0;

int main( void )
{
    printf("开始\n");
    // 这个函数是TI官方提供的，用来初始化芯片的各种配置，比如时钟、GPIO等。
    SYSCFG_DL_init();
    NVIC_ClearPendingIRQ(TIMER_0_INST_INT_IRQN);
    //使能定时器中断
    NVIC_EnableIRQ(TIMER_0_INST_INT_IRQN);

    OLED_Init();

    // // 初始化系统中断
    // // 使能(打开) 编码器 端口的中断。左轮编码器的引脚接在GPIOA上。
    NVIC_EnableIRQ(encoder_INT_IRQN);


    while(1)
    {
        // 在第一行显示左轮编码器值
        OLED_ShowString(1, 1, "L:");

        OLED_ShowNum(1, 3, (uint32_t)left_encoder_value, 5);
    
        // 在第二行显示右轮编码器值
        OLED_ShowString(2, 1, "R:");

        OLED_ShowSignedNum(2, 3, right_encoder_value, 5);
        
        OLED_ShowString(3, 1, "i:");//判断是否进入中断

        OLED_ShowSignedNum(3, 3, i, 5);
        
        printf("下一回合\n");
        forward();
        DL_TimerG_setCaptureCompareValue(PWM_serov_INST,0,GPIO_PWM_serov_C0_IDX);
        
        // 添加适当的延时，避免刷新过快
        delay_ms(100);
    }
}

int consthh;
//定时器的中断服务函数 已配置为1秒的周期
void TIMER_0_INST_IRQHandler(void)
{

    //如果产生了定时器中断
    switch( DL_TimerG_getPendingInterrupt(TIMER_0_INST) )
    {
        case DL_TIMER_IIDX_ZERO://如果是0溢出中断
            //将LED灯的状态翻转
            if( consthh%10 == 0)
                DL_GPIO_togglePins(LED_PORT, LED_PIN_22_PIN);
            consthh++;
            i++;
            left_encoder_value = Get_Encoder_Left();
            right_encoder_value = Get_Encoder_Right();
            break;

        default://其他的定时器中断
            break;
    }
}
