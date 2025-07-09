#include "pid.h"
#include "../Control/Control.h"
#include "wit.h"

float zangle =0;

// 简化版PID

/************
通用转向环或者角度环：输入目标位置和当前位置
*************/
int Turn_Pid(pid *pid,int now_position,float tar_position)//当前脉冲，目标脉冲 ,角速度
{
    float Err = tar_position - now_position;//目标脉冲-现在脉冲=误差脉冲
	// 限幅
		if(Err>pid->MAX) {Err -= pid->MAX * 2;}
    else if(Err<-pid->MIN) {Err += pid->MIN *2;} 
    // 计算PWM
    float pwm = pid->kp*Err + pid->kd*(Err-pid->last_err);
    // 更新误差
    pid->last_err = Err;
	// 返回PWM
    return pwm;
}

/************
增量式速度环
先加i消除误差，再加p消除静态误差
*************/
int FeedbackControl(pid *pid,int TargetVelocity, int CurrentVelocity)
{
		int Bias = TargetVelocity - CurrentVelocity; //求速度偏差
		
		pid->ControlVelocity += pid->kp * (Bias - pid->last_err) + pid->ki *Bias;  //增量式PI控制器
																		//pid->kp*(Bias-pid->last_err) 作用为限制加速度
																		//pid->ki*Bias             速度控制值由Bias不断积分得到 偏差越大加速度越大
		pid->last_err = Bias;	
		pid->ControlVelocity = (pid->ControlVelocity > pid->MAX) ? pid->MAX : ((pid->ControlVelocity < -pid->MIN) ? -pid->MIN : pid->ControlVelocity);//限幅

		// 使用三目运算符直接限制并返回PWM值
		return pid->ControlVelocity; //返回速度控制值
}

void mithon_run(pid *pid_speed_left, pid *pid_speed_right,pid *pid_turn_right, float i, float speed)
{
	// 计算当前角度与目标角度的差值，计算转向修正值
	int ADD = Turn_Pid(pid_turn_right, wit_data.yaw, i);   // 当前角度与目标角度
	
	// 左轮速度 = 基准速度 - 修正值
	int Lpwm = FeedbackControl(pid_speed_left, speed - ADD, pid_speed_left->now_speed);
	// 右轮速度 = 基准速度 + 修正值
	int Rpwm = FeedbackControl(pid_speed_right, speed + ADD, pid_speed_right->now_speed);

	Control_speed(Lpwm,Rpwm);
}

