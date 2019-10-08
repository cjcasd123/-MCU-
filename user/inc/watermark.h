#ifndef __WATERMARK_H__
#define __WATERMARK_H__
#include "stc8.h"
#include "type.h"

#define enableExtiInterrupt()		do{EX0 = 1; EX1 = 1;}while(0)
#define disableExtiInterrupt()		do{EX0 = 0; EX1 = 0;}while(0)

void p_watermark_init(void);

void p_watermark_ctrl(uint8_t sy1, uint8_t sy2);

#endif 