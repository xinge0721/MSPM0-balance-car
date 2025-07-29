#include "sys.h"
//#include "mspm0_clock.h"

// 左编码器�?
float left_encoder_value = 0;
// 右编码器�?
float right_encoder_value = 0;

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
float target_angle = 0.0f; // 定义一个目标角度变量
//定时器的中断服务函数 已配置为10ms的周期
void TIMER_0_INST_IRQHandler(void)
{
    //如果产生了定时器中断
    switch( DL_TimerG_getPendingInterrupt(TIMER_0_INST) )
    {
        case DL_TIMER_IIDX_ZERO://如果0溢出中断
            LED_count ++;

            if( LED_count % 50 == 0)//100ms中断一次
            {
                //将LED灯的状态翻转，确认单片机没有卡
                DL_GPIO_togglePins(LED_PORT, LED_PIN_22_PIN);
                // 调用control函数更新目标角度，但不驱动舵机
                 if(go_flag != 0)
                 {
                    target_angle += 1.0f;
                    if(target_angle >= 30) target_angle = 0;
                    control(1, target_angle);
                    control(2, target_angle);
                 }


            }
            // 获取左编码器
            right.now_speed = right_encoder_value  = -Get_Encoder_Right();
            // 获取右编码器
            left.now_speed  = left_encoder_value   = -Get_Encoder_Left();
            
            // 在定时器中计算并设置目标角度
            // 示例：每秒增加10度
            if(LED_count % 50 == 0) // 每秒执行一次
            {
            }

            
            if(go_flag ==1)// 巡线
            {
                left.ki=48.0; left.kp=0.0; left.kd=0.0;//left.ki=110.0; left.kp=75.0; left.kd=8.0;
                right.ki=48.0; right.kp=0.0; right.kd=0.0;
                turn.kp=0.46; turn.kd=0.0;
                //total_distance = update_mileage();//里程
                mithon_run_yew(&left,&right,&turn,0,0);
                // turn_err_k = process_sensors();//循迹差
                // mithon_run_xunxian(&left,&right,&turn,turn_err_k,80);//85
                // if(total_distance==20)
                // {
                //     go_flag=9;
                //     clear_mileage();//清空里程
                // }
            }
            if(go_flag==2)// 转弯1
            {
                left.ki=20; left.kp=0.5;
                right.ki=20; right.kp=0.5;
                turn.kp=0.6; turn.kd=0.0;
                total_distance = update_mileage();
                mithon_run_yew(&left,&right,&turn,105,85);
                //mithon_run_gray(&left,&right,&turn,turn_err_k,85);//85
                if(total_distance==13)
                {
                    go_flag=3;
                    clear_mileage();
                }
            }
            if(go_flag==3)// 转弯2
            {
                left.ki=20; left.kp=0.5;
                right.ki=20; right.kp=0.5;
                turn.kp=0.66; turn.kd=0.0;
                total_distance = update_mileage();
                mithon_run_yew(&left,&right,&turn,-176,85);
                //mithon_run_gray(&left,&right,&turn,turn_err_k,85);//85
                if(total_distance==10)
                {
                    go_flag=4;
                    clear_mileage();
                }
            }
            if(go_flag ==4)// 巡线
            {
                left.ki=20; left.kp=0.5;
                right.ki=20; right.kp=0.5;
                turn.kp=0.63; turn.kd=0.0;
                total_distance = update_mileage();//里程
                //mithon_run_yaw(&left,&right,&turn,0,65);
                turn_err_k = process_sensors();//循迹差
                mithon_run_xunxian(&left,&right,&turn,turn_err_k,80);//85
                if(total_distance==22)
                {
                    go_flag=9;
                    clear_mileage();//清空里程
                }
            }
            // if(go_flag==5)// 转弯1
            // {
            //     left.ki=20; left.kp=0.5;
            //     right.ki=20; right.kp=0.5;
            //     turn.kp=0.6; turn.kd=0.0;
            //     total_distance = update_mileage();
            //     mithon_run_yaw(&left,&right,&turn,85);
            //     //mithon_run_gray(&left,&right,&turn,turn_err_k,85);//85
            //     if(total_distance==13)
            //     {
            //         go_flag=3;
            //         clear_mileage();
            //     }
            // }
            // if(go_flag==6)// 转弯2
            // {
            //     left.ki=20; left.kp=0.5;
            //     right.ki=20; right.kp=0.5;
            //     turn.kp=0.68; turn.kd=0.0;
            //     total_distance = update_mileage();
            //     mithon_run_yaw(&left,&right,&turn,-171,85);
            //     //mithon_run_gray(&left,&right,&turn,turn_err_k,85);//85
            //     if(total_distance==9)
            //     {
            //         go_flag=9;
            //         clear_mileage();
            //     }
            // }
            if(go_flag==9)
            {
                stopcar();//停车
            }

            // PID_run_xunxian(left,right,turn,tracking_status.deviation,20);
            // PID_run_xunxian(left,right,turn,tracking_status.deviation,20);
            // PID_run_xunxian(left,right,turn,tracking_status.deviation,20);
            // PID_run_xunxian(left,right,turn,tracking_status.deviation,20);
            // PID_run_xunxian(left,right,turn,tracking_status.deviation,20);

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

