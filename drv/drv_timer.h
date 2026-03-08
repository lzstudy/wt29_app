#ifndef __DRV_TIMER_H__
#define __DRV_TIMER_H__
#include <stdint.h>


void timer_init(void);
void timer_start(void);
void timer_stop(void);
void timer_restart(uint16_t cycle);
#endif