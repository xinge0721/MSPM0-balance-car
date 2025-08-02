#include "system/sys/sys.h"
#include "Hardware/Serial/Serial.h"
#include "Hardware/APP/APP.h"
#include "Hardware/key/key.h"
#include "Hardware/OLED/OLED.h"
#include "system/topic/topic.h"
#include "system/delay/delay.h"
#include <string.h>

float angle = 0.0f;
char txbuf[3]={0,2,3};
extern uint8_t go_flag;
extern float target_angle; // 从sys.c导入的目标角度变量
uint8_t keynum;
uint8_t current_motor_index = 1;
uint8_t key1_pressed = 0;  // 记录按键一是否被按下
uint8_t Get_Key1Num()
{
	uint8_t KeyNum=0;
	if(!KEY_key1())
	{
		delay_ms(20);
		while(!KEY_key1());
		delay_ms(20);
		KeyNum=1;
	}
	return KeyNum;
}

uint8_t Get_Key2Num()
{
	uint8_t KeyNum=0;
	if(!KEY_key2())
	{
		delay_ms(20);
		while(!KEY_key2());
		delay_ms(20);
		KeyNum=2;
	}
	return KeyNum;
}

uint8_t Get_Key3Num()
{
	uint8_t KeyNum=0;
	if(!KEY_key3())
	{
		delay_ms(20);
		while(!KEY_key3());
		delay_ms(20);
		KeyNum=3;
	}
	return KeyNum;
}

uint8_t Get_Key4Num()
{
	uint8_t KeyNum=0;
	if(!KEY_key4())
	{
		delay_ms(20);
		while(!KEY_key4());
		delay_ms(20);
		KeyNum=4;
	}
	return KeyNum;
}

uint8_t Get_Key5Num()
{
	uint8_t KeyNum=0;
	if(!KEY_key5())
	{
		delay_ms(20);
		while(!KEY_key5());
		delay_ms(20);
		KeyNum=5;
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
    // 按键驱动初始化
    KEY_Init();
    // 这里需要传入对应的串口print的函数，是函数名，因为参数是一个函数指针
    APP_Init(uart0_printf);
    // 维特陀螺仪初始化
    // WIT_Init();
    // 初始化系统中断

    // 使能(打开) 编码器 端口的中断。左轮编码器的引脚接在GPIOA上
    NVIC_EnableIRQ(GPIOA_INT_IRQn);
    NVIC_EnableIRQ(GPIOB_INT_IRQn);
    SMS_STS_Init();
    delay_s(2);

    int iiii = 0;
    
    // 显示启动信息


    // 简单的GPIO测试主循环 - 排除所有干扰
    while(1)
    {   
        delay_ms(10);
        // 简单的按键检测
        // 按键一 - 控制x轴反转（仅当OpenMv_status==0时生效）
        keynum=Get_Key1Num();
        if(keynum==1){
            go_flag=1;
            key1_pressed = 1;  // 记录按键一被按下
        } else {
            key1_pressed = 0;  // 按键一未被按下
        }
        
        // 按键二 - 短按执行任务一
        keynum=Get_Key2Num();
        if(keynum==2) {
            current_task_state = TASK_1_RUNNING;
        }
        
        // 按键三 - 短按执行任务三  
        keynum=Get_Key3Num();
        if(keynum==3) {
            current_task_state = TASK_3_RUNNING;
        }
        
        // 按键四 - 增加圈数
        keynum=Get_Key4Num();
        if(keynum==4) {
            if(target_laps < 255) target_laps++;
            quanshu = target_laps; // 更新任务一的圈数变量
        }
        
        // 按键五 - 减少圈数
        keynum=Get_Key5Num();
        if(keynum==5) {
            if(target_laps > 1) target_laps--;
            quanshu = target_laps; // 更新任务一的圈数变量
        }
        
        // 根据任务状态控制舵机位置更新
        if(current_task_state == TASK_3_RUNNING) {
            // 任务三运行时必须调用Update_Servos_Position
            Update_Servos_Position();
        } else if(OpenMv_status != 2 && current_task_state != TASK_1_RUNNING) {
            // 其他情况下的原有逻辑（任务一时不执行）
            Update_Servos_Position();
        }
        // 显示任务状态和圈数信息
        OLED_ShowString(1, 1, "Task:");
        OLED_ShowNum(1, 6, current_task_state, 1);          // 任务状态
        OLED_ShowString(2, 1, "Laps:");
        OLED_ShowNum(2, 6, target_laps, 2);                 // 目标圈数
        
        // 根据任务状态显示不同信息
        if(current_task_state == TASK_3_RUNNING) {
            // 任务三运行时显示瞄准信息
            OLED_ShowString(3, 1, "Err:");
            OLED_ShowSignedNum(3, 5, target_angle_x, 4);    // X轴像素误差
            OLED_ShowString(4, 1, "Dir:");
            OLED_ShowNum(4, 5, OpenMv_status, 1);           // 显示转动方向状态
        } else if(current_task_state == TASK_1_RUNNING) {
            // 任务一运行时显示编码器信息
            OLED_ShowString(3, 1, "L:");
            OLED_ShowSignedNum(3, 3, left_encoder_value, 4);
            OLED_ShowString(4, 1, "R:");
            OLED_ShowSignedNum(4, 3, right_encoder_value, 4);
        } else {
            // 空闲状态显示提示信息
            OLED_ShowString(3, 1, "K1Rev:");
            OLED_ShowNum(3, 7, key1_pressed, 1);           // 显示按键一状态
            OLED_ShowString(4, 1, "Sta:");
            OLED_ShowNum(4, 5, OpenMv_status, 1);          // 显示OpenMv状态
        }

        // 自适应瞄准系统调试参数发送（只调试X轴舵机）
        float arrrr[5] = {
            15,     // X轴舵机当前带符号位置（支持负数）
            target_angle_y,                 // X轴像素误差（摄像头检测）
            aiming_y.scale_factor,          // X轴自适应比例系数
            aiming_y.overshoot_count,        // X轴过冲次数
            // OpenMv_X,                 // X坐标值（带符号）
            OpenMv_Y              // Y坐标值（带符号）
        };
        APP_Send(arrrr,5);

        // 显示系统状态
        // OLED_ShowString(1, 1, "State:");
        // OLED_ShowNum(1, 7, go_flag, 1);
        
        // OLED_ShowString(2, 1, "Angle:");
        // OLED_ShowFloatNum(2, 7, target_angle, 3);
        

        // Control_speed(0,2000);
        // // 调用封装好的函数来更新舵机状态
        // SMS_STS_Set_Angle_With_Direction(1,iiii,0,0);
        // SMS_STS_Set_Angle_With_Direction(2,-JJJJ,0,0);
        
        // iiii += 1;
        // JJJJ += 1;
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
        // Update_Servos_Position();

        // 当前目标值
        // OLED_ShowString(1, 1, "X:");
        // OLED_ShowNum(1, 3,OpenMv_X, 3);

        // OLED_ShowString(2, 1, "Y:");
        // OLED_ShowNum(2, 3,OpenMv_Y, 3);

        // 当前角度值
        // OLED_ShowString(1, 1, "1:");
        // OLED_ShowNum(1, 3,motor_status[1].position, 4);

        // OLED_ShowString(2, 1, "2:");
        // OLED_ShowNum(2, 3,motor_status[2].position, 4);

        // 串口接收数据显示
        // OLED_ShowHexNum(3, 1,uart_arr[0], 2);
        // OLED_ShowHexNum(3, 4,uart_arr[1], 2);
        // OLED_ShowHexNum(3, 7,uart_arr[2], 2);
        // OLED_ShowHexNum(3, 10,uart_arr[3], 2);
        // OLED_ShowHexNum(4, 1,uart_arr[4], 2);
        // OLED_ShowHexNum(4, 4,uart_arr[5], 2);
        // OLED_ShowHexNum(4, 7,uart_arr[6], 2);
        // OLED_ShowHexNum(4, 10,uart_arr[7], 2);
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