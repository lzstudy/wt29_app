#include "lvgl.h"
#include "nrf_delay.h"
#include "screen_core.h"
#include "mon_widget.h"
#include "common.h"
#include "text.h"
#include "pic.h"
#include "screen_utils.h"
#include "mon_widget_type.h"

#define MON_SUB_POS_X_START     45                                              /**< 起始位置(由左向右) */
#define MON_SUB_POS_Y           250                                             /**< Y位置, 固定 */
#define MON_SUB_W               2                                               /**< 从进度宽度 */
#define MON_SUB_H               6                                               /**< 从进度高度 */
#define MON_SUB_INTERVAL        6                                               /**< 从进度间隔 */

/* 符号 */
#define SYM_N_POS_X             25                                              /**< 负号x */
#define SYM_P_POS_X             140                                             /**< 正号x */
#define SYM_POS_Y               246                                             /**< 负号y */

/**************************************************************************************************
 * @brief  : 创建mon main item单元
 * @param  : None
 * @return : None
**************************************************************************************************/
static lv_obj_t *create_mon_msp_cell(lv_obj_t *parent, lv_style_t *style, lv_palette_t color)
{
    /* 创建矩形 */
    lv_obj_t *item = lv_obj_create(parent);
    lv_obj_set_size(item, MON_SUB_W, MON_SUB_H);
    lv_obj_set_style_border_width(item, 0, 0);

    /* 设置颜色 */
    lv_obj_set_style_bg_color(item, lv_palette_main(color), LV_PART_MAIN);
    lv_obj_add_style(item, style, LV_PART_MAIN);

    return item;
}

/**************************************************************************************************
 * @brief  : 设置mon 进度
 * @param  : None
 * @return : None
**************************************************************************************************/
void set_mon_sub_progress(mon_sub_prog_t *msp, uint8_t progress)
{
    /* 隐藏部分 */
    for(int i = progress ; i < MON_SUB_CNT ; i++)
    {
        lv_obj_add_flag(msp->item[i], LV_OBJ_FLAG_HIDDEN);
    }

    /* 显示部分 */
    for(int i = 0 ; i < progress ; i++)
    {
        lv_obj_clear_flag(msp->item[i], LV_OBJ_FLAG_HIDDEN);
    }    
}


/**************************************************************************************************
 * @brief  : 创建MMP - base : 灰色
 * @param  : None
 * @return : None
**************************************************************************************************/
static void create_mon_msp_base(mon_widget_t *widget)
{
    int pos_x = MON_SUB_POS_X_START;
    mon_core_t *core = &widget->core;
    mon_sub_prog_t *msp = &widget->msp;

    for(int i = 0 ; i < MON_SUB_CNT ; i++)
    {
        msp->base[i] = create_mon_msp_cell(widget->parent, &core->style, LV_PALETTE_GREY);
        lv_obj_set_style_bg_opa(msp->base[i], LV_OPA_30, LV_PART_MAIN);
        lv_obj_set_pos(msp->base[i], pos_x, MON_SUB_POS_Y);
        pos_x += MON_SUB_INTERVAL;
    }
}

/**************************************************************************************************
 * @brief  : 创建MMP - item ： 黄色
 * @param  : None
 * @return : None
**************************************************************************************************/
static void create_mon_msp_item(mon_widget_t *widget)
{
    int pos_x = MON_SUB_POS_X_START;
    mon_core_t *core = &widget->core;
    mon_sub_prog_t *msp = &widget->msp;

    for(int i = 0 ; i < MON_SUB_CNT ; i++)
    {
        msp->item[i] = create_mon_msp_cell(widget->parent, &core->style, LV_PALETTE_YELLOW);
        lv_obj_set_pos(msp->item[i], pos_x, MON_SUB_POS_Y);
        pos_x += MON_SUB_INTERVAL;
    }
}

/**************************************************************************************************
 * @brief  : 创建符号
 * @param  : None
 * @return : None
**************************************************************************************************/
static lv_obj_t *create_symbol_with_pos(lv_obj_t *parent, char *sym, int x, int y)
{
    lv_obj_t *obj = lv_label_create(parent);

    /* 设置字体和颜色 */
    lv_obj_set_style_text_color(obj, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_font(obj, &font1, 0);

    /* 设置文字内容 */
    lv_label_set_text(obj, sym);
    lv_obj_set_pos(obj, x, y);

    return obj;
}

/**************************************************************************************************
 * @brief  : 创建符号
 * @param  : None
 * @return : None
**************************************************************************************************/
static void create_symbol(mon_widget_t *widget)
{
    mon_sub_prog_t *msp = &widget->msp;

    /* 创建正负号 */
    msp->sym_n = create_symbol_with_pos(widget->parent, "-", SYM_N_POS_X, SYM_POS_Y);
    msp->sym_p = create_symbol_with_pos(widget->parent, "+", SYM_P_POS_X, SYM_POS_Y);
}

/**************************************************************************************************
 * @brief  : MON - MMP初始化, 主要PROGRESS模式相关的初始化(使用图片作为进度条)
 * @param  : None
 * @return : None
**************************************************************************************************/
void mon_msp_init(mon_widget_t *widget)
{
    mon_core_t *core = &widget->core;
    mon_sub_prog_t *msp = &widget->msp;

    create_mon_msp_base(widget);            // 创建子进度 - 背景
    create_mon_msp_item(widget);            // 创建子进度 - ITEM

    create_symbol(widget);                  // 创建符号 正负号
}

/**************************************************************************************************
 * @brief  : msp隐藏
 * @param  : None
 * @return : None
**************************************************************************************************/
static void mon_msp_enable(mon_sub_prog_t *msp)
{
    for(int i = 0 ; i < MON_SUB_CNT ; i++)
    {
        lv_obj_set_visual(msp->base[i], 1);
    }    

    lv_obj_set_visual(msp->sym_n, 1);
    lv_obj_set_visual(msp->sym_p, 1);
}


/**************************************************************************************************
 * @brief  : msp隐藏
 * @param  : None
 * @return : None
**************************************************************************************************/
static void mon_msp_disable(mon_sub_prog_t *msp)
{
    for(int i = 0 ; i < MON_SUB_CNT ; i++)
    {
        lv_obj_set_visual(msp->base[i], 0);
        lv_obj_set_visual(msp->item[i], 0);
    }

    lv_obj_set_visual(msp->sym_n, 0);
    lv_obj_set_visual(msp->sym_p, 0);
}

/**************************************************************************************************
 * @brief  : mmp隐藏
 * @param  : None
 * @return : None
**************************************************************************************************/
void mon_mon_sub_visual(mon_sub_prog_t *msp, uint8_t state)
{
    if(state)
        mon_msp_enable(msp);
    else
        mon_msp_disable(msp);
}
