#include "ti_msp_dl_config.h"
#include "system/sys/sys.h"
#include "system/delay/delay.h"
#include "Hardware/Encoder/Encoder.h"
#include <stdio.h>
#include "./Hardware/OLED/OLED.h"
#include "Hardware/Serial/Serial.h"
#include <math.h>
#include "Hardware/PID/pid.h"
#include "Hardware/Control/Control.h"
#include "Hardware/APP/APP.h"   
#include "wit.h"
// 左编码器值
float left_encoder_value = 0;
// 右编码器值
float right_encoder_value = 0;
int i = 0;
char str[30]={0};

int main( void )
{
    char oled_buffer[40];
    printf("开始\n");
    // 这个函数是TI官方提供的，用来初始化芯片的各种配置，比如时钟、GPIO等。
    SYSCFG_DL_init();
    NVIC_ClearPendingIRQ(TIMER_0_INST_INT_IRQN);
    //使能定时器中断
    NVIC_EnableIRQ(TIMER_0_INST_INT_IRQN);

    OLED_Init();

    // // 初始化系统中断
    // // 使能(打开) 编码器 端口的中断。左轮编码器的引脚接在GPIOA上。
    NVIC_EnableIRQ(GPIOA_INT_IRQn);
    NVIC_EnableIRQ(GPIOB_INT_IRQn);
    APP_Init(uart0_printf);
    WIT_Init();
    Control_forward();

    while(1)
    {
        // 在第二行显示右轮编码器值
        OLED_ShowString(1, 1, "R:");

        OLED_ShowSignedNum(1, 3,(uint32_t)right_encoder_value, 4);
        
        OLED_ShowString(2, 1, "L:");

        OLED_ShowSignedNum(2, 3,(uint32_t)left_encoder_value, 4);
        OLED_ShowString(3, 1, "i:");

        OLED_ShowSignedNum(3, 3,i, 4);

        OLED_ShowString(4, 1, "yaw:");
        OLED_ShowFloatNum(4, 7, (wit_data.yaw),3);

        // 添加适当的延时，避免刷新过快
        // delay_ms(10);
    }
}

int consthh;
//定时器的中断服务函数 已配置为1秒的周期
void TIMER_0_INST_IRQHandler(void)
{
    // 初始化PID

    // 右轮PID
        static pid right = {
        1,//kp
        15,//ki
        1,//kd
        0,//last_err
        3200,//MAX
        3200,//MIN
        0,//ControlVelocity
        0//
    };

    // 左轮PID
    static pid left = {
        1,//kp
        5,//ki
        1,//kd
        0,//last_err
        3200,//MAX
        3200,//MIN
        0,//ControlVelocity
        0
    };

    // 旋转PID
    static pid turn = {
        1,//kp
        0,//ki
        0,//kd
        0,//last_err
        3200,//MAX
        -3200,//MIN
        0,//ControlVelocity
        0
    };
    //如果产生了定时器中断
    switch( DL_TimerG_getPendingInterrupt(TIMER_0_INST) )
    {
        case DL_TIMER_IIDX_ZERO://如果是0溢出中断
            //将LED灯的状态翻转
            if( consthh++ %10 == 0)
                DL_GPIO_togglePins(LED_PORT, LED_PIN_22_PIN);
            // 获取左编码值
            right.now_speed = left_encoder_value  = -Get_Encoder_Left();
            // 获取右编码值
            left.now_speed = right_encoder_value = Get_Encoder_Right();
            // 运行PID
            PID_run(left,right,turn,0,70);
            // int speed = FeedbackControl(&right, 110, right_encoder_value);
            // Control_speed(speed,speed);
                // left_encoder_value  = -Get_Encoder_Left();
                // right_encoder_value  = Get_Encoder_Right();
                i++;
                // 运行PID
                // PID_run(left,right,turn,0,70);
                // FeedbackControl(&left, 70, left.now_speed);
                // int speed = FeedbackControl(&left, 70, left_encoder_value);
                // APP_Send((float[]){left_encoder_value,-right_encoder_value,100},3);
                // uart0_printf("speed:%d  left:%d \r\n",speed,left_encoder_value);

                // Control_speed(speed,speed);
            break;

        default://其他的定时器中断
            break;
    }
}
