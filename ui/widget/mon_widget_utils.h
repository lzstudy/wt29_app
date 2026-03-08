#ifndef __MON_WIDGET_UTILS_H__
#define __MON_WIDGET_UTILS_H__
#include "lvgl.h"

void mon_update_now(int time);
lv_obj_t *create_mon_mml_cell(lv_obj_t *parent, lv_style_t *style, lv_palette_t color);

#endif
