#ifndef __HTS221_H
#define __HTS221_H

#include "../../sys.h"

void HTS221_Init(void);
void HTS221_turn(uint16_t angle, uint16_t speed, uint8_t ID);
void HTS221_stop(uint8_t ID);
void HTS221_getAngle(uint8_t ID);
void HTS221_setAngleLimit(uint16_t minAngle, uint16_t maxAngle, uint8_t ID);


#endif
