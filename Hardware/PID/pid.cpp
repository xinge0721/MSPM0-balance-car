#include "pid.h"
#include <cmath>
#include <stdint.h> // 添加标准整数类型定义

/**
 * @brief PWM限幅函数
 * 
 * 将PWM输出限制在±3599范围内，防止超出电机驱动的有效范围
 * 
 * @param pwm 输入PWM值
 * @return float 限幅后的PWM值
 */
float pwm_control(float pwm)
{
    if(pwm > 3599)
        pwm = 3599;
    else if(pwm < -3599)
        pwm = -3599;
    return pwm;
}

// 转向环全局变量
float p_p = 0.6, p_d = 2;

/**
 * @brief PID类构造函数实现
 * 
 * 初始化PID控制器的所有参数
 */
PID::PID(float kp, float ki, float kd, float err, float lastErr)
{
    Kp = kp;
    Ki = ki;
    Kd = kd;
    error = err;
    lastError = lastErr;
    integral = 0;
}

/**
 * @brief PID类拷贝构造函数实现
 * 
 * 从另一个PID对象复制所有参数
 */
PID::PID(const PID& other)
{
    Kp = other.Kp;
    Ki = other.Ki;
    Kd = other.Kd;
    error = other.error;
    lastError = other.lastError;
    integral = other.integral;
}

/**
 * @brief PID类赋值运算符实现
 * 
 * 将另一个PID对象的所有参数赋值给当前对象
 */
PID& PID::operator=(const PID& other)
{
    if (this != &other)
    {
        Kp = other.Kp;
        Ki = other.Ki;
        Kd = other.Kd;
        error = other.error;
        lastError = other.lastError;
        integral = other.integral;
    }
    return *this;
}

/**
 * @brief 增量式速度PID控制实现
 * 
 * 计算公式: △u = Kp*(e(k)-e(k-1)) + Ki*e(k)
 * 增量式PID只输出控制量的增量，适合于直接控制执行机构的场合
 * 具有抗饱和性能，不需要设置抗积分饱和
 */
int PID::Velocity(int targetVelocity, int currentVelocity)
{
    error = targetVelocity - currentVelocity; // 求速度偏差
    int controlOutput = Kp * (error - lastError) + Ki * error;
    lastError = error;	
    controlOutput = pwm_control(controlOutput); // 限幅
    return controlOutput;
}

/**
 * @brief 位置式PID控制实现
 * 
 * 计算公式: u = Kp*e(k) + Ki*∑e(i) + Kd*(e(k)-e(k-1))
 * 位置式PID直接输出控制量，适合于位置控制
 * 需要设置积分限幅以防止积分饱和
 */
float PID::Position(int now_position, float target_position)
{
    now_position = abs(now_position); // 转成正数
    error = target_position - now_position; // 目标脉冲 - 现在脉冲 = 误差脉冲
    integral += error;

    // 积分限幅 - 防止积分饱和导致系统响应变慢或产生震荡
    if(integral > 970) integral = 970;
    if(integral < -970) integral = -970;

    float output = Kp * error + Ki * integral + Kd * (error - lastError);
    lastError = error;
    return output;
}

/**
 * @brief 转向PD控制实现
 * 
 * 转向控制通常只需要比例和微分项，不需要积分项
 * 比例项使系统快速接近目标，微分项抑制振荡
 */
float PID::Turn(int now_position, float target_position)
{
    static float lastErr;

    error = target_position - now_position; // 目标脉冲 - 现在脉冲 = 误差脉冲

    float output = Kp * error + Kd * (error - lastErr);

    lastErr = error;
    return output;											
}

/**
 * @brief 转向PID控制函数
 * 
 * 使用静态PID对象实现转向控制，保持控制状态的连续性
 * 由于是静态对象，每次调用时会保留上次的状态
 */
int turn_pid(int now_position, float tar_position)
{
    static PID turnPID(p_p, 0, p_d);
    return turnPID.Turn(now_position, tar_position);
}

// 增量式速度环—左轮
float Velcity_Kp = 0, Velcity_Ki = 0, Velcity_Kd = 0; // 相关速度PID参数
int ControlVelocity = 0, Last_bias = 0, Bias = 0;

/**
 * @brief 左轮速度环PID控制
 * 
 * 使用静态PID对象实现左轮速度控制
 * 每次调用前从全局变量更新PID参数，保持与原代码的兼容性
 * 调用后更新全局状态变量，方便外部访问和调试
 */
int LVelocity_FeedbackControl(int TargetVelocity, int CurrentVelocity)
{
    static PID leftVelocityPID;
    
    // 更新PID参数 - 允许外部动态调整PID参数
    leftVelocityPID.Kp = Velcity_Kp;
    leftVelocityPID.Ki = Velcity_Ki;
    leftVelocityPID.Kd = Velcity_Kd;
    
    // 计算控制量
    ControlVelocity = leftVelocityPID.Velocity(TargetVelocity, CurrentVelocity);
    
    // 保存用于兼容旧代码的全局变量
    Bias = leftVelocityPID.error;
    Last_bias = leftVelocityPID.lastError;
    
    return ControlVelocity; // 返回速度控制值
}


// 增量式速度环—右轮
float Velcity_RKp = 0, Velcity_RKi = 0, Velcity_RKd = 0; // 相关速度PID参数
int RControlVelocity = 0, Last_bias1 = 0, Bias1 = 0;

/**
 * @brief 右轮速度环PID控制
 * 
 * 使用静态PID对象实现右轮速度控制
 * 每次调用前从全局变量更新PID参数，保持与原代码的兼容性
 * 调用后更新全局状态变量，方便外部访问和调试
 */
int RVelocity_FeedbackControl(int TargetVelocity, int CurrentVelocity)
{
    static PID rightVelocityPID;
    
    // 更新PID参数
    rightVelocityPID.Kp = Velcity_RKp;
    rightVelocityPID.Ki = Velcity_RKi;
    rightVelocityPID.Kd = Velcity_RKd;
    
    // 计算控制量
    RControlVelocity = rightVelocityPID.Velocity(TargetVelocity, CurrentVelocity);
    
    // 保存用于兼容旧代码的全局变量
    Bias1 = rightVelocityPID.error;
    Last_bias1 = rightVelocityPID.lastError;
    
    return RControlVelocity; // 返回速度控制值
}


// 位置式pid位置控制 - 左
float pos_Err = 0, pos_last_err = 0, Integral = 0, pos_pwm = 0, pid_p = 0, pid_i = 0, pid_d = 0;

/**
 * @brief 左轮位置环PID控制
 * 
 * 使用静态PID对象实现左轮位置控制
 * 每次调用前从全局变量更新PID参数，保持与原代码的兼容性
 * 位置环通常作为外环，控制车轮的目标位置
 */
float p_pid(int16_t now_position, float tar_position)
{
    static PID leftPositionPID;
    
    // 更新PID参数
    leftPositionPID.Kp = pid_p;
    leftPositionPID.Ki = pid_i;
    leftPositionPID.Kd = pid_d;
    
    // 计算控制量
    pos_pwm = leftPositionPID.Position(now_position, tar_position);
    
    // 保存用于兼容旧代码的全局变量
    pos_Err = leftPositionPID.error;
    pos_last_err = leftPositionPID.lastError;
    Integral = leftPositionPID.integral;
    
    return pos_pwm;
}

// 位置式pid位置控制 - 右
float pos_Err1 = 0, pos_last_err1 = 0, Integral1 = 0, pos_pwm1 = 0, pid_p1 = 0, pid_i1 = 0, pid_d1 = 0;

/**
 * @brief 右轮位置环PID控制
 * 
 * 使用静态PID对象实现右轮位置控制
 * 每次调用前从全局变量更新PID参数，保持与原代码的兼容性
 * 位置环通常作为外环，控制车轮的目标位置
 */
float p_pidR(int16_t now_position1, float tar_position1)
{
    static PID rightPositionPID;
    
    // 更新PID参数
    rightPositionPID.Kp = pid_p1;
    rightPositionPID.Ki = pid_i1;
    rightPositionPID.Kd = pid_d1;
    
    // 计算控制量
    pos_pwm1 = rightPositionPID.Position(now_position1, tar_position1);
    
    // 保存用于兼容旧代码的全局变量
    pos_Err1 = rightPositionPID.error;
    pos_last_err1 = rightPositionPID.lastError;
    Integral1 = rightPositionPID.integral;
    
    return pos_pwm1;
}

// 速度(PWM)限幅
int TargetVelocity = 0; // 此处可调速度	

/**
 * @brief 速度(PWM)限幅函数
 * 
 * 根据目标速度动态计算PWM限幅值
 * 限幅值与目标速度成正比，系数为37
 * 
 * @param PWM_P 输入PWM值
 * @param TargetVelocity 目标速度
 * @return int 限幅后的PWM值
 */
int Velocity_Restrict(int PWM_P, int TargetVelocity)
{
    if(PWM_P > +TargetVelocity * 37) 
        PWM_P = +TargetVelocity * 37;
    else if(PWM_P < -TargetVelocity * 37) 
        PWM_P = -TargetVelocity * 37;
    
    return PWM_P;
}


