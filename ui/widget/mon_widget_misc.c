#include "lvgl.h"
#include "nrf_delay.h"
#include "screen_core.h"
#include "mon_widget.h"
#include "common.h"
#include "text.h"
#include "pic.h"
#include "screen_utils.h"
#include "mon_widget_utils.h"
#include "mon_widget_type.h"

/* 文本 */
#define FOOTER_POS_X            10                                              /**< footer x */
#define FOOTER_POS_Y            280                                             /**< footer y */

/* home图表 */
#define LOGO_HOME_X             15                                              /**< HOME_X */
#define LOGO_HOME_Y             278                                             /**< HOME_Y */

/**************************************************************************************************
 * @brief  : 创建Home图标
 * @param  : None
 * @return : None
**************************************************************************************************/
static void create_footer_home(mon_widget_t *widget)
{
    mon_misc_t *misc = &widget->misc;

    misc->home = lv_img_create(widget->parent);
    lv_img_set_src(misc->home, &pic_home);
    lv_obj_set_style_border_width(misc->home, 0, 0);
    lv_obj_set_pos(misc->home, LOGO_HOME_X, LOGO_HOME_Y);
}

/**************************************************************************************************
 * @brief  : 创建lable
 * @param  : None
 * @return : None
**************************************************************************************************/
static void create_footer_lable(mon_widget_t *widget)
{
    mon_misc_t *misc = &widget->misc;

    /* 创建页脚 */
    misc->footer = lv_label_create(widget->parent);

    /* 设置字体和颜色 */
    lv_obj_set_style_text_color(misc->footer, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_font(misc->footer, &font1, 0);

    /* 设置文字内容 */
    lv_label_set_text(misc->footer, text_back_home);
    lv_obj_align(misc->footer, LV_ALIGN_TOP_MID, FOOTER_POS_X, FOOTER_POS_Y);
}

/**************************************************************************************************
 * @brief  : 创建页脚文本
 * @param  : None
 * @return : None
**************************************************************************************************/
static void create_footer(mon_widget_t *widget)
{
    create_footer_home(widget);
    create_footer_lable(widget);
}



/**************************************************************************************************
 * @brief  : 背景mon 控件 : MISC初始化
 * @param  : None
 * @return : None
**************************************************************************************************/
void mon_misc_init(mon_widget_t *widget)
{
    
    create_footer(widget);                      // 创建页脚文本 - 点击电源键返回首页
}