#include "pid.h"
#include "../../system/sys/sys.h"
#include "../Control/Control.h"
#include "wit.h"

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