#include "sys.h"
// 左编码器值
float left_encoder_value = 0;
// 右编码器值
float right_encoder_value = 0;

// 这是中断服务函数(ISR)，专门处理第一组(Group 1)的中断。
// 当GPIOA或者GPIOB上我们设定好的引脚电平发生变化时，程序就会暂停正在做的事情，
// 自动跳转到这里来执行代码。
void GROUP1_IRQHandler(void)
{
    // --- 右轮编码器逻辑 (假设在 GPIOA) ---
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
        // SR04_ECHO() 宏用于读取ECHO引脚的当前电平
        if( SR04_ECHO() ) // 如果是上升沿 (从低电平变为高电平)
        {
            // 只有在空闲状态下收到的第一个上升沿才是有效的计时开始信号
            if (sr04_state == SR04_IDLE)
            {
                HCSR04_StartTime = Get_TIMER_Count();
                sr04_state = SR04_WAIT_FALLING_EDGE; // 切换到等待下降沿状态
            }
        }
        else // 如果是下降沿 (从高电平变为低电平)
        {
            // 只有在等待下降沿状态下收到的下降沿，才是有效的测量结束信号
            if (sr04_state == SR04_WAIT_FALLING_EDGE)
            {
                uint32_t pulse_width_us = Get_TIMER_Count() - HCSR04_StartTime;
                distance = (float)pulse_width_us / 58.0f;
                
                SR04_Flag = 1;   // 设置完成标志，告诉主程序可以读取距离了
                sr04_state = SR04_IDLE; // 测量完成，返回空闲状态
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
        15,//ki
        1,//kd
        0,//last_err
        3200,//MAX
        3200,//MIN
        0,//ControlVelocity
        0//
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
    switch( DL_TimerG_getPendingInterrupt(TIMER_0_INST) )//为了配合超声波模块，现在这个定时器是1ms一次
    {
        case DL_TIMER_IIDX_ZERO://如果是0溢出中断
            msHcCount++;
            LED_count ++;
            PID_count ++;
            if( LED_count % 100 == 0)//10ms中断一次
            {
                //将LED灯的状态翻转，确认单片机没有卡死
                DL_GPIO_togglePins(LED_PORT, LED_PIN_22_PIN);
            }

            if( PID_count % 10 == 0)//5ms中断一次
            {
                // 获取左编码值
                right.now_speed = left_encoder_value  = -Get_Encoder_Left();
                // 获取右编码值
                left.now_speed = right_encoder_value = - Get_Encoder_Right();
                // 运行PID
                mithon_run(&left,&right,&turn,0,50);
            }
 
            
            break;

        default://其他的定时器中断
            break;
    }
}


