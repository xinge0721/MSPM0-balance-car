#include "topic.h"







// 题目一
void Topic_1(void)
{
    
}

// 题目二
void Topic_2(void)
{
    
}

uint32_t Nowposithon_left = 0;
uint32_t Nowposithon_right = 0;
uint32_t Nowposithon_all = 0;
uint8_t stopflag = 0;
static TrackingStatus tracking = 0;
// 题目三
void Topic_3(void)
{

    if(stopflag==0)  // 第一阶段：斜线行驶
    {
        // zhuanwan(-50,50);  // 左转50度，速度50
        PID_run(left,right,turn,-50,50);
        // 累计行驶距离
        Nowposithon_left  += left.now_speed; // 左轮行驶距离
        Nowposithon_right += right.now_speed; // 右轮行驶距离
        Nowposithon_all = (Nowposithon_left + Nowposithon_right) / 2; // 总行驶距离
        
        if(Nowposithonall >= 7500)  // 如果行驶距离达到设定值
        {
            // 重置位置计数器
            Nowposithon_left = 0;
            Nowposithon_right = 0;
            Nowposithon_all = 0;  
            stopflag = 1;  // 切换到第二阶段
        }
    }
    else if(stopflag==1)  // 第二阶段：直线行驶直到检测到黑线
    {
        tracking = xunji();  // 检测黑线
        PID_run(left,right,turn,1,50);  // 微调角度为1度，速度50
        
        if(tracking.line_detected==1) {  // 如果检测到黑线
            stopflag = 2;  // 切换到第三阶段
        }
    }
    else if(stopflag==2)  // 第三阶段：沿黑线行驶一段距离
    {

        tracking = xunji();  // 获取寻迹板数据
        PID_run_xunxian(left,right,turn,tracking.deviation,50);  // 根据偏差进行寻迹
    
        // 累计行驶距离
        Nowposithon_left += left.now_speed; // 左轮行驶距离
        Nowposithon_right += right.now_speed; // 右轮行驶距离
        Nowposithon_all = (Nowposithon_left + Nowposithon_right) / 2; // 总行驶距离
        
        if(Nowposithon_all >= 5000 && tracking.line_detected==0)  // 如果行驶距离达到设定值
        { 
            // 重置位置计数器
            Nowposithon_left = 0;
            Nowposithon_right = 0;
            Nowposithon_all = 0; 
            stopflag = 3;  // 切换到第四阶段
        }
    }
    else if(stopflag==3)  // 第四阶段：沿斜线行驶
    {
        PID_run(left,right,turn,-130,50);
        
        // 累计行驶距离
        Nowposithon_left += left.now_speed; // 左轮行驶距离
        Nowposithon_right += right.now_speed; // 右轮行驶距离
        Nowposithon_all = (Nowposithon_left + Nowposithon_right) / 2; // 总行驶距离
        
        if(Nowposithon_all >= 7500)  // 如果行驶距离达到设定值且tt为0
        {
            // 重置位置计数器
            Nowposithon_left = 0;
            Nowposithon_right = 0;
            Nowposithon_all = 0;  
            stopflag = 4;  // 切换到第五阶段
        }
    }
    else if(stopflag==4)  // 第五阶段：转向180度直到检测到黑线
    {
        tracking = xunji();  // 检测黑线
        PID_run(left,right,turn,178,50);  // 转向178度，速度50
        
        if(tracking.line_detected==1) {  // 如果检测到黑线
            stopflag = 5;  // 切换到第六阶段
        }
    }
    else if(stopflag==5)  // 第六阶段：沿黑线行驶一段距离
    {

        tracking = xunji();  // 获取寻迹板数据
        PID_run_xunxian(left,right,turn,tracking.deviation,50);  // 根据偏差进行寻迹
        
        // 累计行驶距离
        Nowposithon_left += left.now_speed; // 左轮行驶距离
        Nowposithon_right += right.now_speed; // 右轮行驶距离
        Nowposithon_all = (Nowposithon_left + Nowposithon_right) / 2; // 总行驶距离
        
        if(Nowposithon_all >= 7000 && tracking.line_detected==0)  // 如果行驶距离达到设定值且未检测到黑线
        { 
            // 重置位置计数器
            Nowposithon_left = 0;
            Nowposithon_right = 0;
            Nowposithon_all = 0; 
            stopflag = 6;  // 切换到最终停车阶段
        }
    }
    else if(stopflag==6)  // 最终阶段：停车
    {
        stopcar();  // 执行停车功能
    }
}


// 题目四
void Topic_4(void)
{

}
