#ifndef __PID_H
#define __PID_H

#include <stdint.h> // 添加标准整数类型定义

/**
 * @brief PID控制器类定义
 * 
 * 此类实现了通用的PID控制算法，包含三种常用PID控制模式：
 * 1. 增量式PID - 适用于速度环控制
 * 2. 位置式PID - 适用于位置环控制
 * 3. 转向控制 - 特殊的PD控制，用于平衡车转向
 * 
 * 类内部保存了PID控制所需的所有状态变量，便于持续控制
 */
class PID {
public:
    float Kp;          // 比例系数 - 控制响应强度
    float Ki;          // 积分系数 - 消除静态误差
    float Kd;          // 微分系数 - 抑制超调和振荡
    float error;       // 当前误差 - 目标值与实际值的差
    float lastError;   // 上次误差 - 用于计算误差变化率
    float integral;    // 积分项 - 误差累积量
    
    /**
     * @brief 构造函数
     * 
     * @param kp 比例系数
     * @param ki 积分系数
     * @param kd 微分系数
     * @param err 初始误差
     * @param lastErr 初始上次误差
     */
    PID(float kp = 0, float ki = 0, float kd = 0, float err = 0, float lastErr = 0);
    
    /**
     * @brief 拷贝构造函数
     * 
     * @param other 源PID对象
     */
    PID(const PID& other);
    
    /**
     * @brief 赋值运算符
     * 
     * @param other 源PID对象
     * @return PID& 目标对象引用
     */
    PID& operator=(const PID& other);

    /**
     * @brief 增量式速度PID控制
     * 
     * 计算公式: △u = Kp*(e(k)-e(k-1)) + Ki*e(k)
     * 控制器输出的是增量，而非直接输出
     * 
     * @param targetVelocity 目标速度
     * @param currentVelocity 当前速度
     * @return int 控制器输出PWM值
     */
    int Velocity(int targetVelocity, int currentVelocity);
    
    /**
     * @brief 位置式PID控制
     * 
     * 计算公式: u = Kp*e(k) + Ki*∑e(i) + Kd*(e(k)-e(k-1))
     * 控制器输出与误差、误差积分、误差微分有关
     * 
     * @param now_position 当前位置
     * @param target_position 目标位置
     * @return float 控制器输出
     */
    float Position(int now_position, float target_position);
    
    /**
     * @brief 转向PD控制
     * 
     * 仅使用比例和微分项的控制器，适用于转向控制
     * 计算公式: u = Kp*e(k) + Kd*(e(k)-e(k-1))
     * 
     * @param now_position 当前位置
     * @param target_position 目标位置
     * @return float 控制器输出
     */
    float Turn(int now_position, float target_position);
};

/**
 * @brief 转向PID控制函数
 * 
 * @param now_position 当前位置
 * @param tar_position 目标位置
 * @return int 控制输出
 */
int turn_pid(int now_position, float tar_position);

/**
 * @brief PWM限幅函数
 * 
 * @param pwm 输入PWM值
 * @return float 限幅后的PWM值
 */
float pwm_control(float pwm);

/**
 * @brief 左轮速度环PID控制
 * 
 * @param TargetVelocity 目标速度
 * @param CurrentVelocity 当前速度
 * @return int 控制输出
 */
int LVelocity_FeedbackControl(int TargetVelocity, int CurrentVelocity);

/**
 * @brief 右轮速度环PID控制
 * 
 * @param TargetVelocity 目标速度
 * @param CurrentVelocity 当前速度
 * @return int 控制输出
 */
int RVelocity_FeedbackControl(int TargetVelocity, int CurrentVelocity);

/**
 * @brief 左轮位置环PID控制
 * 
 * @param now_position 当前位置
 * @param tar_position 目标位置
 * @return float 控制输出
 */
float p_pid(int16_t now_position, float tar_position);

/**
 * @brief 右轮位置环PID控制
 * 
 * @param now_position1 当前位置
 * @param tar_position1 目标位置
 * @return float 控制输出
 */
float p_pidR(int16_t now_position1, float tar_position1);

/**
 * @brief 速度限幅函数
 * 
 * @param PWM_P 输入PWM值
 * @param TargetVelocity 目标速度
 * @return int 限幅后的PWM值
 */
int Velocity_Restrict(int PWM_P, int TargetVelocity);

// 外部全局变量声明
extern int TargetVelocity, RControlVelocity, ControlVelocity, Last_bias, Last_bias1;
extern float p_p, p_d;                    // 转向环
extern float Velcity_Kp, Velcity_Ki, Velcity_Kd;    // 左轮速度环
extern float Velcity_RKp, Velcity_RKi, Velcity_RKd;  // 右轮速度环
extern float pid_p, pid_i, pid_d;         // 左轮位置环
extern float pid_p1, pid_i1, pid_d1;      // 右轮位置环
#endif
