#ifndef __ENCODER_H
#define __ENCODER_H

#include "ti_msp_dl_config.h"
#include "system/sys/sys.h"

// 轮子参数（根据实际测量修改）
#define SIDE_DIAMETER  10     // 一条边的里程
#define WHEEL_DIAMETER  0.068f     // 轮子直径(米) 示例：65mm
#define WHEEL_CIRCUMFERENCE (3.1415926f * WHEEL_DIAMETER)  // 轮子周长
extern uint32_t total_distance;

extern volatile int Encoder_Left_Value ;
extern volatile int Encoder_Right_Value ;
extern volatile int Encoder_i_Value ;

// 编码器中断处理函数
void GROUP1_IRQHandler(void);

// 获取编码器值的函数（返回后会自动清零）
int Get_Encoder_Left(void);
int Get_Encoder_Right(void);
int update_mileage(void);
void clear_mileage(void);

#endif