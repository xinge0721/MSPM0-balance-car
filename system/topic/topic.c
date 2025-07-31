#include "topic.h"
#include "Hardware/gray/gray.h"  // 包含循迹板相关定义

uint8_t Topic_flag = 0;
// 拐弯角度值
uint8_t Topic_yaw = 0;


uint8_t Time_cont;

// 题目一
void Topic_1(void)
{
    int err = process_sensors();
    Time_cont++;
    if(Topic_flag == 0)
    {
        left.ki=5 ,left.kp=0.5; left.kd=0.0;//left.ki=110.0; left.kp=75.0; left.kd=8.0;
        right.ki=5; right.kp=0.5; right.kd=0.0;
        turn.kp=0.1; turn.kd=0.0;
        mithon_run_xunxian(&left,&right,&turn,-err,30);//85
        total_distance = update_mileage();
        if(Time_cont > 40 &&  A_stable == 0)
        {
            Time_cont = 0;
            Topic_flag = 1;
        }
    }
    if(Topic_flag == 1)
    {
        // 保险机制：如果位置环卡住不动，启动小位置环救援
        // 当Time_cont > 80且还没达到目标时，清除累计脉冲并启动小位置环
        if(Time_cont > 80 && Time_cont % 20 == 0 && 
           (target_position_right < 1600 || -target_position_left < 1600))
        {
            // 计算还需要的脉冲数
            int remaining_right = 1800 - target_position_right;
            int remaining_left = 1800 - (-target_position_left);
            int remaining_avg = (remaining_right + remaining_left) / 2;
            
            // 清除累计脉冲，重新开始一个小的位置环
            mithon_run_pid_clear();
            
            // 启动小位置环，目标为剩余距离的一部分（比如1/4）
            int small_target = remaining_avg / 4;
            if(small_target < 100) small_target = 100; // 最小推力
            if(small_target > 400) small_target = 400; // 最大推力
            
            mithon_run_pid(left.now_speed, right.now_speed, small_target);
        }
        else
        {
            // 正常位置环控制
            mithon_run_pid(left.now_speed,right.now_speed,1800);
        }
        
        // 正常完成条件
        if(target_position_right > 800 && -target_position_left > 800 && A_stable == 1)
        {
            Time_cont = 0;
            Topic_flag = 0;
            clear_mileage();
            mithon_run_pid_clear();
        }
        
        // 强制完成条件（防止无限循环）
        if(Time_cont > 200)
        {
            Time_cont = 0;
            Topic_flag = 0;
            clear_mileage();
            mithon_run_pid_clear();
        }
    }
}

// 题目二
float Position1 = 30,Position2 = 30; //舵机当前位置

void Topic_2(void)
{
    float Velocity1 = Position_PID_1(Position1,300);
    // float Velocity2 = Position_PID_2(Position2,0);

    Position1 += Velocity1;
    // Position2 += Velocity2;

    Set_Servo1_Angle((uint16_t)Position1);
    // Set_Servo2_Angle((uint16_t)Position2);

}


// 题目三
void Topic_3(void)
{

    
}


// 题目四
void Topic_4(void)
{

}

// 角度转换函数：输入角度+90度，结果保持在-180~180范围内
float angle_add_90(float angle)
{
    float result = angle + 75.0f;
    
    // 保持角度在-180到180范围内
    if (result > 180.0f) {
        result -= 360.0f;
    } else if (result < -180.0f) {
        result += 360.0f;
    }
    
    return result;
}
