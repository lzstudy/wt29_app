#ifndef __MON_WIDGET_INTERFACE_H__
#define __MON_WIDGET_INTERFACE_H__
#include "mon_widget_type.h"

/* 扫描相关接口 */
void mon_mms_init(mon_widget_t *widget);                    // scan初始化
void mon_misc_init(mon_widget_t *widget);

/* 模式设置 */
void set_mon_mode_prog(mon_widget_t *widget);
void set_mon_mode_scan(mon_widget_t *widget);

/* 主进度相关 */
void set_mon_main_progress(mon_main_prog_t *mmp, uint8_t progress);
void set_mon_main_level(mon_main_level_t *mml, mon_main_level_type_t level);
void set_mon_main_color(mon_main_level_t *mml, lv_palette_t color);

/* 子进度相关接口 */
void set_mon_sub_progress(mon_sub_prog_t *msp, uint8_t progress);
void mon_mon_sub_visual(mon_sub_prog_t *msp, uint8_t state);


#endif