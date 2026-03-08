#ifndef __DRV_MISC_H__
#define __DRV_MISC_H__
#include <stdint.h>

void ir_init(void);
void ir_set(uint8_t val);
void ir_troggle(void);

void motor_init(void);
void motor_start(void);
void motor_stop(void);


#endif

