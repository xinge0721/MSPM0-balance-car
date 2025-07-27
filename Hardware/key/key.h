#ifndef _KEY_H_
#define _KEY_H_

#include "system/sys/sys.h"

// 最多支持4个按键
#define KEY1_IN DL_GPIO_readPins(GPIO_PORT_0, GPIO_PIN_1)
#define KEY2_IN DL_GPIO_readPins(GPIO_PORT_0, GPIO_PIN_2)
#define KEY3_IN DL_GPIO_readPins(GPIO_PORT_0, GPIO_PIN_3)
#define KEY4_IN DL_GPIO_readPins(GPIO_PORT_0, GPIO_PIN_4)

void key_init(void);





#endif
