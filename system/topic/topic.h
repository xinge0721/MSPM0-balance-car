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


#endif