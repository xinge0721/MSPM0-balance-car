

#ifndef __SERVO_H__
#define __SERVO_H__

// 设置舵机角度
void Servo_SetAngle_Int16(int16_t angle);
// 修改舵机脉冲宽度
void Servo_SetPulse(int Pulse_us);
// 舵机脉冲宽度处理函数
void Servo_Tick_Handler(void);

extern int servo_pulse_us ; 

#endif
