#ifndef __DRV_ST7789_H__
#define __DRV_ST7789_H__

#include <stdint.h>

void st7789_init(void);
void st7789_bl_ctrl(uint8_t state);
void set_lcd_power_state(uint8_t state);

#endif
