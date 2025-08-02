#include "topic.h"
#include "Hardware/gray/gray.h"  // 包含循迹板相关定义
#include <stdlib.h>  // 包含abs函数
#include <math.h>    // 包含fabsf函数

uint8_t Topic_flag = 0;
// 拐弯角度值
uint8_t Topic_yaw = 0;


uint8_t Time_cont;

uint8_t zhuanwan = 0;
extern uint8_t target_laps;  // 使用外部定义的目标圈数
uint8_t quanshu = 4;
int small_target = 2200;
uint8_t chaoshi = 0;

static int prev_left_pulse = 0, prev_right_pulse = 0;
static int pulse_stable_count = 0;  // 脉冲值不变的连续计数

// 题目一
void Topic_1(void)
{
    int err = process_sensors();
    Time_cont++;
    int current_left_pulse = left_encoder_value;  // 替换为实际左轮脉冲变量
    int current_right_pulse = right_encoder_value; // 替换为实际右轮脉冲变量
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

        if(current_left_pulse == prev_left_pulse && 
           current_right_pulse == prev_right_pulse)
        {
            pulse_stable_count++;  // 脉冲值未变化，计数器增加
            
            // 连续5个周期脉冲值不变（可根据需要调整阈值）
            if(pulse_stable_count > 3) 
            {
                chaoshi = 1;  // 触发超时救援
            }
        }
        else 
        {
            pulse_stable_count = 0;  // 脉冲有变化，重置计数器
        }
        // 更新脉冲记录
        prev_left_pulse = current_left_pulse;
        prev_right_pulse = current_right_pulse;

        // 保险机制：如果位置环卡住不动，启动小位置环救援
        // 当Time_cont > 80且还没达到目标时，清除累计脉冲并启动小位置环
        // 原有超时条件保留
        if(Time_cont > 100 && Time_cont % 20 == 0 && 
           (target_position_right < 1600 || -target_position_left < 1600))
        {
            chaoshi = 1;
        }

        wei_pid_left.kp = 4.5;
        wei_pid_right.kp = 3.5;

        // 正常位置环控制
        if(chaoshi == 0)
            mithon_run_pid(left.now_speed,right.now_speed,small_target);
        else {
            // 触发差速转动（右正左负，原地右转）
            Control_speed(-800, 800);
            
            // 新增：超时状态下也检查是否达到目标
            if(target_position_right > 1500 && -target_position_left > 1500 && A_stable == 1)
            {
                chaoshi = 0;  // 退出超时状态
            }
        }

        // 正常完成条件
        if(target_position_right > 1500 && -target_position_left > 1500 && A_stable == 1)
        {
            chaoshi = 0;
            Time_cont = 0;
            zhuanwan++;
            if(zhuanwan == 4)  // 完成一圈（4个转弯）
            {
                target_laps--;     // 剩余圈数减1
                zhuanwan = 0;  // 重置转弯计数器，开始下一圈
            }
            if(target_laps == 0)   // 所有圈数完成
                Topic_flag = 3;  // 进入停车状态
            else 
                Topic_flag = 0;  // 继续下一圈或下一个转弯
            small_target = 1900;
            clear_mileage();
            mithon_run_pid_clear();
        }
    
    }
    else if (Topic_flag == 3)
    {
        stopcar();
        current_task_state = TASK_IDLE;  // 任务完成，重置为空闲状态
    }
}



// 题目三 - 自适应瞄准系统
void Topic_3(void)
{
    static uint8_t init_flag = 0;
    
    // 只在第一次调用时初始化
    if (init_flag == 0) {
        adaptive_aiming_init();
        init_flag = 1;
    }
    
    // 执行自适应瞄准更新
    adaptive_aiming_update();
}



// // 角度转换函数：输入角度+90度，结果保持在-180~180范围内
// float angle_add_90(float angle)
// {
//     float result = angle + 75.0f;
    
//     // 保持角度在-180到180范围内
//     if (result > 180.0f) {
//         result -= 360.0f;
//     } else if (result < -180.0f) {
//         result += 360.0f;
//     }
    
//     return result;
// }

// // 自适应瞄准控制器实例
AdaptiveAiming_t aiming_x;  // X轴瞄准控制器
AdaptiveAiming_t aiming_y;  // Y轴瞄准控制器

// 自适应瞄准初始化函数
void adaptive_aiming_init(void)
{
    // 初始化X轴瞄准控制器
    aiming_x.scale_factor = 1.0f;        // 初始比例系数
    aiming_x.last_error = 0;             // 上一次误差
    aiming_x.overshoot_count = 0;        // 过冲计数
    aiming_x.min_scale = 0.1f;           // 最小比例系数
    aiming_x.max_scale = 4.0f;           // 最大比例系数
    aiming_x.stable_count = 0;           // 稳定计数
    
    // 初始化Y轴瞄准控制器
    aiming_y.scale_factor = 1.0f;        
    aiming_y.last_error = 0;             
    aiming_y.overshoot_count = 0;        
    aiming_y.min_scale = 0.1f;           
    aiming_y.max_scale = 4.0f;           
    aiming_y.stable_count = 0;           
}

// 单轴自适应瞄准计算函数
float adaptive_aiming_calculate(AdaptiveAiming_t* aiming, int current_error, float base_step)
{
    // 如果误差很小，认为已经稳定
    if (abs(current_error) <= 3) {
        aiming->stable_count++;
        if (aiming->stable_count > 5) {
            return 0;  // 停止调整
        }
    } else {
        aiming->stable_count = 0;
    }
    
    // 检查是否发生过冲（误差符号变化）
    if ((aiming->last_error > 0 && current_error < 0) || 
        (aiming->last_error < 0 && current_error > 0)) {
        // 发生过冲，缩小比例系数
        aiming->overshoot_count++;
        aiming->scale_factor *= 0.8f;  // 每次过冲缩小到80%
        
        // 限制最小比例系数
        if (aiming->scale_factor < aiming->min_scale) {
            aiming->scale_factor = aiming->min_scale;
        }
    } else if (aiming->overshoot_count == 0) {
        // 如果没有发生过冲，可以适当增加比例系数（更积极）
        aiming->scale_factor *= 1.1f;  // 缓慢增加10%
        
        // 限制最大比例系数
        if (aiming->scale_factor > aiming->max_scale) {
            aiming->scale_factor = aiming->max_scale;
        }
    }
    
    // 计算控制量（基于误差方向和比例系数）
    float control_output = current_error * base_step * aiming->scale_factor;
    
    // 确保最小步长限制（绝对值至少为8）
    if (fabsf(control_output) > 0 && fabsf(control_output) < 8) {
        control_output = (control_output > 0) ? 8 : -8;
    }
    
    // 更新历史误差
    aiming->last_error = current_error;
    
    return control_output;
}

// 控制频率分频器（保持算法精髓的同时降低执行频率）
static uint8_t control_divider = 0;
static int last_target_x = 0, last_target_y = 0;

// 自适应瞄准主更新函数（保持算法精髓，优化稳定性）
void adaptive_aiming_update(void)
{
    // 智能分频：只有数据变化时或者达到最大等待时间才执行
    uint8_t should_execute = 0;
    
    // 检查数据是否更新
    if (target_angle_x != last_target_x || target_angle_y != last_target_y) {
        should_execute = 1;  // 数据更新，立即执行
        control_divider = 0;  // 重置分频器
        last_target_x = target_angle_x;
        last_target_y = target_angle_y;
    } else {
        // 数据未更新，使用分频器降低频率
        control_divider++;
        if (control_divider >= 1) {  // 每30ms强制执行一次
            should_execute = 1;
            control_divider = 0;
        }
    }
    
    if (!should_execute) {
        return;  // 跳过本次执行，保持算法学习状态
    }
    
    // 保持你的算法精髓：自适应基础步长计算
    float base_step_x = 0.3f;  // 稍微保守的基础步长
    float base_step_y = 0.3f;  // Y轴更保守
    
    // 你的算法核心：根据误差大小动态调整基础步长
    if (abs(target_angle_x) > 50) {
        base_step_x = 0.6f;  // 大误差时增大步长
    } else if (abs(target_angle_x) > 20) {
        base_step_x = 0.4f;  // 中等误差时中等步长
    }
    
    if (abs(target_angle_y) > 50) {
        base_step_y = 0.5f;  // Y轴大误差
    } else if (abs(target_angle_y) > 20) {
        base_step_y = 0.35f; // Y轴中等误差
    }
    
    // 保持你的算法精髓：自适应计算（会自动学习最佳比例系数）
    float control_x = adaptive_aiming_calculate(&aiming_x, target_angle_x, base_step_x);
    float control_y = -adaptive_aiming_calculate(&aiming_y, target_angle_y, base_step_y);
    
    // 根据OpenMv_status和按键一状态控制x轴转动方向
    // 只有当OpenMv_status==0且按下按键一时才反转
    // extern uint8_t key1_pressed;  // 声明外部变量
    // if(OpenMv_status == 0 && key1_pressed == 1) {
    //     control_x = -control_x;  // x轴转动角度取反
    // }
    // 其他情况（OpenMv_status!=0 或 没按按键一）都正常转动
    
    // 保持舵机最小步长限制的同时执行控制
    if (fabsf(control_x) >= 8) {
        int32_t new_position_x = (int32_t)(STS_Data[1].SignedPosition + control_x);
        control_position(1, new_position_x);
    }
    
    if (fabsf(control_y) >= 8) {
        int32_t new_position_y = (int32_t)(STS_Data[2].SignedPosition + control_y);
        control_position(2, new_position_y);
    }
}
