

#ifndef __SERVO_H__
#define __SERVO_H__

// 舵机角度通用限制(实际为PWM值)
#define SERVO_MAX_ANGLE        400     // 舵机最大PWM值 

// 舵机1角度限制 - 根据实际使用位置调整(实际为PWM值)
#define SERVO1_MIN_ANGLE       0       // 舵机1最小PWM值
#define SERVO1_MAX_ANGLE       400     // 舵机1最大PWM值
#define SERVO1_MID_ANGLE       200     // 舵机1中间位置PWM值
#define SERVO1_LEFT_LIMIT      100     // 舵机1左极限PWM值
#define SERVO1_RIGHT_LIMIT     300     // 舵机1右极限PWM值

// 舵机2角度限制 - 根据实际使用位置调整(实际为PWM值)
#define SERVO2_MIN_ANGLE       0       // 舵机2最小PWM值
#define SERVO2_MAX_ANGLE       400     // 舵机2最大PWM值
#define SERVO2_MID_ANGLE       200     // 舵机2中间位置PWM值
#define SERVO2_UP_LIMIT        100     // 舵机2上极限PWM值
#define SERVO2_DOWN_LIMIT      300     // 舵机2下极限PWM值

// Servo 1 - 270度舵机
void Set_Servo1_Angle(unsigned int angle);
unsigned int Get_Servo1_Angle(void);
void Set_Servo1_MidAngle(void);

// Servo 2 - 270度舵机
void Set_Servo2_Angle(unsigned int angle);
unsigned int Get_Servo2_Angle(void);
void Set_Servo2_MidAngle(void);

// Legacy functions for backward compatibility
void Set_Servo_Angle(unsigned int angle);
unsigned int Get_Servo_Angle(void);

#endif
