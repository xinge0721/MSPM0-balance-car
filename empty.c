#include "system/sys/sys.h"
#include "Hardware/OLED/OLED.h"
#include "system/topic/topic.h"
#include "Hardware/PID/pid.h"
#include "Hardware/gray/gray.h"
float angle = 0.0f;
char txbuf[100];
extern uint8_t go_flag;
uint8_t keynum;
uint8_t Get_Key1Num()
{
	uint8_t KeyNum=0;
	if(!DL_GPIO_readPins(KEY_PORT,KEY_key1_PIN))
	{
		delay_ms(20);
		while(DL_GPIO_readPins(KEY_PORT,KEY_key1_PIN));
		delay_ms(20);
		KeyNum=1;
	}
	return KeyNum;
}

void Xunjiban(void)
{
    OLED_ShowNum(1,1,IN0,1);
    OLED_ShowNum(1,2,IN1,1);
    OLED_ShowNum(1,3,IN2,1);
    OLED_ShowNum(1,4,IN3,1);
    OLED_ShowNum(1,5,IN4,1);
    OLED_ShowNum(1,6,IN5,1);
    OLED_ShowNum(1,7,IN6,1);
    OLED_ShowNum(1,8,IN7,1);
    OLED_ShowNum(1,9,IN8,1);
    OLED_ShowNum(1,10,IN9,1);
    OLED_ShowNum(1,11,IN10,1);
    OLED_ShowNum(1,12,IN11,1);
}


int main( void )
{
    SYSCFG_DL_init();

    //delay_ms(20);
    // OLED初始化
    OLED_Init();
    // 这里需要传入对应的串口print的函数，是函数名，因为参数是一个函数指针
    APP_Init(uart0_printf);
    // 维特陀螺仪初始化
    // WIT_Init();
    // 初始化防误判系统
    // init_anti_misjudgment();
    // 初始化系统中断

    // 使能(打开) 编码器 端口的中断。左轮编码器的引脚接在GPIOA上
    NVIC_EnableIRQ(GPIOA_INT_IRQn);
    NVIC_EnableIRQ(GPIOB_INT_IRQn);

    NVIC_ClearPendingIRQ(TIMER_0_INST_INT_IRQN);
    //使能定时器中断
    NVIC_EnableIRQ(TIMER_0_INST_INT_IRQN);

    NVIC_ClearPendingIRQ(UART_0_INST_INT_IRQN);
    NVIC_ClearPendingIRQ(UART_1_INST_INT_IRQN);
    //使能串口中断
    NVIC_EnableIRQ(UART_0_INST_INT_IRQN);
    NVIC_EnableIRQ(UART_1_INST_INT_IRQN);
    
    SMS_STS_Init();
    int iiii = 0;
     int JJJJ = 0;
    while(1)
    {   
        if(iiii>180)
        {
            iiii = 0;
        }
        iiii += 1;
        keynum=Get_Key1Num();
        if(keynum==1){go_flag=1;}        

        // process_sensors();
        Xunjiban();
        OLED_ShowString(3, 1, "Value:");
        OLED_ShowSignedNum(3, 7,A_stable, 1);//total_distance
        OLED_ShowSignedNum(3, 10,Topic_flag, 1);//total_distance


        OLED_ShowString(4, 1, "yaw:");
        OLED_ShowFloatNum(4, 7, (wit_data.yaw),3);
        delay_ms(10);

        float data[] = {target_position_left, target_position_right,728*2};
        APP_Send(data, 3);

        delay_ms(10);

    }
}

// //printf
// int fputc(int c, FILE * stream)
// {
//     DL_UART_Main_transmitDataBlocking(UART_1_INST,c); // 发送完成后结束
//     return c;
// }

// int fputs(const char * restrict s, FILE * restrict stream)
// {
//     uint16_t i, len;
//     len = strlen(s);
//     for(i=0; i<len; i++)
//     {
//         DL_UART_Main_transmitDataBlocking(UART_1_INST, s[i]);
//     }
//     return len;
// }

// int puts(const char *_ptr)
// {
//     int count = fputs(_ptr,stdout);
//     count += fputs("\n",stdout);
//     return count;
// }