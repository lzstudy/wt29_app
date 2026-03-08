#include "lvgl.h"
#include "bat_widget.h"
#include "pic.h"
#include "screen_utils.h"
#include "drv_analog_device.h"
#include "common.h"
#include "drv_battery.h"

#define CYCLE_BAT_CHECK             500                                        /**# 电量检测, 500ms检测一次 */ 

#define MON_BAT_CNT                 5                                           /**< 电池进度数量 */
#define MON_POS_X_START             49                                          /**< X起始位置 */
#define MON_POS_Y                   12                                          /**< Y位置 */


typedef struct _bat_widget {
    lv_obj_t *parent;                                                           /**@ 与screen main公用一个parent */
    uint8_t prog;                                                               /**@ 进度 */
    uint32_t time_bat_check;                                                    /**@ 电池检测时间累计 */
    bat_state_t stat;                                                           /**@ 状态 */
    int16_t power;                                                              /**@ 电量 */
    lv_obj_t *icon_bat_charging;                                                /**@ 充电中 */
    lv_obj_t *icon_bat_full;                                                    /**@ 充满电 */
    lv_obj_t *item[MON_BAT_CNT]
}bat_widget_t;

static bat_widget_t bat_widget;

/**************************************************************************************************
 * @brief  : 获取bat widget
 * @param  : None
 * @return : None
**************************************************************************************************/
static bat_widget_t *get_bat_widget(void)
{
    return &bat_widget;
}

/**************************************************************************************************
 * @brief  : 隐藏所有的进度
 * @param  : None
 * @return : None
**************************************************************************************************/
static void bat_progress_hidden(bat_widget_t *widget)
{
    for(int i = 0 ; i < MON_BAT_CNT ; i++)
    {
        lv_obj_set_visual(widget->item[i], 0);
    }
}

/**************************************************************************************************
 * @brief  : 设置进度
 * @param  : None
 * @return : None
**************************************************************************************************/
void bat_widget_set_progress(uint8_t progress)
{
    bat_widget_t *widget = get_bat_widget();

    /* 如果电量为0, 清空电量条 */
    if(progress == 0) {
        bat_progress_hidden(widget);
        return;
    }

    /* 传入进度大于当前进度, 增长1格 */
    if(progress > widget->prog) {
        lv_obj_set_visual(widget->item[progress - 1], 1);
        widget->prog = progress;
        return;
     }

     /* 传入进度小于当前进度, 减少1格*/
     if(progress < widget->prog) {
        lv_obj_set_visual(widget->item[progress - 1], 0);
        widget->prog = progress;
        return;
     }

     if(progress == widget->prog) {
        lv_obj_set_visual(widget->item[progress - 1], 1);
        widget->prog = progress;
        return;
     }
}

/**************************************************************************************************
 * @brief  : 电量转进度
 * @param  : None
 * @return : None
**************************************************************************************************/
static uint8_t power_to_progress(int16_t pow)
{
    if(pow < 2901)
        return 0;
    else if(pow >= 2901 && pow < 3038)
        return 1;
    else if(pow >= 3038 && pow < 3174)
        return 2;
    else if(pow >= 3174 && pow < 3310)
        return 3;
    else if(pow >= 3310 && pow < 3550)
        return 4;
    else
        return 5;
}

/**************************************************************************************************
 * @brief  : 创建mmp
 * @param  : None
 * @return : None
**************************************************************************************************/
static lv_obj_t *create_bat_cell(lv_obj_t *parent)
{
    lv_obj_t *image = lv_img_create(parent);
    lv_img_set_src(image, &pic_bat);

    lv_obj_align(image, LV_ALIGN_TOP_MID, 0, 0);
    return image;
}


/**************************************************************************************************
 * @brief  : 创建MMP - item
 * @param  : None
 * @return : None
**************************************************************************************************/
static void create_bat_item(bat_widget_t *widget)
{
    int pos_x = MON_POS_X_START;

    /* 创建4个进度 - 默认为蓝色 */
    for(int i = 0 ; i < MON_BAT_CNT ; i++)
    {
        widget->item[i] = create_bat_cell(widget->parent);
        lv_obj_set_pos(widget->item[i], pos_x, MON_POS_Y);
        lv_obj_set_visual(widget->item[i], 0);
        pos_x += 4;
    }
}

/**************************************************************************************************
 * @brief  : 创建bat图标
 * @param  : None
 * @return : None
**************************************************************************************************/
static void create_bat_icon(bat_widget_t *widget)
{
    /* 创建充电中图标 */
    widget->icon_bat_charging = lv_img_create(widget->parent);
    lv_img_set_src(widget->icon_bat_charging, &pic_bat_charging);
    lv_obj_set_pos(widget->icon_bat_charging, 115, 10);
    lv_obj_set_visual(widget->icon_bat_charging, 0);

    /* 创建充满电图标 */
    widget->icon_bat_full = lv_img_create(widget->parent);
    lv_img_set_src(widget->icon_bat_full, &pic_bat_full);
    lv_obj_set_pos(widget->icon_bat_full, 115, 10);
    lv_obj_set_visual(widget->icon_bat_full, 0);
}

/**************************************************************************************************
 * @brief  : 电量进度初始化
 * @param  : None
 * @return : None
**************************************************************************************************/
static void bat_progress_init(bat_widget_t *widget)
{    
    int16_t pow = get_analog_device_value(ANA_DEV_BAT);
    uint8_t progress = power_to_progress(pow);

    for(int i = 0 ; i < progress - 1; i++)
    {
        lv_obj_set_visual(widget->item[i], 1);
    }
}

/**************************************************************************************************
 * @brief  : 电池控件初始化
 * @param  : None
 * @return : None
**************************************************************************************************/
void bat_widget_init(lv_obj_t *parent)
{
    bat_widget_t *widget = &bat_widget;
    widget->parent = parent;

    create_bat_item(widget);
    create_bat_icon(widget);
    bat_progress_init(widget);
}

/**************************************************************************************************
 * @brief  : 未充电
 * @param  : None
 * @return : None
**************************************************************************************************/
static void bat_widget_uncharge(bat_widget_t *widget)
{
    int16_t pow;
    uint8_t progress;
    
    /* 获取电量 */
    pow = get_analog_device_value(ANA_DEV_BAT);
    progress = power_to_progress(pow);

    /* 设置进度条 */
    bat_widget_set_progress(progress);
    LOG_I("battery pow = %d %p", pow, widget);
}

/**************************************************************************************************
 * @brief  : 设置状态
 * @param  : None
 * @return : None
**************************************************************************************************/
void bat_widget_set_state(bat_state_t state, uint8_t prog)
{
    bat_widget_t *widget = get_bat_widget();

    /* 没有检测到引脚 */
    if(state == BAT_STAT_UNCHARGE)
    {
        lv_obj_set_visual(widget->icon_bat_charging, 0);
        lv_obj_set_visual(widget->icon_bat_full, 0);
        return;        
    }

    /* 最大进度, 表示充满 */
    if(prog >= MON_BAT_CNT && state == BAT_STAT_FULLY) {
        lv_obj_set_visual(widget->icon_bat_charging, 0);
        lv_obj_set_visual(widget->icon_bat_full, 1);
        return;
    }

    /* 充电中状态 */
    lv_obj_set_visual(widget->icon_bat_charging, 1);
    lv_obj_set_visual(widget->icon_bat_full, 0);

#if 0
    switch(state)
    {
    case BAT_STAT_UNCHARGE:
        lv_obj_set_visual(widget->icon_bat_charging, 0);
        lv_obj_set_visual(widget->icon_bat_full, 0);
        break;

    case BAT_STAT_CHARGING:
        lv_obj_set_visual(widget->icon_bat_charging, 1);
        lv_obj_set_visual(widget->icon_bat_full, 0);
        break;

    case BAT_STAT_FULLY:
        lv_obj_set_visual(widget->icon_bat_charging, 0);
        lv_obj_set_visual(widget->icon_bat_full, 1);
        break;
    }
#endif
}

/**************************************************************************************************
 * @brief  : bat widget运行
 * @param  : None
 * @return : None
**************************************************************************************************/
void bat_widget_run(void)
{
    bat_widget_t *widget = get_bat_widget();

    /* 获取电池状态 */
    widget->stat = get_bat_state();
    widget->power = get_analog_device_value(ANA_DEV_BAT);
    widget->prog = power_to_progress(widget->power);

    /* 设置进度 */
    bat_widget_set_progress(widget->prog);
    bat_widget_set_state(widget->stat, widget->prog);
}