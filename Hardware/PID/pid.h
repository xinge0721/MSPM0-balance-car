#ifndef __PID_H
#define __PID_H
#include "../../system/sys/sys.h"
// 定义PID结构体
typedef struct pid
{
	float kp;
	float ki;
	float kd;
	float last_err;
	uint16_t MAX;
	uint16_t MIN;
    int ControlVelocity;
}pid;


/************
通用转向环或者角度环：输入目标位置和当前位置
*************/
int Turn_Pid(pid *pid,int now_position,float tar_position);

/************
增量式速度环
先加i消除误差，再加p消除静态误差
*************/
int FeedbackControl(pid *pid,int TargetVelocity, int CurrentVelocity);


int turn_pid(int now_position,float tar_position);
float pwm_control(float pwm);
int LVelocity_FeedbackControl(int TargetVelocity, int CurrentVelocity);
int RVelocity_FeedbackControl(int TargetVelocity, int CurrentVelocity);
float p_pid(int16_t now_position,float tar_position);
float p_pidR(int16_t now_position1,float tar_position1);
int Velocity_Restrict(int PWM_P, int TargetVelocity);
extern int   TargetVelocity,RControlVelocity,ControlVelocity,Last_bias,Last_bias1;
extern float p_p,p_d;//转锟斤拷锟斤拷锟?
extern float P_angle_i;
extern float Velcity_Kp,  Velcity_Ki,  Velcity_Kd;//锟斤拷锟斤拷锟劫度伙拷
extern float Velcity_RKp, Velcity_RKi,  Velcity_RKd;//锟斤拷锟斤拷锟劫度伙拷
extern float pid_p,pid_i,pid_d;//锟斤拷锟斤拷位锟矫伙拷
extern float pid_p1,pid_i1,pid_d1;//锟斤拷锟斤拷位锟矫伙拷
#endif
