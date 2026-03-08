#include "common.h"
#include "lvgl.h"
#include "screen_type.h"
#include "pic.h"
#include "screen_core.h"
#include "mon_widget.h"
#include "drv_analog_device.h"
#include "screen_utils.h"

#define WLESS_SIGNAL_MAX      200

typedef struct _scr_detect_wless_cxt {
    uint8_t prog_slave;                                                         /**@ slave进度 */
    uint16_t time_read_wless;                                                   /**@ 读取检波器周期 */
    sliding_filter_t filter;                                                    /**@ 滤波器 */
}scr_detect_wless_cxt_t;


/**************************************************************************************************
 * @brief  : detect_wless生命周期 - 进入
 * @param  : None
 * @return : None
**************************************************************************************************/
static void screen_life_post(void *arg)
{
    scr_detect_wless_cxt_t *cxt = calloc(sizeof(*cxt), 1);

    /* 设置mon参数 */
    mon_widget_set_mode(MON_MODE_PROG);                         // 设置 进度 模式
    mon_widget_enable();                                        // 使能widget
    mon_widget_sub_attr_set(MON_SUB_ATTR_ENABLE, (void *)1);    // 使能子进度

    /* 设置默认进度 */
    cxt->time_read_wless = 0;

    /* 保存上下文 */
    save_screen_cxt(cxt);
}

/**************************************************************************************************
 * @brief  : detect_wless生命周期 - 销毁
 * @param  : None
 * @return : None
**************************************************************************************************/
static void screen_life_destory(void *arg)
{
    scr_detect_wless_cxt_t *cxt = arg;

    mon_widget_disable();                               // 隐藏mon
    free(cxt);                                          // 释放cxt
}

/**************************************************************************************************
 * @brief  : 处理跟踪tag业务
 * @param  : None
 * @return : None
**************************************************************************************************/
static int wless_val_to_prog(uint16_t value)
{
    int precent, prog;

    precent = value * 100 / WLESS_SIGNAL_MAX;
    prog = precent * MON_MAIN_PROG_MAX / 100;

    if(prog >= MON_MAIN_PROG_MAX)
        return MON_MAIN_PROG_MAX;

    if(prog <= 0)
        return 0;    
}

/**************************************************************************************************
 * @brief  : detect_wless生命周期 - 运行
 * @param  : None
 * @return : None
**************************************************************************************************/
static void screen_life_running(void *arg)
{
    uint16_t value, prog, filter_val;
    scr_detect_wless_cxt_t *cxt = arg;

    /* 获取IR值 */
    value = get_analog_device_value(ANA_DEV_RECTIFIER);

    /* 滤波 */
    filter_val = slider_filter_process(&cxt->filter, value);

    /* 获取prog */
    prog = wless_val_to_prog(filter_val);

    LOG_I("raw %d, fileter %d, prog %d", value, filter_val, prog);
    // nrf_delay_ms(500);

    /* 更新进度条 */
    mon_widget_main_attr_set(MON_MAIN_ATTR_PROG, (void *)prog);
}


static screen_life_ops_t detect_wless_sl_ops = {
    .post = screen_life_post,
    .destory = screen_life_destory,
    .running = screen_life_running,
};

/**************************************************************************************************
 * @brief  : detect_wless生命周期 - 销毁
 * @param  : None
 * @return : None
**************************************************************************************************/
static void key_up_event(void *arg)
{
    scr_detect_wless_cxt_t *cxt = arg;
    cxt->prog_slave++;
    mon_widget_sub_attr_set(MON_SUB_ATTR_PROG, (void *)cxt->prog_slave);
}

/**************************************************************************************************
 * @brief  : detect_wless生命周期 - 销毁
 * @param  : None
 * @return : None
**************************************************************************************************/
static void key_down_event(void *arg)
{
    scr_detect_wless_cxt_t *cxt = arg;
    cxt->prog_slave--;
    mon_widget_sub_attr_set(MON_SUB_ATTR_PROG, (void *)cxt->prog_slave);
}

/**************************************************************************************************
 * @brief  : detect_wless按键 - 控制
 * @param  : None
 * @return : None
**************************************************************************************************/
static void key_ctrl_event(void *arg)
{
    toggle_screen("main");
}


static screen_key_event_t detect_wless_sk_ev = {
    .up   = key_up_event,
    .down = key_down_event,
    .ctrl = key_ctrl_event,
};

gui_screen_t screen_detect_wless = {
    .name = "detect_wless",
    .sl_ops = &detect_wless_sl_ops,
    .sk_ev  = &detect_wless_sk_ev,
};



