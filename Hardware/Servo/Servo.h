

#ifndef __SERVO_H__
#define __SERVO_H__

// 舵机角度通用限制
#define SERVO_MAX_ANGLE        270     // 舵机最大角度 

// 舵机1角度限制 - 根据实际使用位置调整
#define SERVO1_MIN_ANGLE       0       // 舵机1最小角度限制
#define SERVO1_MAX_ANGLE       270     // 舵机1最大角度限制
#define SERVO1_MID_ANGLE       135     // 舵机1中间位置角度
#define SERVO1_LEFT_LIMIT      45      // 舵机1左极限角度
#define SERVO1_RIGHT_LIMIT     225     // 舵机1右极限角度

// 舵机2角度限制 - 根据实际使用位置调整
#define SERVO2_MIN_ANGLE       20      // 舵机2最小角度限制
#define SERVO2_MAX_ANGLE       250     // 舵机2最大角度限制
#define SERVO2_MID_ANGLE       135     // 舵机2中间位置角度
#define SERVO2_UP_LIMIT        60      // 舵机2上极限角度
#define SERVO2_DOWN_LIMIT      210     // 舵机2下极限角度

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
