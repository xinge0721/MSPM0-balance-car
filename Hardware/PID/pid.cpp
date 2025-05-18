#include "stm32f10x.h"                 
#include "pid.h"

int16_t myabs(int a)
{ 		   
    int temp;
    if(a < 0)  
        temp = -a;  
    else 
        temp = a;
    return temp;
}

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

// 转向环PID控制
int turn_pid(int now_position, float tar_position)
{
    static float turn_last_err;

    float turn_Err = tar_position - now_position; // 目标脉冲 - 现在脉冲 = 误差脉冲
    
    float turn_pwm = p_p * turn_Err + p_d * (turn_Err - turn_last_err);
    
    turn_last_err = turn_Err;
    return turn_pwm;
}
// PID控制器类定义
class PID {
public:
    float Kp;          // 比例系数
    float Ki;          // 积分系数
    float Kd;          // 微分系数
    float error;       // 误差
    float lastError;   // 上次误差
    float integral;    // 积分项
    
	// 没有默认构造函数，因为pid的参数怎么说也不能为0
    // 构造函数
    PID(float kp = 0, float ki = 0, float kd = 0, float err = 0, float lastErr = 0)
    {
        Kp = kp;
        Ki = ki;
        Kd = kd;
        error = err;
        lastError = lastErr;
        integral = 0;
    }
    
    // 拷贝构造函数
    PID(const PID& other)
    {
        Kp = other.Kp;
        Ki = other.Ki;
        Kd = other.Kd;
        error = other.error;
        lastError = other.lastError;
        integral = other.integral;
    }
    
    // 赋值运算符
    PID& operator=(const PID& other)
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

	// 增量式速度环
	
};

// 增量式速度环—左轮
float Velcity_Kp = 0, Velcity_Ki = 0, Velcity_Kd = 0; // 相关速度PID参数
int ControlVelocity = 0, Last_bias = 0, Bias = 0;

int LVelocity_FeedbackControl(int TargetVelocity, int CurrentVelocity)
{
    Bias = TargetVelocity - CurrentVelocity; // 求速度偏差
    
    ControlVelocity += Velcity_Kp * (Bias - Last_bias) + Velcity_Ki * Bias;
    Last_bias = Bias;	
    ControlVelocity = pwm_control(ControlVelocity); // 限幅
    return ControlVelocity; // 返回速度控制值
}


// 增量式速度环—右轮
float Velcity_RKp = 0, Velcity_RKi = 0, Velcity_RKd = 0; // 相关速度PID参数
int RControlVelocity = 0, Last_bias1 = 0, Bias1 = 0;

int RVelocity_FeedbackControl(int TargetVelocity, int CurrentVelocity)
{
    Bias1 = TargetVelocity - CurrentVelocity; // 求速度偏差
    
    RControlVelocity += Velcity_RKp * (Bias1 - Last_bias1) + Velcity_RKi * Bias1;
    Last_bias1 = Bias1;	
    RControlVelocity = pwm_control(RControlVelocity); // 限幅
    return RControlVelocity; // 返回速度控制值
}


// 位置式pid位置控制 - 左
float pos_Err = 0, pos_last_err = 0, Integral = 0, pos_pwm = 0, pid_p = 0, pid_i = 0, pid_d = 0;

float p_pid(int16_t now_position, float tar_position)
{
    now_position = myabs(now_position); // 转成正数
    pos_Err = tar_position - now_position; // 目标脉冲 - 现在脉冲 = 误差脉冲
    Integral += pos_Err;
    
    // 积分限幅
    if(Integral > 970) Integral = 970;
    if(Integral < -970) Integral = -970;
    
    pos_pwm = pid_p * pos_Err + pid_i * Integral + pid_d * (pos_Err - pos_last_err);
    pos_last_err = pos_Err;
    return pos_pwm;
}

// 位置式pid位置控制 - 右
float pos_Err1 = 0, pos_last_err1 = 0, Integral1 = 0, pos_pwm1 = 0, pid_p1 = 0, pid_i1 = 0, pid_d1 = 0;

float p_pidR(int16_t now_position1, float tar_position1)
{
    now_position1 = myabs(now_position1); // 转成正数
    pos_Err1 = tar_position1 - now_position1; // 目标脉冲 - 现在脉冲 = 误差脉冲
    Integral1 += pos_Err1;
    
    // 积分限幅
    if(Integral1 > 970) Integral1 = 970;
    if(Integral1 < -970) Integral1 = -970;
    
    pos_pwm1 = pid_p1 * pos_Err1 + pid_i1 * Integral1 + pid_d1 * (pos_Err1 - pos_last_err1);
    pos_last_err1 = pos_Err1;
    return pos_pwm1;
}

// 速度(PWM)限幅
int TargetVelocity = 0; // 此处可调速度
int Velocity_Restrict(int PWM_P, int TargetVelocity)
{
    if(PWM_P > +TargetVelocity * 37) 
        PWM_P = +TargetVelocity * 37;
    else if(PWM_P < -TargetVelocity * 37) 
        PWM_P = -TargetVelocity * 37;
    
    return PWM_P;
}


