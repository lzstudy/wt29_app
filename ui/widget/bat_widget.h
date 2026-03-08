#ifndef __BAT_WIDGET_H__
#define __BAT_WIDGET_H__
#include <stdint.h>
#include "lvgl.h"
#include "drv_battery.h"

void bat_widget_init(lv_obj_t *parent);                                         /**# 初始化 */
void bat_widget_set_progress(uint8_t progress);                                 /**# 设置进度 */
void bat_widget_run(void);                                                      /**# bat自动管理 */

#endif
