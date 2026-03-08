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

#define MON_MAIN_SCAN_CYCLE     30                                              /**< 主扫描动画周期, 单位ms */


/**************************************************************************************************
 * @brief  : 设置扫描模式
 * @param  : None
 * @return : None
**************************************************************************************************/
void set_mon_mode_scan(mon_widget_t *widget)
{
    mon_main_scan_t *mms = &widget->mms;

    /* 设置方向 - 向上开始扫描 */
    mms->direct = 1;

    /* 扫描条初始位置 - 设置在中间 */
    mms->pos = MON_MAIN_POS_START - (MON_MAIN_INTERVAL * 5);

    /* 设置位置 */
    lv_obj_set_y(mms->item, mms->pos);

    /* 显示扫描条 */
    lv_obj_clear_flag(mms->item, LV_OBJ_FLAG_HIDDEN);

    /* 初始化周期 */
    mms->time = 0;
}

/**************************************************************************************************
 * @brief  : mon scan
 * @param  : None
 * @return : None
**************************************************************************************************/
static void do_mms_scan(mon_main_scan_t *mms, uint16_t pos_end)
{
    if(mms->direct)
    {
        lv_obj_set_y(mms->item, mms->pos);
        mms->pos -= MON_MAIN_INTERVAL;

        /* 切换向下扫描 */
        if(mms->pos <= pos_end + MON_MAIN_INTERVAL)
            mms->direct = 0;
    }
    else
    {
        lv_obj_set_y(mms->item, mms->pos);
        mms->pos += MON_MAIN_INTERVAL;

        /* 切换扫描方向 */
        if(mms->pos >= MON_MAIN_POS_START)
            mms->direct = 1;
    }
}

/**************************************************************************************************
 * @brief  : 主进度扫描, 需要外部定时调用来刷新扫描条
 * @param  : None
 * @return : None
**************************************************************************************************/
void mon_widget_main_scan(void)
{
    mon_widget_t *widget = get_mon_widget();
    mon_main_scan_t *mms = &widget->mms;

    /* 定时刷新 */
    mms->time += get_ui_refresh_time();
    if(mms->time >= MON_MAIN_SCAN_CYCLE) {
        do_mms_scan(mms, widget->core.main_pos_end);
        mms->time = 0;
    }
}

/**************************************************************************************************
 * @brief  : MON - MMS初始化, SCAN模式相关的初始化(扫描条相关)
 * @param  : None
 * @return : None
**************************************************************************************************/
void mon_mms_init(mon_widget_t *widget)
{
    mon_core_t *core = &widget->core;
    mon_main_scan_t *mms = &widget->mms;

    /* 设置基础属性 */
    mms->color = LV_PALETTE_RED;
    mms->direct = 1;
    mms->time = 0;
    mms->pos = MON_MAIN_POS_START - (MON_MAIN_INTERVAL * 5);

    /* 创建扫描条, 隐藏 */
    mms->item = create_mon_mml_cell(widget->parent, &core->style, LV_PALETTE_RED);
    lv_obj_add_flag(mms->item, LV_OBJ_FLAG_HIDDEN);
}
