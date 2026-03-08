#include "common.h"
#include "lvgl.h"
#include "screen_type.h"
#include "pic.h"
#include "screen_core.h"
#include "mon_widget.h"
#include "drv_misc.h"

typedef enum _camera_detect_level {
    CAMERA_DETECT_LEVEL0,                                                       /**< 关闭红外灯 */
    CAMERA_DETECT_LEVEL1,                                                       /**< 打开红外灯 */
    CAMERA_DETECT_LEVEL2,                                                       /**< 红外灯闪烁1 */
    CAMERA_DETECT_LEVEL3,                                                       /**< 红外灯闪烁2 */
    CAMERA_DETECT_LEVEL4,                                                       /**< 红外灯闪烁3 */
}camera_detect_level_t;

typedef enum _app_screen_stat {
    APP_STAT_FLASH,                                                              /**< 00' 闪烁态 */
    APP_STAT_IDLE,                                                               /**< 01' 空闲态 */
}app_screen_stat_t;

typedef struct _scr_detect_camera_cxt {
    uint8_t level;                                                              /**@ 红外灯level */
    uint8_t flash_cycle;                                                        /**@ 闪烁周期, 单位 (刷新周期) */
    uint8_t flash_time;                                                         /**@ 闪烁计数, 到达flash_cycle触发troggle */
    app_screen_stat_t stat;                                                     /**@ 应用状态 */
}scr_detect_camera_cxt_t;


/**************************************************************************************************
 * @brief  : detect_camera生命周期 - 进入
 * @param  : None
 * @return : None
**************************************************************************************************/
static void screen_life_post(void *arg)
{
    scr_detect_camera_cxt_t *cxt = calloc(sizeof(*cxt), 1);

    /* 设置状态为空闲态 */
    cxt->stat = APP_STAT_IDLE;

    /* 设置mon参数 */
    mon_widget_set_mode(MON_MODE_LEVEL);
    mon_widget_enable();

    /* 打开红外 */
    ir_set(1);

    /* 设置主进度条 */
    cxt->level = 1;
    mon_widget_main_attr_set(MON_MAIN_ATTR_LEVEL, (void *)MON_MAIN_LEVEL1);

    /* 保存上下文 */
    save_screen_cxt(cxt);
}

/**************************************************************************************************
 * @brief  : detect_camera生命周期 - 销毁
 * @param  : None
 * @return : None
**************************************************************************************************/
static void screen_life_destory(void *arg)
{
    scr_detect_camera_cxt_t *cxt = arg;

    mon_widget_disable();               // 隐藏mon
    ir_set(0);                          // 关闭红外
    free(cxt);
}

/**************************************************************************************************
 * @brief  : detect_camera生命周期 - 运行
 * @param  : None
 * @return : None
**************************************************************************************************/
static void app_ir_troggle(scr_detect_camera_cxt_t *cxt)
{
    cxt->flash_time++;

    if(cxt->flash_time >= cxt->flash_cycle)
    {
        ir_troggle();
        cxt->flash_time = 0;
    }
}

/**************************************************************************************************
 * @brief  : detect_camera生命周期 - 运行
 * @param  : None
 * @return : None
**************************************************************************************************/
static void screen_life_running(void *arg)
{
    scr_detect_camera_cxt_t *cxt = arg;

    switch(cxt->stat)
    {
    case APP_STAT_FLASH:
        app_ir_troggle(cxt);
        break;

    default:
        break;
    }
}


static screen_life_ops_t detect_camera_sl_ops = {
    .post = screen_life_post,
    .destory = screen_life_destory,
    .running = screen_life_running,
};


/**************************************************************************************************
 * @brief  : detect_camera生命周期 - 销毁
 * @param  : None
 * @return : None
**************************************************************************************************/
static void do_level_work(scr_detect_camera_cxt_t *cxt)
{
    switch(cxt->level)
    {
    case CAMERA_DETECT_LEVEL0:
        ir_set(0);
        cxt->stat = APP_STAT_IDLE;
        break;

    case CAMERA_DETECT_LEVEL1:
        ir_set(1);
        cxt->stat = APP_STAT_IDLE;
        break;

    case CAMERA_DETECT_LEVEL2:
        cxt->flash_time = 0;
        cxt->flash_cycle = 15;
        cxt->stat = APP_STAT_FLASH;
        break;

    case CAMERA_DETECT_LEVEL3:
        cxt->flash_time = 0;
        cxt->flash_cycle = 10;
        cxt->stat = APP_STAT_FLASH;
        break;

    case CAMERA_DETECT_LEVEL4:
        cxt->flash_time = 0;
        cxt->flash_cycle = 5;
        cxt->stat = APP_STAT_FLASH;
        break;

    default:
        break;
    }
}


/**************************************************************************************************
 * @brief  : detect_camera生命周期 - 销毁
 * @param  : None
 * @return : None
**************************************************************************************************/
static void key_up_event(void *arg)
{
    scr_detect_camera_cxt_t *cxt = arg;

    /* 更新进度 */
    mon_widget_main_attr_set(MON_MAIN_ATTR_LEVEL, (void *)MON_MAIN_LEVEL_ADD);
    mon_widget_main_attr_get(MON_MAIN_ATTR_LEVEL, (void *)&cxt->level);
    do_level_work(cxt);
}

/**************************************************************************************************
 * @brief  : detect_camera生命周期 - 销毁
 * @param  : None
 * @return : None
**************************************************************************************************/
static void key_down_event(void *arg)
{
    scr_detect_camera_cxt_t *cxt = arg;

    /* 更新进度 */
    mon_widget_main_attr_set(MON_MAIN_ATTR_LEVEL, (void *)MON_MAIN_LEVEL_SUB);
    mon_widget_main_attr_get(MON_MAIN_ATTR_LEVEL, (void *)&cxt->level);
    do_level_work(cxt);
}

/**************************************************************************************************
 * @brief  : detect_camera按键 - 控制
 * @param  : None
 * @return : None
**************************************************************************************************/
static void key_ctrl_event(void *arg)
{
    toggle_screen("main");
}


static screen_key_event_t detect_camera_sk_ev = {
    .up   = key_up_event,
    .down = key_down_event,
    .ctrl = key_ctrl_event,
};

gui_screen_t screen_detect_camera = {
    .name = "detect_camera",
    .sl_ops = &detect_camera_sl_ops,
    .sk_ev  = &detect_camera_sk_ev,
};



