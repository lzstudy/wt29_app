#include "lvgl.h"
#include "nrf_delay.h"
#include "screen_core.h"
#include "mon_widget.h"
#include "screen_core.h"
#include "common.h"
#include "text.h"
#include "pic.h"
#include "screen_utils.h"
#include "mon_widget_type.h"
#include "mon_widget_utils.h"
#include "mon_widget_interface.h"
#include "beep_manager.h"
#include "motor_manager.h"
#include "timer_manager.h"

static mon_widget_t mon_widget;                                                 /**# mon部件 */


/*=====================================================================================================> 10 工具部分 */
/**************************************************************************************************
 * @brief  : 获取mon widget
 * @param  : None
 * @return : None
**************************************************************************************************/
mon_widget_t *get_mon_widget(void)
{
    return &mon_widget;
}

/**************************************************************************************************
 * @brief  : 创建main base
 * @param  : None
 * @return : None
**************************************************************************************************/
static void create_mon_main_base(mon_widget_t *widget)
{
    int pos_y = MON_MAIN_POS_START;
    mon_core_t *core = &widget->core;

    /* 创建16个灰色背景进度 */
    for(int i = 0 ; i < MON_MAIN_CNT ; i++)
    {
        core->base[i] = create_mon_mml_cell(widget->parent, &core->style, LV_PALETTE_GREY);
        lv_obj_set_style_bg_opa(core->base[i], LV_OPA_30, LV_PART_MAIN);
        lv_obj_set_y(core->base[i], pos_y);
        pos_y -= MON_MAIN_INTERVAL;
    }

    /* 保存结束位置 */
    core->main_pos_end = pos_y;
}

/*=====================================================================================================> 09 主属性部分 */
/**************************************************************************************************
 * @brief  : 隐藏主进度 - 镜像格式
 * @param  : None
 * @return : None
**************************************************************************************************/
static void hidden_mon_mmp(mon_main_prog_t *mmp)
{
    for(int i = 0 ; i < MON_MAIN_CNT ; i++)
    {
        lv_obj_add_flag(mmp->item[i], LV_OBJ_FLAG_HIDDEN);
    }
}


/**************************************************************************************************
 * @brief  : 隐藏主进度 - 镜像格式
 * @param  : None
 * @return : None
**************************************************************************************************/
static void hidden_mon_mml(mon_main_level_t *mml)
{
    for(int i = 0 ; i < MON_MAIN_CNT ; i++)
    {
        lv_obj_add_flag(mml->item[i], LV_OBJ_FLAG_HIDDEN);
    }
}

/**************************************************************************************************
 * @brief  : 隐藏主进度 - ITEM格式
 * @param  : None
 * @return : None
**************************************************************************************************/
static void hidden_mon_mms(mon_main_scan_t *mms)
{
    lv_obj_add_flag(mms->item, LV_OBJ_FLAG_HIDDEN);
}

/**************************************************************************************************
 * @brief  : 隐藏主进度上的所有的ITEM : 红色进度、蓝色进度、扫描条
 * @param  : None
 * @return : None
**************************************************************************************************/
static void hidden_mon_main_item(mon_widget_t *widget)
{
    hidden_mon_mmp(&widget->mmp);                   // 清mmp进度条  
    hidden_mon_mml(&widget->mml);                   // 清mml进度条
    hidden_mon_mms(&widget->mms);                   // 清ITEM格式的进度条
}

/**************************************************************************************************
 * @brief  : mon设备停止
 * @param  : None
 * @return : None
**************************************************************************************************/
static inline void mon_device_stop(mon_main_prog_t *mmp)
{
    motor_manager()->stop();
    timer_manager()->stop();
    beep_manager()->stop();
    lv_obj_set_visual(mmp->item[0], 0);
}

/**************************************************************************************************
 * @brief  : 设置mon 进度
 * @param  : None
 * @return : None
**************************************************************************************************/
void set_mon_main_progress(mon_main_prog_t *mmp, uint8_t progress)
{
    /* 进度为0 停止相关硬件 */
    if(progress == 0) {
        mon_device_stop(mmp);
        return;
    }

    set_main_progress(mmp, progress - 1);
}



/**************************************************************************************************
 * @brief  : 设置main进度条颜色
 * @param  : None
 * @return : None
**************************************************************************************************/
void set_mon_main_color(mon_main_level_t *mml, lv_palette_t color)
{
    /* 设置颜色  */
    for(int i = 0 ; i < MON_MAIN_CNT ; i++)
    {
        lv_obj_set_style_bg_color(mml->item[i], lv_palette_main(color), LV_PART_MAIN);
    }
}



/**************************************************************************************************
 * @brief  : 设置属性
 * @param  : None
 * @return : None
**************************************************************************************************/
void mon_widget_main_attr_set(mon_widget_main_attr_t attr, void *data)
{
    mon_widget_t *widget = &mon_widget;

    switch(attr)
    {
    case MON_MAIN_ATTR_PROG:                    // 设置进度
        set_mon_main_progress(&widget->mmp, (uint16_t)data);
        break;

    case MON_MAIN_ATTR_LEVEL:                   // 设置level
        set_mon_main_level(&widget->mml, (uint8_t)data);
        break;

    case MON_MAIN_ATTR_COLOR:                   // 设置颜色
        set_mon_main_color(&widget->mml, (lv_palette_t)data);
        break;

    default:
        break;
    }
}

/**************************************************************************************************
 * @brief  : 获取属性值
 * @param  : None
 * @return : None
**************************************************************************************************/
void mon_widget_main_attr_get(mon_widget_main_attr_t attr, void *data)
{
    mon_widget_t *widget = &mon_widget;

    switch (attr)
    {
    case MON_MAIN_ATTR_LEVEL: {
        uint8_t *val = (uint8_t *)data;
        *val = widget->mml.level;
        }
        break;
    
    default:
        break;
    }
}

/**************************************************************************************************
 * @brief  : 设置属性
 * @param  : None
 * @return : None
**************************************************************************************************/
void mon_widget_sub_attr_set(mon_widget_sub_attr_t attr, void *data)
{
    mon_widget_t *widget = &mon_widget;

    switch(attr)
    {
    case MON_SUB_ATTR_ENABLE:
        mon_mon_sub_visual(&widget->msp, (uint8_t)data);
        break;


    case MON_SUB_ATTR_PROG:
        set_mon_sub_progress(&widget->msp, (uint8_t)data);
        break;

    default:
        break;
    }
}


/**************************************************************************************************
 * @brief  : 设置部件模式
 * @param  : None
 * @return : None
**************************************************************************************************/
void mon_widget_set_mode(mon_widget_mode_t mode)
{
    mon_widget_t *widget = &mon_widget;

    /* 清主ITEM */
    hidden_mon_main_item(widget);

    switch(mode)
    {
    case MON_MODE_PROG:             // 进度模式
        set_mon_mode_prog(widget);
        break;

    case MON_MODE_LEVEL:            // LEVEL模式
        set_mon_mode_level(widget);
        break;

    case MON_MODE_SCAN:             // 扫描模式
        set_mon_mode_scan(widget);
        break;
    }

    /* 保存模式 */
    widget->core.mode = mode;
}


/*=====================================================================================================> 02 接口部分 */
/**************************************************************************************************
 * @brief  : 隐藏widget
 * @param  : None
 * @return : None
**************************************************************************************************/
void mon_widget_disable(void)
{
    mon_widget_t *widget = &mon_widget;

    /* 隐藏parent */
    lv_obj_add_flag(widget->parent, LV_OBJ_FLAG_HIDDEN);

    /* 清主进度 */
    mon_widget_main_attr_set(MON_MAIN_ATTR_PROG, (void *)0);

    /* 清子进度 */
    mon_widget_sub_attr_set(MON_SUB_ATTR_PROG, (void *)0);       // 清0进度
    mon_widget_sub_attr_set(MON_SUB_ATTR_ENABLE, (void *)0);     // 子进度关闭

    /* 关闭beep */
    motor_manager()->stop();
    timer_manager()->stop();
    beep_manager()->stop();
}

/**************************************************************************************************
 * @brief  : mon widget show, 将mon widget放到前景，并显示
 * @param  : None
 * @return : None
**************************************************************************************************/
void mon_widget_enable(void)
{
    mon_widget_t *widget = &mon_widget;
    lv_obj_move_foreground(widget->parent);
    lv_obj_clear_flag(widget->parent, LV_OBJ_FLAG_HIDDEN);
}


/*=====================================================================================================> 01 初始化部分 */
/**************************************************************************************************
 * @brief  : 创建mon parent(主要用于设置背景)
 * @param  : None
 * @return : None
**************************************************************************************************/
static void mon_parent_init(mon_widget_t *widget)
{
    widget->parent = lv_img_create(lv_scr_act());
    lv_obj_set_style_bg_opa(widget->parent, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_bg_color(widget->parent, lv_color_black(), 0);
    lv_obj_set_size(widget->parent, lv_disp_get_hor_res(NULL), lv_disp_get_ver_res(NULL));
    lv_obj_set_style_border_width(widget->parent, 0, 0);
    lv_obj_add_flag(widget->parent, LV_OBJ_FLAG_HIDDEN);
}

/**************************************************************************************************
 * @brief  : mon核心初始化
 * @param  : None
 * @return : None
**************************************************************************************************/
static void mon_core_init(mon_widget_t *widget)
{
    mon_core_t *core = &widget->core;

    /* 设置矩形为标准矩形 */
    lv_style_init(&core->style);
    lv_style_set_radius(&core->style, 0);

    /* 设置模式 */
    core->mode = MON_MODE_PROG;

    /* 创建主背景进度条 */
    create_mon_main_base(widget);
}


/**************************************************************************************************
 * @brief  : 背景mon 控件 ：背景 + 进度条构成
 * @param  : None
 * @return : None
**************************************************************************************************/
void mon_widget_init(void)
{
    mon_widget_t *widget = &mon_widget;

    /* 父对象初始化 */
    mon_parent_init(widget);

    /* 核心初始化 */
    mon_core_init(widget);

    /* 主进度相关初始化 */
    mon_mml_init(widget);           // mml初始化
    mon_mmp_init(widget);           // mmp初始化
    mon_mms_init(widget);           // mms初始化

    /* 从进度初始化 */
    mon_msp_init(widget);           // msp初始化
    
    /* 其他部分初始化 */
    mon_misc_init(widget);          // 其他部分 
}
