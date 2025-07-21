#ifndef __PID_H
#define __PID_H
#include "../../system/sys/sys.h"
#include "../Control/Control.h"
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
	int now_speed;
}pid;

// 类似引用的宏定义，防呆
// 参数一：左轮PID
// 参数二：右轮PID
// 参数三：转向PID
// 参数四：i
// 参数五：速度
#define PID_run(pid_speed_left,pid_speed_right,pid_turn_right,i,speed) mithon_run(&pid_speed_left,&pid_speed_right,&pid_turn_right,i,speed)
#define PID_run_xunxian(pid_speed_left,pid_speed_right,pid_turn_right,i,speed) mithon_run_xunxian(&pid_speed_left,&pid_speed_right,&pid_turn_right,i,speed)

/************
通用转向环或者角度环：输入目标位置和当前位置
*************/
int Turn_Pid(pid *pid,int now_position,float tar_position);

/************
增量式速度环
先加i消除误差，再加p消除静态误差
*************/
int FeedbackControl(pid *pid,int TargetVelocity, int CurrentVelocity);

void mithon_run(pid *pid_speed_left, pid *pid_speed_right,pid *pid_turn_right, float i, float speed);


#endif
