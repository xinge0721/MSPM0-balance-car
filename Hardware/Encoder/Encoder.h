#ifndef __ENCODER_H
#define __ENCODER_H

#include "ti_msp_dl_config.h"
#include "system/sys/sys.h"

extern volatile int Encoder_Left_Value ;
extern volatile int Encoder_Right_Value ;
extern volatile int Encoder_i_Value ;




// 编码器中断处理函数
void GROUP1_IRQHandler(void);

// 获取编码器值的函数（返回后会自动清零）
int Get_Encoder_Left(void);
int Get_Encoder_Right(void);

#endif