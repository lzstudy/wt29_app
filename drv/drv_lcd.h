#ifndef __DRV_LCD_H__
#define __DRV_LCD_H__
#include <stdint.h>

void lcd_init(void);                                                            /**# lcd初始化 */
void lcd_bl_ctrl(uint8_t state);                                                /**# lcd背光控制 */

#endif