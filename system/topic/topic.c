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
        if(Time_cont > 50 &&  A_stable == 0)
        {
            Time_cont = 0;
            Topic_flag = 1;
        }
    }
    if(Topic_flag == 1)
    {
        mithon_run_pid(left.now_speed,right.now_speed,1200);
        if(target_position_right > 800 && target_position_left > 800 && A_stable == 1)
        {
            Time_cont = 0;
            Topic_flag = 2;
            clear_mileage();
            mithon_run_pid_clear();
        }
    }
    else if(Topic_flag == 2)
    {
        left.ki=5 ,left.kp=0.5; left.kd=0.0;//left.ki=110.0; left.kp=75.0; left.kd=8.0;
        right.ki=5; right.kp=0.5; right.kd=0.0;
        turn.kp=0.1; turn.kd=0.0;
        int err = process_sensors();
        mithon_run_xunxian(&left,&right,&turn,-err,30);//85
        total_distance = update_mileage();
        if(Time_cont > 50 && A_stable == 0)
        {
            Time_cont = 0;
            Topic_flag = 3;
            clear_mileage();
            // 记录当前角度
        }
    }
    else if(Topic_flag == 3)
    {
        stopcar();

    }
}

// 题目二
void Topic_2(void)
{
    
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
