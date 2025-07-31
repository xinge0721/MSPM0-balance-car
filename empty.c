#include "system/sys/sys.h"

float angle = 0.0f;
char txbuf[3]={0,2,3};
extern uint8_t go_flag;
uint8_t keynum;
uint8_t current_motor_index = 1;
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

int main( void )
{
    SYSCFG_DL_init();
    NVIC_ClearPendingIRQ(UART_0_INST_INT_IRQN);
    //使能串口中断
    NVIC_EnableIRQ(UART_0_INST_INT_IRQN);
    NVIC_ClearPendingIRQ(UART_1_INST_INT_IRQN);
    //使能串口中断
    NVIC_EnableIRQ(UART_1_INST_INT_IRQN);
    
    NVIC_ClearPendingIRQ(TIMER_0_INST_INT_IRQN);
    //使能定时器中断
    NVIC_EnableIRQ(TIMER_0_INST_INT_IRQN);
    
    // SMS_STS_Init();
    //delay_ms(20);
    // OLED初始化
    OLED_Init();
    // 这里需要传入对应的串口print的函数，是函数名，因为参数是一个函数指针
    //APP_Init(uart0_printf);
    // 维特陀螺仪初始化
    WIT_Init();
    // 初始化系统中断

    // 使能(打开) 编码器 端口的中断。左轮编码器的引脚接在GPIOA上
    NVIC_EnableIRQ(GPIOA_INT_IRQn);
    NVIC_EnableIRQ(GPIOB_INT_IRQn);

    // SMS_STS_Init();
    int iiii = 0;
    while(1)
    {   
        Set_Servo1_Angle(iiii);
        iiii += 1;
        keynum=Get_Key1Num();
        if(keynum==1){go_flag=1;}
          

        // Control_speed(0,2000);
        // // 调用封装好的函数来更新舵机状态
        // SMS_STS_Set_Angle_With_Direction(1,iiii,0,0);
        // SMS_STS_Set_Angle_With_Direction(2,-JJJJ,0,0);
        
        // iiii += 1;
        // JJJJ += 1;
        //Update_Servos();//读舵机当前角度
        // 在第二行显示右轮编码器值

        // OLED_ShowString(1, 1, "A:");
        // OLED_ShowFloatNum(1, 3,A_stable,4);
        
        // OLED_ShowString(2, 1, "F:");
        // OLED_ShowFloatNum(2, 3,Topic_flag, 4);

        // OLED_ShowString(3, 1, "R:");
        // OLED_ShowSignedNum(3, 3,target_position_right, 4);//

        // OLED_ShowString(4, 1, "L:");
        // OLED_ShowSignedNum(4, 3, target_position_left,4);

        // OLED_ShowString(1, 1, "R:");
        // OLED_ShowFloatNum(1, 3,STS_Data[1].MultiTurnAngle,4);
        
        // OLED_ShowString(2, 1, "L:");
        // OLED_ShowFloatNum(2, 3,STS_Data[2].MultiTurnAngle, 4);

        // OLED_ShowString(3, 1, "Value:");
        // OLED_ShowSignedNum(3, 7,current_motor_index, 4);//

        // OLED_ShowString(4, 1, "yaw:");
        // OLED_ShowFloatNum(4, 7, (wit_data.yaw),3);

        // OLED_ShowString(1, 1, "X:");
        // OLED_ShowSignedNum(1, 7,OpenMv_X, 4);

        // OLED_ShowString(2, 1, "Y:");
        // OLED_ShowSignedNum(2, 7,OpenMv_Y, 4);

        // OLED_ShowString(3, 1, "Status:");
        // OLED_ShowSignedNum(3, 7,OpenMv_status, 4);
    }
}

//printf
int fputc(int c, FILE * stream)
{
    DL_UART_Main_transmitDataBlocking(UART_1_INST,c); // 发送完成后结束
    return c;
}

int fputs(const char * restrict s, FILE * restrict stream)
{
    uint16_t i, len;
    len = strlen(s);
    for(i=0; i<len; i++)
    {
        DL_UART_Main_transmitDataBlocking(UART_1_INST, s[i]);
    }
    return len;
}

int puts(const char *_ptr)
{
    int count = fputs(_ptr,stdout);
    count += fputs("\n",stdout);
    return count;
}