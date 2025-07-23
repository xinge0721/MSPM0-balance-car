#include "sys.h"
#include "mspm0_clock.h"

// 左编码器�?
float left_encoder_value = 0;
// 右编码器�?
float right_encoder_value = 0;
// 初始化PID
// 右轮PID
pid right = {
    0.1,//kp
    15,//ki
    0,//kd
    0,//last_err
    0,//ControlVelocity
    0//
};

// 左轮PID
pid left = {
    0.1,//kp
    15,//ki
    0,//kd
    0,//last_err
    0,//ControlVelocity
    0//
};

// 旋转PID
pid turn = {
    0,//kp
    0,//ki
    0,//kd
    0,//last_err
    0,//ControlVelocity
    0
};

// 这是中断服务函数(ISR)，专门处理第一�?(Group 1)的中断�?
// 当GPIOA或者GPIOB上我们设定好的引脚电平发生变化时，程序就会暂停正在做的事情，
// 自动跳转到这里来执行代码�?
void GROUP1_IRQHandler(void)
{
    /*MPU605O INT */
    
    #if defined GPIO_MULTIPLE_GPIOA_INT_IIDX || defined GPIO_MULTIPLE_GPIOB_INT_IIDX || defined GPIO_MULTIPLE_INT_IIDX
    if (DL_GPIO_getEnabledInterruptStatus(GPIO_MPU6050_PORT, GPIO_MPU6050_PIN_INT_PIN) & GPIO_MPU6050_PIN_INT_PIN)
    {
        Read_Quad();
    }
    #endif

    // --- 右轮编码器逻辑 (假设�? GPIOA) ---
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
    else if(gpioa_interrupt_status & encoder_right_B_PIN)
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
    else if(gpioa_interrupt_status & encoder_left_A_PIN)
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
    else if(gpioa_interrupt_status & encoder_left_B_PIN)
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

    uint32_t gpiob_interrupt_status = DL_GPIO_getEnabledInterruptStatus(GPIOB, SR04_Echo_PIN);
    if(gpiob_interrupt_status & SR04_Echo_PIN)
    {
        // SR04_ECHO() 宏用于读取ECHO引脚的当前电�?
        if( SR04_ECHO() ) // 如果是上升沿 (从低电平变为高电�?)
        {
            // 只有在空闲状态下收到的第一个上升沿才是有效的计时开始信�?
            if (sr04_state == SR04_IDLE)
            {
                HCSR04_StartTime = Get_TIMER_Count();
                sr04_state = SR04_WAIT_FALLING_EDGE; // 切换到等待下降沿状�?
            }
        }
        else // 如果是下降沿 (从高电平变为低电�?)
        {
            // 只有在等待下降沿状态下收到的下降沿，才是有效的测量结束信号
            if (sr04_state == SR04_WAIT_FALLING_EDGE)
            {
                uint32_t pulse_width_us = Get_TIMER_Count() - HCSR04_StartTime;
                distance = (float)pulse_width_us / 58.0f;
                
                SR04_Flag = 1;   // 设置完成标志，告诉主程序可以读取距离�?
                sr04_state = SR04_IDLE; // 测量完成，返回空闲状�?
                DL_GPIO_disableInterrupt(SR04_PORT, SR04_Echo_PIN); // 收到回波后立即关闭中断，防止重复触发
            }
        }
        DL_GPIO_clearInterruptStatus(GPIOB, SR04_Echo_PIN);
    }
}




int LED_count;
int PID_count;
//定时器的中断服务函数 已配置为1秒的周期
void TIMER_0_INST_IRQHandler(void)
{
    //如果产生了定时器中断
    switch( DL_TimerG_getPendingInterrupt(TIMER_0_INST) )//为了配合超声波模块，现在这个定时器是10ms一�?
    {
        case DL_TIMER_IIDX_ZERO://如果�?0溢出中断
            msHcCount++;
            LED_count ++;

            if( LED_count % 10 == 0)//10ms中断一�?
            {
                //将LED灯的状态翻转，确认单片机没有卡�?
                DL_GPIO_togglePins(LED_PORT, LED_PIN_22_PIN);
            }
                turn.kp = 0.3;
                // 获取左编码�?
                right.now_speed = right_encoder_value  = -Get_Encoder_Right();
                // 获取右编码�?
                left.now_speed  = left_encoder_value   = -Get_Encoder_Left();

                PID_run(left,right,turn,0,0);
                // Control_speed(2000,2000);
                // Servo_Tick_Handler();
 
            break;

        default://其他的定时器中断
            break;
    }
}

// 滴答定时器中断服务函�?
void SysTick_Handler(void)
{
    
    tick_ms++;
    if( delay_times != 0 )
    {
        delay_times--;
    }
}

