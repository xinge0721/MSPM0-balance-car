#ifndef _INTERRUPT_H_
#define _INTERRUPT_H_

#include "system/sys/sys.h"
#include "ti_msp_dl_config.h"
#include "system/delay/delay.h"
#include "wit.h"

void Red_green_xy(void);
typedef struct{
	uint8_t rxbuffer[13];	//数据缓存区
	uint16_t len_flag;	//数据长度标志位
}rxbuf;
rxbuf rxflag;
rxbuf uart0;
extern double Red_xy[2];//xy的pid输出


#endif  /* #ifndef _INTERRUPT_H_ */