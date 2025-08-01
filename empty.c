#include "system/sys/sys.h"
#include "Hardware/Serial/Serial.h"
#include "Hardware/APP/APP.h"
#include <string.h>

float angle = 0.0f;
char txbuf[3]={0,2,3};
extern uint8_t go_flag;
extern float target_angle; // 从sys.c导入的目标角度变量
uint8_t keynum;
uint8_t current_motor_index = 1;

// 按键事件处理函数
void Handle_Key_Events(void)
{
    // 检查每个按键的事件
    for(uint8_t i = 0; i < 5; i++)
    {
        if(KeyCfg[i].KEY_Event != KEY_Event_Null)
        {
            switch(i)
            {
                case 0: // KEY1 事件处理
                    if(KeyCfg[i].KEY_Event == KEY_Event_SingleClick)
                    {
                        go_flag = !go_flag; // 单击KEY1切换运行状态
                        OLED_ShowString(3, 1, "KEY1 Click!");
                    }
                    else if(KeyCfg[i].KEY_Event == KEY_Event_DoubleClick)
                    {
                        current_motor_index++; // 双击KEY1切换电机
                        if(current_motor_index > 2) current_motor_index = 1;
                        OLED_ShowString(3, 1, "KEY1 Double!");
                    }
                    else if(KeyCfg[i].KEY_Event == KEY_Event_LongPress)
                    {
                        go_flag = 0; // 长按KEY1停止所有动作
                        target_angle = 0; // 重置目标角度
                        OLED_ShowString(3, 1, "KEY1 Long!");
                    }
                    break;
    //                 
                case 1: // KEY2 事件处理
                    if(KeyCfg[i].KEY_Event == KEY_Event_SingleClick)
                    {
                        target_angle += 10; // 单击KEY2增加角度
                        if(target_angle > 180) target_angle = 180;
                        OLED_ShowString(3, 1, "KEY2 Click!");
                 }
                    break;
                    
                case 2: // KEY3 事件处理
                    if(KeyCfg[i].KEY_Event == KEY_Event_SingleClick)
                    {
                        target_angle -= 10; // 单击KEY3减少角度
                        if(target_angle < 0) target_angle = 0;
                        OLED_ShowString(3, 1, "KEY3 Click!");
                    }
                    break;
                    
                case 3: // KEY4 事件处理
                    if(KeyCfg[i].KEY_Event == KEY_Event_SingleClick)
                    {
                        // 可以添加其他功能
                        OLED_ShowString(3, 1, "KEY4 Click!");
                    }
                    break;
                    
                case 4: // KEY5 事件处理
                    if(KeyCfg[i].KEY_Event == KEY_Event_SingleClick)
                    {
                        // 可以添加其他功能
                        OLED_ShowString(3, 1, "KEY5 Click!");
                    }
                    break;
            }
            
            // 清除事件标志
            KeyCfg[i].KEY_Event = KEY_Event_Null;
        }
    }
}
uint8_t Get_Key1Num()
{
	uint8_t KeyNum=0;
	if(!KEY_key1())
	{
		delay_ms(20);
		while(KEY_key1());
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
    // 按键驱动初始化
    KEY_Init();
    // 这里需要传入对应的串口print的函数，是函数名，因为参数是一个函数指针
    APP_Init(uart0_printf);
    // 维特陀螺仪初始化
    WIT_Init();
    // 初始化系统中断

    // 使能(打开) 编码器 端口的中断。左轮编码器的引脚接在GPIOA上
    NVIC_EnableIRQ(GPIOA_INT_IRQn);
    NVIC_EnableIRQ(GPIOB_INT_IRQn);
        delay_s(2);

    // SMS_STS_Init();
    int iiii = 0;
    
    // 显示启动信息
    // OLED_ShowString(1, 1, "Key Demo Start!");
    // OLED_ShowString(2, 1, "KEY1:Start/Stop");
    // OLED_ShowString(3, 1, "KEY2/3:Angle+/-");
    // OLED_ShowString(4, 1, "Ready...");
    SMS_STS_Run(1,0,0,0);
    SMS_STS_Run(2,0,0,0);

    while(1)
    {   
        // 处理按键事件（高优先级）
        // Handle_Key_Events();
    
        // 保留原有的按键检测作为备用
        keynum=Get_Key1Num();
        if(keynum==1){go_flag=1;}
        Update_Servos_Position();

        // 显示当前位置
        OLED_ShowString(1, 1, "X:");
        OLED_ShowNum(1, 3,STS_Data[1].Position,4);
        OLED_ShowString(2, 1, "Y:");
        OLED_ShowNum(2, 3,STS_Data[2].Position,4);


        float arrrr[3] = {STS_Data[1].Position,STS_Data[2].Position,2000};
        APP_Send(arrrr,3);

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
        delay_ms(100);
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