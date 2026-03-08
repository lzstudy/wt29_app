#include "common.h"
#include "lvgl.h"
#include "screen_type.h"
#include "pic.h"
#include "screen_core.h"
#include "mon_widget.h"
#include "screen_utils.h"
#include "drv_battery.h"
#include "bat_widget.h"

#define CYCLE_BAT_CHECK             500                                        /**# 电量检测, 500ms检测一次 */ 

typedef struct _scr_main_cxt {
    int index;                                                                  /**@ 当前位置 */
    lv_obj_t *parent;                                                           /**@ 背景 */
    lv_obj_t *item;                                                             /**@ 当前项 */
    uint32_t time_bat_check;                                                    /**@ 电池检测时间累计 */
}scr_main_cxt_t;

typedef struct _main_scr_item_cfg {
    const void *pic;                                                            /**@ 图片 */
    uint16_t pos;                                                               /**@ 位置 */
    const char *name;                                                           /**@ 跳转屏幕的名称 */
}main_scr_item_cfg_t;


static main_scr_item_cfg_t msi_cfg_tbl[] = {
    {&pic_main_item0,    30,    "detect_camera"},
    {&pic_main_item1,    70,    "detect_ir"},
    {&pic_main_item2,    113,   "detect_wless"},
    {&pic_main_item3,    154,   "detect_location"},
    {&pic_main_item4,    195,   "detect_magentic"},
    {&pic_main_item5,    236,   "shake_alarm"},
    {&pic_main_item6,    280,   "setting"},
};

static scr_main_cxt_t scr_main_cxt;                                             /**# 主屏幕采用静态创建方式, 永远不销毁 */


/**************************************************************************************************
 * @brief  : 创建背景
 * @param  : None
 * @return : None
**************************************************************************************************/
static void create_main_scr_bg(scr_main_cxt_t *cxt)
{
    cxt->parent = lv_img_create(lv_scr_act());
    lv_img_set_src(cxt->parent, &pic_main_menu);
}

/**************************************************************************************************
 * @brief  : 创建ITEM
 * @param  : None
 * @return : None
**************************************************************************************************/
static void switch_item(scr_main_cxt_t *cxt, uint8_t index)
{
    main_scr_item_cfg_t *item_info;

    /* 如果之前有值, 直接删除 */
    if(cxt->item && lv_obj_is_valid(cxt->item)) {
        lv_obj_del(cxt->item);
        cxt->item = NULL;
    }

    /* 获取配置信息 */
    item_info = &msi_cfg_tbl[index];

    /* 展示最新的ITEM */
    cxt->item = lv_img_create(lv_scr_act());
    lv_img_set_src(cxt->item, item_info->pic);
    lv_obj_align(cxt->item, LV_ALIGN_TOP_MID, 0, item_info->pos);
}

/**************************************************************************************************
 * @brief  : 主屏幕初始化
 * @param  : None
 * @return : None
**************************************************************************************************/
void main_screen_init(void)
{
    scr_main_cxt_t *cxt = &scr_main_cxt;

    cxt->index = 0;
    cxt->item = NULL;

    /* 创建背景 */
    create_main_scr_bg(cxt);

    /* 创建第0个ITEM */
    switch_item(cxt, 0);

    /* 创建bat */
    bat_widget_init(cxt->parent);

    /* 创建mon */
    mon_widget_init();
}



/**************************************************************************************************
 * @brief  : main生命周期 - 进入
 * @param  : None
 * @return : None
**************************************************************************************************/
static void screen_life_post(void *arg)
{
    save_screen_cxt(&scr_main_cxt);
}

/**************************************************************************************************
 * @brief  : main生命周期 - 销毁
 * @param  : None
 * @return : None
**************************************************************************************************/
static void screen_life_destory(void *arg)
{
    LOG_I("%s", __func__);
}

/**************************************************************************************************
 * @brief  : 检测电源电量
 * @param  : None
 * @return : None
**************************************************************************************************/
static void check_bat_power(scr_main_cxt_t *cxt)
{
    if(!check_cycle(&cxt->time_bat_check, CYCLE_BAT_CHECK))
        return;
        
    bat_widget_run();
}

/**************************************************************************************************
 * @brief  : main生命周期 - 运行
 * @param  : None
 * @return : None
**************************************************************************************************/
static void screen_life_running(void *arg)
{
    scr_main_cxt_t *cxt = (scr_main_cxt_t *)arg;

    /* 检测电池电量 */
    check_bat_power(cxt);
}


static screen_life_ops_t main_sl_ops = {
    .post = screen_life_post,
    .destory = screen_life_destory,
    .running = screen_life_running,
};

/**************************************************************************************************
 * @brief  : main生命周期 - 销毁
 * @param  : None
 * @return : None
**************************************************************************************************/
static void up_key_event(void *arg)
{
    scr_main_cxt_t *cxt = arg;

    cxt->index--;

    if(cxt->index < 0)
        cxt->index = ARRAY_SIZE(msi_cfg_tbl) - 1;

    switch_item(cxt, cxt->index);
}

/**************************************************************************************************
 * @brief  : main生命周期 - 销毁
 * @param  : None
 * @return : None
**************************************************************************************************/
static void down_key_event(void *arg)
{
    scr_main_cxt_t *cxt = arg;

    cxt->index = (cxt->index + 1) % ARRAY_SIZE(msi_cfg_tbl);
    switch_item(cxt, cxt->index);
}

/**************************************************************************************************
 * @brief  : main按键 - 控制
 * @param  : None
 * @return : None
**************************************************************************************************/
static void ctrl_key_event(void *arg)
{
    scr_main_cxt_t *cxt = arg;
    main_scr_item_cfg_t *item_info;

    item_info = &msi_cfg_tbl[cxt->index];
    toggle_screen(item_info->name);
}


static screen_key_event_t main_sk_ev = {
    .up   = up_key_event,
    .down = down_key_event,
    .ctrl = ctrl_key_event,
};

gui_screen_t screen_main = {
    .name = "main",
    .sl_ops = &main_sl_ops,
    .sk_ev  = &main_sk_ev,
};



