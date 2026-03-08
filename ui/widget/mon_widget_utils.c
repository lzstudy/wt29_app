#include "lvgl.h"
#include "nrf_delay.h"
#include "screen_core.h"
#include "mon_widget.h"
#include "common.h"
#include "text.h"
#include "pic.h"
#include "screen_utils.h"
#include "mon_widget_type.h"
#include "mon_widget_utils.h"

/**************************************************************************************************
 * @brief  : 创建mon widget 更新
 * @param  : None
 * @return : None
**************************************************************************************************/
void mon_update_now(int time)
{
    lv_timer_handler();
    lv_refr_now(NULL);
    nrf_delay_ms(time);
}


/**************************************************************************************************
 * @brief  : 创建mon main item单元
 * @param  : None
 * @return : None
**************************************************************************************************/
lv_obj_t *create_mon_mml_cell(lv_obj_t *parent, lv_style_t *style, lv_palette_t color)
{
    /* 创建矩形 */
    lv_obj_t *item = lv_obj_create(parent);
    lv_obj_set_size(item, MON_MAIN_W, MON_MAIN_H);
    lv_obj_align(item, LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_set_style_border_width(item, 0, 0);

    /* 设置颜色 */
    lv_obj_set_style_bg_color(item, lv_palette_main(color), LV_PART_MAIN);
    lv_obj_add_style(item, style, LV_PART_MAIN);

    return item;
}
