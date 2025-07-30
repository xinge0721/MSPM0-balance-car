#ifndef __PID_H
#define __PID_H
#include "../../system/sys/sys.h"
#include "../Control/Control.h"
#include "wit.h"
// 定义PID结构
typedef struct pid
{
	float kp; // 比例系数
	float ki; // 积分系数
	float kd; // 微分系数
	signed long long last_err; // 上一次误差
	signed long long last_last_err; // 上一次误差
    int ControlVelocity; // 控制速度
	int now_speed; // 当前速度
}pid;

extern float Integral;

// 右轮PID
extern pid right;
// 左轮PID
extern pid left;
// 旋转PID
extern pid turn;
// 位置式pid位置控制
// 参数一：左轮速度
// 参数二：右轮速度
// 参数三：目标脉冲
// 右轮累计目标值
extern int target_position_right ;
// 左轮累计目标值
extern int target_position_left ;


#define PID_run(pid_speed_left,pid_speed_right,pid_turn_right,i,speed) mithon_run(&pid_speed_left,&pid_speed_right,&pid_turn_right,i,speed)

#define PID_run_xunxian(pid_speed_left,pid_speed_right,pid_turn_right,i,speed) mithon_run_xunxian(&pid_speed_left,&pid_speed_right,&pid_turn_right,i,speed)

/************
通用转向环或者角度环：输入目标位置和当前位置
*************/
int Turn_Pid(pid *pid,int now_position,float tar_position);

/************
增量式速度
先加i消除误差，再加p消除静态误差
*************/
int FeedbackControl(pid *pid,int TargetVelocity, int CurrentVelocity);

void mithon_run_yew(pid *pid_speed_left, pid *pid_speed_right,pid *pid_turn_right, float t_yew, float speed);

void mithon_run_xunxian(pid *pid_speed_left, pid *pid_speed_right,pid *pid_turn_right, float angle, float speed);

void stopcar(void);//刹车

//位置式pid位置控制
//左
float p_pid(int16_t now_position,float tar_position);
float p_pidR(int16_t now_position1,float tar_position1);

// 转向位置环pid
void mithon_run_pid(int pid_speed_left, int pid_speed_right, int now_position);
// 清空位置环累计脉冲
void mithon_run_pid_clear(void);

#endif
