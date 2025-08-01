#include "pid.h"
#include "../../system/sys/sys.h"
#include "../Control/Control.h"
#include "wit.h"
#include <stdlib.h>

float zangle =0;

pid right = {0,0,0,0,0,0};
pid left = {0,0,0,0,0,0};
pid turn = {0,0,0,0,0,0};


float pwm_control(float pwm)
{
    if(pwm>3999)
        pwm=3999;
    else if(pwm<-3999)
        pwm=-3999;
    return pwm;
}
// 简化版PID

/************
通用转向环或者角度环：输入目标位置和当前位置
*************/
int Turn_Pid(pid *pid,int now_position,float tar_position)//当前脉冲，目标脉�?? ,角速度
{
    float Err = tar_position - now_position;//目标脉冲-现在脉冲=误差脉冲
	// 限幅
	if(Err>180) {Err -= 360;}
    else if(Err<-180) {Err += 360;} 
    // 计算PWM
    float pwm = pid->kp*Err + pid->kd*(Err-pid->last_err);
    // 更新误差
    pid->last_err = Err;
	// 返回PWM
    return pwm;
}

/************
增量式速度�??
先加i消除误差，再加p消除静态误�??
*************/
int FeedbackControl(pid *pid,int TargetVelocity, int CurrentVelocity)
{
		int Bias = TargetVelocity - CurrentVelocity; //求速度偏差
		
		pid->ControlVelocity += pid->kp * (Bias - pid->last_err) + pid->ki *Bias+pid->kd*(Bias - 2*pid->last_err + pid->last_last_err);  	//增量式PI控制

		pid->last_last_err=pid->last_err;
		pid->last_err = Bias;

		pid->ControlVelocity = pwm_control(pid->ControlVelocity);
       
		// 使用三目运算符直接限制并返回PWM
		return pid->ControlVelocity; //返回速度控制
}

/************
位置式速度环
先加i消除误差，再加p消除静态误
*************/
int cControl(pid *pid,int TargetVelocity, int CurrentVelocity)
{
		int Bias = TargetVelocity - CurrentVelocity; //求速度偏差
		pid->ControlVelocity = pid->kp * (Bias - pid->last_err) + pid->ki *Bias;  //增量式PI控制器
		pid->last_err = Bias;	

		pid->ControlVelocity = pwm_control(pid->ControlVelocity);

		return pid->ControlVelocity; //返回速度控制值
}


void mithon_run_yew(pid *pid_speed_left, pid *pid_speed_right,pid *pid_turn_right, float t_yew, float speed)
{
	// 计算当前角度与目标角度的差值，计算转向修正
	int ADD = Turn_Pid(pid_turn_right, wit_data.yaw, t_yew);   // 当前角度与目标角
	
	// 左轮速度 = 基准速度 - 修正
	int Lpwm = FeedbackControl(pid_speed_left, speed - ADD, pid_speed_left->now_speed);
	// 右轮速度 = 基准速度 + 修正
	int Rpwm = FeedbackControl(pid_speed_right, speed + ADD, pid_speed_right->now_speed);

	Control_speed(Lpwm,Rpwm);
}

void mithon_run_xunxian(pid *pid_speed_left, pid *pid_speed_right,pid *pid_turn_right, float angle, float speed)
{
	// 计算当前角度与目标角度的差值，计算转向修正
	int ADD = Turn_Pid(pid_turn_right, angle, 0);   // 当前角度与目标角
	
	// 左轮速度 = 基准速度 - 修正
	int Lpwm = FeedbackControl(pid_speed_left, speed - ADD, pid_speed_left->now_speed);
	// 右轮速度 = 基准速度 + 修正
	int Rpwm = FeedbackControl(pid_speed_right, speed + ADD, pid_speed_right->now_speed);

	Control_speed(Lpwm,Rpwm);
}

void stopcar(void)//刹车
{
	left.ki=10; left.kp=6.5;
    right.ki=10; right.kp=6.5;
	// 左轮速度 = 基准速度 - 修正值
	int Lpwm = cControl(&left, 0, left.now_speed);
	// 右轮速度 = 基准速度 + 修正值
	int Rpwm = cControl(&right, 0, right.now_speed);
	Control_speed(Lpwm,Rpwm);

}

typedef struct {
	float p_last_err;
	float Integral;
	float kp;
	float ki;
	float kd;
}wei_pid;

wei_pid wei_pid_left = {0,0,3.5,0,0};
wei_pid wei_pid_right = {0,0,2.2,0,0};

//位置式pid位置控制
//左
float wei_pid_run(wei_pid *pid,int16_t now_position,float tar_position)//积累脉冲(现在脉冲个数)，目标脉冲
{
    now_position=abs(now_position);//转成正数
    float p_Err=tar_position-now_position;//目标脉冲-现在脉冲=误差脉冲
	pid->Integral+=p_Err;
	if(pid->Integral> 970) pid->Integral= 970;	//积分限幅
	if(pid->Integral<-970) pid->Integral=-970;	//积分限幅
	//kp参数如果过大，设置电机转一圈就会过冲在回位，调到回位误差很小，这时候就可以调kd压制过冲
    float p_pwm=pid->kp*p_Err+pid->ki*pid->Integral+pid->kd*(p_Err-pid->p_last_err);
    p_pwm=pwm_control(p_pwm);
    pid->p_last_err=p_Err;
    return p_pwm;
}


// 位置式pid位置控制
// 参数一：左轮速度
// 参数二：右轮速度
// 参数三：目标脉冲
// 右轮累计目标值
int target_position_right = 0;
// 左轮累计目标值
int target_position_left = 0;

// 转向位置环pid
void mithon_run_pid(int pid_speed_left, int pid_speed_right, int now_position)
{
	target_position_right += pid_speed_right;
	target_position_left += pid_speed_left;
	
	// 左轮速度 = 基准速度 - 修正
	int Lpwm = wei_pid_run(&wei_pid_left,target_position_left, now_position);
	// 右轮速度 = 基准速度 + 修正
	int Rpwm = wei_pid_run(&wei_pid_right,target_position_right, now_position);

	Control_speed(-Lpwm,Rpwm);
}

// 清空位置环累计脉冲
void mithon_run_pid_clear(void)
{
	target_position_right = 0;
	target_position_left = 0;
}

//////////////////////////////
//舵机PID
/////////////////////////////

struct PID_TypeDef
{
	float	KP;
	float	KI;
	float	KD;
	float	Bias;
	float	Pwm;
	float	Integral_bias;
	float	Last_Bias;
};

struct PID_TypeDef PID_X = {1,0,0,0,0,0};
struct PID_TypeDef PID_Y = {0,0,0,0,0,0};
//struct PID_TypeDef PID_Y = {2.5,0,15,0,0,0};
/*************************************************************************
函数功能：位置式PID控制器
入口参数：编码器测量位置信息，目标位置
返回  值：电机PWM增量
**************************************************************************/
// float Position_PID_X(float Position,float Target)
// { 	                                          //增量输出
// 	PID_X.Bias=Target-Position;                                  //计算偏差
// 	PID_X.Integral_bias+=PID_X.Bias;	                                 //求出偏差的积分
// 	PID_X.Pwm=PID_X.KP*PID_X.Bias/100+PID_X.KI*PID_X.Integral_bias/100+PID_X.KD*(PID_X.Bias-PID_X.Last_Bias)/100;       //位置式PID控制器
// 	 PID_X.Last_Bias=PID_X.Bias;                                       //保存上一次偏差 
// 	 return PID_X.Pwm;  
// }

float Position_PID_X(float Position,float Target)
{ 	                                          //增量输出
	PID_X.Bias=Target-Position;                                  //计算偏差
	PID_X.Integral_bias+=PID_X.Bias;	                                 //求出偏差的积分
	PID_X.Pwm=PID_X.KP*PID_X.Bias+PID_X.KI*PID_X.Integral_bias+PID_X.KD*(PID_X.Bias-PID_X.Last_Bias);       //位置式PID控制器
	 PID_X.Last_Bias=PID_X.Bias;                                       //保存上一次偏差 
	 return PID_X.Pwm;  
}
/*************************************************************************
函数功能：位置式PID控制器
入口参数：编码器测量位置信息，目标位置
返回  值：电机PWM增量
**************************************************************************/


float Position_PID_Y(float Position,float Target)
{ 	                                         //增量输出
	 PID_Y.Bias=Target-Position;                                  //计算偏差
	 PID_Y.Integral_bias+=PID_Y.Bias;	                                 //求出偏差的积分
	 PID_Y.Pwm=PID_Y.KP*PID_Y.Bias/100+PID_Y.KI*PID_Y.Integral_bias/100+PID_Y.KD*(PID_Y.Bias-PID_Y.Last_Bias)/100;       //位置式PID控制器
	 PID_Y.Last_Bias=PID_Y.Bias;                                       //保存上一次偏差 
	 return PID_Y.Pwm;  
}
