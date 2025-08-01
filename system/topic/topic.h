#ifndef __TOPIC_H__
#define __TOPIC_H__



#include "system/sys/sys.h"

extern uint8_t Topic_flag;

// 防误判结构体
typedef struct {
    uint8_t line_detected_count;    // 连续检测到线的次数
    uint8_t line_lost_count;        // 连续丢失线的次数
    uint8_t distance_stable_count;  // 距离稳定计数
    uint32_t last_distance;         // 上次距离值
    uint8_t confirmation_needed;    // 需要确认的次数
} AntiMisjudgment_t;

extern AntiMisjudgment_t anti_judge;

// 题目一
void Topic_1(void);
// 题目二
void Topic_2(void);
// 题目三
void Topic_3(void);
// 题目四
void Topic_4(void);

// 角度转换函数：输入角度+90度，结果保持在-180~180范围内
float angle_add_90(float angle);

// 自适应瞄准结构体
typedef struct {
    float scale_factor;          // 当前比例系数
    int last_error;              // 上一次的误差值
    uint8_t overshoot_count;     // 过冲计数
    float min_scale;             // 最小比例系数
    float max_scale;             // 最大比例系数
    uint8_t stable_count;        // 稳定计数
} AdaptiveAiming_t;

// 自适应瞄准函数
void adaptive_aiming_update(void);
void adaptive_aiming_init(void);


#endif