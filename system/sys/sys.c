#include "sys.h"
#include "../Hardware/SMS_STS/SMS_STS.h"
//#include "mspm0_clock.h"

// 左编码器�?
float left_encoder_value = 0;
// 右编码器�?
float right_encoder_value = 0;

// 目标角度变量定义
int target_angle_x = 0;
int target_angle_y = 0;

// 这是中断服务函数(ISR)，专门处理第一组(Group 1)的中断
// 当GPIOA或者GPIOB上我们设定好的引脚电平发生变化时，程序就会暂停正在做的事情，
// 自动跳转到这里来执行代码
void GROUP1_IRQHandler(void)
{
    /*MPU605O INT */
    
    // #if defined GPIO_MULTIPLE_GPIOA_INT_IIDX || defined GPIO_MULTIPLE_GPIOB_INT_IIDX || defined GPIO_MULTIPLE_INT_IIDX
    // if (DL_GPIO_getEnabledInterruptStatus(GPIO_MPU6050_PORT, GPIO_MPU6050_PIN_INT_PIN) & GPIO_MPU6050_PIN_INT_PIN)
    // {
    //     Read_Quad();
    // }
    // #endif

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
}




uint8_t LED_count;
uint8_t go_flag=0;


float target_angle = 0.0f; // 目标角度变量

//定时器的中断服务函数 已配置为10ms的周期
void TIMER_0_INST_IRQHandler(void)
{
    //如果产生了定时器中断
    switch( DL_TimerG_getPendingInterrupt(TIMER_0_INST) )
    {
        case DL_TIMER_IIDX_ZERO://如果0溢出中断
            LED_count ++;

            // 每10ms扫描一次按键状态机（支持5个按键）
            KEY_ReadStateMachine(0); // 扫描KEY1
            KEY_ReadStateMachine(1); // 扫描KEY2
            KEY_ReadStateMachine(2); // 扫描KEY3
            KEY_ReadStateMachine(3); // 扫描KEY4
            KEY_ReadStateMachine(4); // 扫描KEY5

            if( LED_count % 10 == 0)//500ms中断一次
            {
                //将LED灯的状态翻转，确认单片机没有卡
                DL_GPIO_togglePins(LED_PORT, LED_PIN_22_PIN);
            }

            target_angle_x = get_angle_x();

            target_angle_y = get_angle_y();


            // 获取左编码器
            right.now_speed = right_encoder_value  = -Get_Encoder_Right();
            // 获取右编码器
            left.now_speed  = left_encoder_value   = -Get_Encoder_Left();
            
            // 在定时器中计算并设置目标角度
            // 示例：每秒增加10度
            if(LED_count % 50 == 0) // 每秒执行一次
            {
            }

            // Control_speed(0,2000);
            if(go_flag ==1)
            {
                Topic_3();
            }
            break;

        default://其他的定时器中断
            break;
    }
}

// 滴答定时器中断服务函数
void SysTick_Handler(void)
{
    SMS_STS_Timeout_Handler();
    //tick_ms++;
    if( delay_times != 0 )
    {
        delay_times--;
    }
}  

