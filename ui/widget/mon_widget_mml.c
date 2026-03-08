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

#define MAIN_LEVEL_DELAY        20                                              /**< LEVEL模式动画延时, 单位ms */



/**************************************************************************************************
 * @brief  : 创建mon level
 * @param  : None
 * @return : None
**************************************************************************************************/
static void create_mon_main_level(mon_widget_t *widget)
{
    int pos_y = MON_MAIN_POS_START;
    mon_core_t *core = &widget->core;
    mon_main_level_t *mml = &widget->mml;
    
    /* 创建16个灰色背景进度 */
    for(int i = 0 ; i < MON_MAIN_CNT ; i++)
    {
        mml->item[i] = create_mon_mml_cell(widget->parent, &core->style, LV_PALETTE_RED);
        lv_obj_set_y(mml->item[i], pos_y);
        pos_y -= MON_MAIN_INTERVAL;
    }
}

/**************************************************************************************************
 * @brief  : main level添加
 * @param  : None
 * @return : None
**************************************************************************************************/
static void mon_main_level_add(mon_main_level_t *mml)
{
    uint8_t start;

    if(mml->level >= MON_MAIN_LEVEL_MAX)
        return;

    /* 计算启动位置 */
    start = mml->level * MON_MAIN_LEVEL_STEP;

    /* 启动添加 */
    for(int i = start ; i < start + MON_MAIN_LEVEL_STEP ; i++)
    {
        lv_obj_clear_flag(mml->item[i], LV_OBJ_FLAG_HIDDEN);
        mon_update_now(MAIN_LEVEL_DELAY);
    }

    /* level ++ */
    mml->level++;
}

/**************************************************************************************************
 * @brief  : main level添加
 * @param  : None
 * @return : None
**************************************************************************************************/
static void mon_main_level_sub(mon_main_level_t *mml)
{
    uint8_t start;

    if(mml->level <= 0)
        return;

    /* 计算启动位置 */
    start = mml->level * MON_MAIN_LEVEL_STEP - 1;

    /* 启动减少 */
    for(int i = start ; i > start - MON_MAIN_LEVEL_STEP ; i--)
    {
        lv_obj_add_flag(mml->item[i], LV_OBJ_FLAG_HIDDEN);
        mon_update_now(MAIN_LEVEL_DELAY);
    }

    /* level ++ */
    mml->level--;
}

/**************************************************************************************************
 * @brief  : 设置mon 进度
 * @param  : None
 * @return : None
**************************************************************************************************/
void set_mon_main_level(mon_main_level_t *mml, mon_main_level_type_t level)
{
    switch(level)
    {
    case MON_MAIN_LEVEL1:
        mml->level = 0;
        mon_main_level_add(mml);
        break;

    case MON_MAIN_LEVEL_ADD:
        mon_main_level_add(mml);
        break;

    case MON_MAIN_LEVEL_SUB:
        mon_main_level_sub(mml);
        break;

    default:
        break;
    }
}

/**************************************************************************************************
 * @brief  : 设置模式 - level
 * @param  : None
 * @return : None
**************************************************************************************************/
void set_mon_mode_level(mon_widget_t *widget)
{
    mon_main_level_t *mml = &widget->mml;

    /* 设置进度条为红色 */
    set_mon_main_color(mml, LV_PALETTE_RED);                        // 设置颜色为红色
    mml->level = 0;                                                 // 设置level为0
}

/**************************************************************************************************
 * @brief  : 创建MMP - item
 * @param  : None
 * @return : None
**************************************************************************************************/
static void create_mon_mml_item(mon_widget_t *widget)
{
    int pos_y = MON_MAIN_POS_START;
    mon_core_t *core = &widget->core;
    mon_main_level_t *mml = &widget->mml;

    /* 创建16个进度 - 默认为红色 */
    for(int i = 0 ; i < MON_MAIN_CNT ; i++)
    {
        mml->item[i] = create_mon_mml_cell(widget->parent, &core->style, LV_PALETTE_RED);
        lv_obj_set_y(mml->item[i], pos_y);
        pos_y -= MON_MAIN_INTERVAL;
    }
}

/**************************************************************************************************
 * @brief  : MON - MML初始化, 主要LEVEL模式相关的初始化(代码绘制进度条)
 * @param  : None
 * @return : None
**************************************************************************************************/
void mon_mml_init(mon_widget_t *widget)
{
    mon_main_level_t *mml = &widget->mml;
    mml->color = LV_PALETTE_RED;
    create_mon_mml_item(widget);
}
