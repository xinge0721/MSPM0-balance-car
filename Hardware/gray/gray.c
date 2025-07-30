#include "gray.h"

uint8_t IN0 , IN1, IN2 ,IN3 ,IN4 ,IN5 ,IN6 ,IN7 ,IN8 ,IN9 ,IN10,IN11;

void gw_gray_serial_read_simple (void)
{
    IN0= DL_GPIO_readPins(Tracking_IN1_PORT,Tracking_IN1_PIN)?1:0;
    IN1= DL_GPIO_readPins(Tracking_IN2_PORT,Tracking_IN2_PIN)?1:0;
    IN2= DL_GPIO_readPins(Tracking_IN3_PORT,Tracking_IN3_PIN)?1:0;
    IN3= DL_GPIO_readPins(Tracking_IN4_PORT,Tracking_IN4_PIN)?1:0;
    IN4= DL_GPIO_readPins(Tracking_IN5_PORT,Tracking_IN5_PIN)?1:0;
    IN5= DL_GPIO_readPins(Tracking_IN6_PORT,Tracking_IN6_PIN)?1:0;
    IN6= DL_GPIO_readPins(Tracking_IN7_PORT,Tracking_IN7_PIN)?1:0;
    IN7= DL_GPIO_readPins(Tracking_IN8_PORT,Tracking_IN8_PIN)?1:0;
    IN8= DL_GPIO_readPins(Tracking_IN9_PORT,Tracking_IN9_PIN)?1:0;
    IN9= DL_GPIO_readPins(Tracking_IN10_PORT,Tracking_IN10_PIN)?1:0;
    IN10= DL_GPIO_readPins(Tracking_IN11_PORT,Tracking_IN11_PIN)?1:0;
    IN11= DL_GPIO_readPins(Tracking_IN12_PORT,Tracking_IN12_PIN)?1:0;
}

int8_t turn_err_k=0;
int turn_err=0;
uint8_t A=0; // 识别到黑线标志位
uint8_t A_stable=0; // 防误判处理后的稳定状态

// 防误判处理相关变量
static uint8_t debounce_counter = 0;     // 消抖计数器
static uint8_t last_state = 0;           // 上一次的状态
static const uint8_t DEBOUNCE_THRESHOLD = 20; // 消抖阈值（连续20次相同状态才认为稳定）

// 使用示例
int process_sensors()
{
    gw_gray_serial_read_simple();
    
    int result = xunji();
    
    // 对A进行防误判处理，获得稳定状态
    process_line_detection_debounce(A);
    
    return result;
}

int xunji(void)
{
    if(IN0==1   &&IN1==0&&IN2==0&&IN3==0&&IN4==0&&IN5==0&&IN6==0&&IN7==0&&IN8==0&&IN9==0&&IN10==0&&IN11==0)
    {
        turn_err=55; A=1; 
    }
    else if(   IN0==1&&IN1==1   &&IN2==0&&IN3==0&&IN4==0&&IN5==0&&IN6==0&&IN7==0&&IN8==0&&IN9==0&&IN10==0&&IN11==0)
    {
        turn_err=50;A=1;
    }
    else if(IN0==0&&   IN1==1   &&IN2==0&&IN3==0&&IN4==0&&IN5==0&&IN6==0&&IN7==0&&IN8==0&&IN9==0&&IN10==0&&IN11==0)
    {
        turn_err=45;A=1;
    }
    else if(IN0==0&&   IN1==1&&IN2==1   &&IN3==0&&IN4==0&&IN5==0&&IN6==0&&IN7==0&&IN8==0&&IN9==0&&IN10==0&&IN11==0)
    {
        turn_err=40;A=1;
    }
    else if(IN0==0&&IN1==0&&   IN2==1   &&IN3==0&&IN4==0&&IN5==0&&IN6==0&&IN7==0&&IN8==0&&IN9==0&&IN10==0&&IN11==0)
    {
        turn_err=35;A=1;
    }
    else if(IN0==0&&IN1==0&&   IN2==1&&IN3==1   &&IN4==0&&IN5==0&&IN6==0&&IN7==0&&IN8==0&&IN9==0&&IN10==0&&IN11==0)
    {
        turn_err=30;A=1;
    }
    else if(IN0==0&&IN1==0&&IN2==0&&   IN3==1   &&IN4==0&&IN5==0&&IN6==0&&IN7==0&&IN8==0&&IN9==0&&IN10==0&&IN11==0)
    {
        turn_err=25; A=1;
    }
    else if(IN0==0&&IN1==0&&IN2==0&&   IN3==1&&IN4==1   &&IN5==0&&IN6==0&&IN7==0&&IN8==0&&IN9==0&&IN10==0&&IN11==0)
    {
        turn_err=20; A=1;
    }
    else if(IN0==0&&IN1==0&&IN2==0&&IN3==0&&   IN4==1   &&IN5==0&&IN6==0&&IN7==0&&IN8==0&&IN9==0&&IN10==0&&IN11==0)
    {
        turn_err=15; A=1;
    }
    else if(IN0==0&&IN1==0&&IN2==0&&IN3==0&&   IN4==1&&IN5==1   &&IN6==0&&IN7==0&&IN8==0&&IN9==0&&IN10==0&&IN11==0)
    {
        turn_err=10; A=1;
    }
    else if(IN0==0&&IN1==0&&IN2==0&&IN3==0&&IN4==0&&   IN5==1   &&IN6==0&&IN7==0&&IN8==0&&IN9==0&&IN10==0&&IN11==0)
    {
        turn_err=5; A=1;
    }
    else if(IN0==0&&IN1==0&&IN2==0&&IN3==0&&IN4==0&&   IN5==1&&IN6==1   &&IN7==0&&IN8==0&&IN9==0&&IN10==0&&IN11==0)
    {
        turn_err=0; A=1;
    }
    else if(IN0==0&&IN1==0&&IN2==0&&IN3==0&&IN4==0&&IN5==0&&   IN6==1   &&IN7==0&&IN8==0&&IN9==0&&IN10==0&&IN11==0)
    { 
        turn_err=-5; A=1;
    }
    else if(IN0==0&&IN1==0&&IN2==0&&IN3==0&&IN4==0&&IN5==0&&   IN6==1&&IN7==1   &&IN8==0&&IN9==0&&IN10==0&&IN11==0)
    {
        turn_err=-10; A=1;
    }
    else if(IN0==0&&IN1==0&&IN2==0&&IN3==0&&IN4==0&&IN5==0&&IN6==0&&   IN7==1   &&IN8==0&&IN9==0&&IN10==0&&IN11==0)
    {
        turn_err=-15; A=1;
    }
    else if(IN0==0&&IN1==0&&IN2==0&&IN3==0&&IN4==0&&IN5==0&&IN6==0&&   IN7==1&&IN8==1   &&IN9==0&&IN10==0&&IN11==0)
    {
        turn_err=-20; A=1;
    }
    else if(IN0==0&&IN1==0&&IN2==0&&IN3==0&&IN4==0&&IN5==0&&IN6==0&&IN7==0&&   IN8==1   &&IN9==0&&IN10==0&&IN11==0)
    {
        turn_err=-25; A=1;
    }
    else if(IN0==0&&IN1==0&&IN2==0&&IN3==0&&IN4==0&&IN5==0&&IN6==0&&IN7==0&&   IN8==1&&IN9==1   &&IN10==0&&IN11==0)
    {
        turn_err=-30; A=1;
    }
    else if(IN0==0&&IN1==0&&IN2==0&&IN3==0&&IN4==0&&IN5==0&&IN6==0&&IN7==0&&IN8==0&&   IN9==1   &&IN10==0&&IN11==0)
    {
        turn_err=-35; A=1;
    }
    else if(IN0==0&&IN1==0&&IN2==0&&IN3==0&&IN4==0&&IN5==0&&IN6==0&&IN7==0&&IN8==0&&   IN9==1&&IN10==1   &&IN11==0)
    {
        turn_err=-40; A=1;
    }
    else if(IN0==0&&IN1==0&&IN2==0&&IN3==0&&IN4==0&&IN5==0&&IN6==0&&IN7==0&&IN8==0&&IN9==0&&   IN10==1   &&IN11==0)
    {
        turn_err=-45; A=1;
    }
    else if(IN0==0&&IN1==0&&IN2==0&&IN3==0&&IN4==0&&IN5==0&&IN6==0&&IN7==0&&IN8==0&&IN9==0&&   IN10==1&&IN11==1   )
    {
        turn_err=-50; A=1;
    }
    else if(IN0==0&&IN1==0&&IN2==0&&IN3==0&&IN4==0&&IN5==0&&IN6==0&&IN7==0&&IN8==0&&IN9==0&&IN10==0&&   IN11==1   )
    {
        turn_err=-55; A=1;
    }
    else if(IN0==0&&IN1==0&&IN2==0&&IN3==0&&IN4==0&&IN5==0&&IN6==0&&IN7==0&&IN8==0&&IN9==0&&IN10==0&&   IN11==0   )
    {
        A=0; 

    }
	return turn_err;
}

/**
 * @brief 循迹板防误判处理函数（基于按键消抖思路）
 * @param current_state 当前检测到的线路状态（通常是变量A的值）
 * @return 防误判处理后的稳定状态
 * @note 此函数应每10ms调用一次，与传感器读取频率保持一致
 * 
 * 工作原理：
 * 1. 当检测到状态变化时，开始计数
 * 2. 只有当状态连续DEBOUNCE_THRESHOLD次保持不变时，才更新稳定状态
 * 3. 如果在计数期间状态发生变化，则重新开始计数
 */
uint8_t process_line_detection_debounce(uint8_t current_state)
{
    // 如果当前状态与上一次状态相同
    if (current_state == last_state) 
    {
        // 计数器递增，但不超过阈值
        if (debounce_counter < DEBOUNCE_THRESHOLD) 
        {
            debounce_counter++;
        }
        
        // 如果计数器达到阈值，更新稳定状态
        if (debounce_counter >= DEBOUNCE_THRESHOLD) 
        {
            A_stable = current_state;
        }
    }
    else 
    {
        // 状态发生变化，重置计数器，更新上一次状态
        debounce_counter = 1;  // 从1开始计数，因为当前状态已经是新状态的第一次
        last_state = current_state;
    }
    
    return A_stable;
}	