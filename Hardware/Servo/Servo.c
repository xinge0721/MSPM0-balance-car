

#include "system/delay/delay.h"
#include "Servo.h"



// 舵机连接�? PA4
#define SERVO_PIN   GPIO_Servo_PORT
#define SERVO_PORT  GPIO_Servo_PIN_15_PIN

// 脉冲宽度范围: 500-2500us (对应0.5-2.5ms)
#define SERVO_MIN_PULSE 500
#define SERVO_MAX_PULSE 2500

// 全局变量，用于在中断中确定脉冲宽�?(单位: us)
int servo_pulse_us = 1667; 

// 中间位置脉冲宽度
#define SERVO_MIDDLE_PULSE 1667 

// 直接设置脉冲宽度(us)
// 舵机脉宽范围: 500us ~ 2500us 对应 0~180�?
void Servo_SetPulse(int Pulse_us)
{
    if (Pulse_us < SERVO_MIN_PULSE) Pulse_us = SERVO_MIN_PULSE;
    if (Pulse_us > SERVO_MAX_PULSE) Pulse_us = SERVO_MAX_PULSE;
    
    servo_pulse_us = Pulse_us;
}

// 使用角度设置舵机位置 (角度范围: 0~180�?)
void Servo_SetAngle_Int16(int16_t angle)
{
    // 限制角度范围
    if (angle < 0) angle = 0;
    if (angle > 180) angle = 180;
    
    // 角度转换为脉冲宽�?
    // 脉冲宽度(μs) = 500 + 角度 × 11.11
    int pulse = SERVO_MIN_PULSE + (int)((float)angle * 11.11f);
    
    // 设置脉冲宽度
    Servo_SetPulse(pulse);
}



// 此函数需要被 SysTick_Handler �?20ms的频率调�?
void Servo_Tick_Handler(void)
{
    static int tick_counter = 0;
    for(int i=0;i<2500;i++)
    {
        if(i <= servo_pulse_us)
        {
            DL_GPIO_setPins(SERVO_PIN,SERVO_PORT);
        }
        else
        {
            DL_GPIO_clearPins(SERVO_PIN,SERVO_PORT);
        }
        delay_1us(1);
    }
} 

