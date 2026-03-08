#include "common.h"
#include "lvgl.h"
#include "ble_common.h"
#include "screen_type.h"
#include "pic.h"
#include "screen_core.h"
#include "msgbus.h"
#include "mon_widget.h"
#include "screen_info.h"
#include "topic.h"
#include "screen_utils.h"


#define FIRST_SCAN_ANIM_TIME  1000                                              /**< 初始扫描动画展示事件, 单位ms */
#define RSSI_UPDATE_CYCLE     100                                               /**< 50ms更新一次 */     


#define RSSI_MIN            (-100)                                              /**< RSSI最小值 */
#define RSSI_MAX            (-30)                                               /**< RSSI最大值 */

typedef enum _sdl_state {
    SDL_STAT_FIRST_SCAN,                                                        /**< 初次扫描动画 */
    SDL_STAT_PRE_SCAN,                                                          /**< 预扫描 - 中间态 */
    SDL_STAT_SCAN,                                                              /**< 扫描模式 */
    SDL_STAT_PRE_WORK,                                                          /**< 预处理业务 - 中间态 */
    SDL_STAT_WORK,                                                              /**< 业务模式 */
}sdl_state_t;

typedef struct _scr_detect_location_cxt {
    msgbus_client_t *mb_client;                                                 /**@ mb客户端 */
    sdl_state_t stat;                                                           /**@ 屏幕业务状态 */
    uint8_t level;                                                              /**@ 进度等级 */
    int rssi;                                                                   /**@ 当前rssi */
    int rssi_max;                                                               /**@ rssi最大值 */
    uint16_t cycle;                                                             /**@ 扫描周期, 50ms统计一次, 记录最大值 */
    uint16_t fist_scan_timeount;                                                /**@ 初次扫描超时时间 */
    uint8_t flag_tag_work : 1;                                                  /**@ tag业务 */
    sliding_filter_t filter;                                                    /**@ 平滑滤波 */
}scr_detect_location_cxt_t;

/**************************************************************************************************
 * @brief  : detect_location生命周期 - 进入
 * @param  : None
 * @return : None
**************************************************************************************************/
static void ble_topic_event(void *data, uint16_t len, void *priv)
{
    unused(len);
    scr_detect_location_cxt_t *cxt = priv;
    cxt->rssi = (*((int *)data));

    if(cxt->flag_tag_work == 0) {
        cxt->stat = SDL_STAT_PRE_WORK;
        cxt->flag_tag_work = 1;
    }
}

/**************************************************************************************************
 * @brief  : detect_location生命周期 - 进入
 * @param  : None
 * @return : None
**************************************************************************************************/
static void screen_life_post(void *arg)
{
    unused(arg);
    scr_detect_location_cxt_t *cxt = calloc(sizeof(*cxt), 1);

    /* 创建 mon 并设置模式为扫描模式 */
    mon_widget_enable();
    mon_widget_set_mode(MON_MODE_SCAN);
    mon_widget_sub_attr_set(MON_SUB_ATTR_ENABLE, (void *)0);

    /* 注册订阅 */
	cxt->mb_client = msgbus()->subscribe_topic("ble", ble_topic_event, cxt);
    msgbus()->set_client_attr(cxt->mb_client, MBC_ATTR_NAME, "screen_detect_location");

    /* 设置屏幕模式为扫描模式 */
    cxt->stat = SDL_STAT_FIRST_SCAN;                                        // 更新为预扫描态
    cxt->flag_tag_work = 0;                                                 // tag业务默认关闭

    /* 保存上下文 */
    save_screen_cxt(cxt);
}

/**************************************************************************************************
 * @brief  : 处理初次扫描业务
 * @param  : None
 * @return : None
**************************************************************************************************/
static void do_fist_scan_work(scr_detect_location_cxt_t *cxt)
{
    uint16_t total_time;

    /* 执行扫描动画 */
    mon_widget_main_scan();

    /* 更新首次扫描动画时间 */
    cxt->fist_scan_timeount++;

    /* 计算总体运行时间 */
    total_time = cxt->fist_scan_timeount * get_ui_refresh_time();

    /* 进入scan模式 */
    if(total_time > FIRST_SCAN_ANIM_TIME)
    {
        cxt->fist_scan_timeount = 0;
        cxt->stat = SDL_STAT_SCAN;      // 设置扫描模式
        ble_scan_start();               // 启动扫描
    }
}





/**************************************************************************************************
 * @brief  : 更新进度条
 * @param  : None
 * @return : None
**************************************************************************************************/
static void update_mon_main_progress(scr_detect_location_cxt_t *cxt, int rssi)
{
    unused(cxt);

    int precent, val, range;
    int prog;


    /* 当前值 */
    val = rssi - RSSI_MIN;
    range = RSSI_MAX - RSSI_MIN;

    /* 计算比分比 */
    precent = val * 100 / range;
    prog = precent * 16 / 100;

    /* 更新进度条 */
    mon_widget_main_attr_set(MON_MAIN_ATTR_PROG, (void *)prog);
}

/**************************************************************************************************
 * @brief  : 处理跟踪tag业务
 * @param  : None
 * @return : None
**************************************************************************************************/
static int rssi_to_prog(int rssi)
{
    int precent, val, range, prog;

    val = rssi - RSSI_MIN;
    range = RSSI_MAX - RSSI_MIN;

    precent = val * 100 / range;
    prog = precent * MON_MAIN_PROG_MAX / 100;

    if(prog >= MON_MAIN_PROG_MAX)
        return MON_MAIN_PROG_MAX;

    if(prog <= 0)
        return 0;

    return prog;
}

/**************************************************************************************************
 * @brief  : 处理跟踪tag业务
 * @param  : None
 * @return : None
**************************************************************************************************/
static void do_trace_tag_work(scr_detect_location_cxt_t *cxt)
{
    int rssi, prog;

    /* 滤波 */
    rssi = slider_filter_process(&cxt->filter, cxt->rssi);

    /* rssi转prog */
    prog = rssi_to_prog(rssi);

    /* 更新进度条 */
    mon_widget_main_attr_set(MON_MAIN_ATTR_PROG, (void *)prog);
}

/**************************************************************************************************
 * @brief  : detect_location生命周期 - 运行态
 * @param  : None
 * @return : None
**************************************************************************************************/
static void screen_life_running(void *arg)
{
    scr_detect_location_cxt_t *cxt = arg;

    switch(cxt->stat)
    {
    case SDL_STAT_FIRST_SCAN:
        do_fist_scan_work(cxt);
        break;

    case SDL_STAT_SCAN:
        mon_widget_main_scan();
        break;

    case SDL_STAT_PRE_WORK:
        mon_widget_set_mode(MON_MODE_PROG);                     // 设置为进度模式
        cxt->stat = SDL_STAT_WORK;                              // 更新为进度状态
        break;

    case SDL_STAT_WORK:
        do_trace_tag_work(cxt);
        break;

    default:
        break;

    }
}

/**************************************************************************************************
 * @brief  : detect_location生命周期 - 销毁
 * @param  : None
 * @return : None
**************************************************************************************************/
static void screen_life_destory(void *arg)
{
    scr_detect_location_cxt_t *cxt = arg;

    /* 注销订阅 */
    msgbus()->unsubscribe_topic(cxt->mb_client);

    /* 隐藏控件 */
    mon_widget_disable();

    /* 删除背景 */
    free(cxt);

    /* 关闭蓝牙扫描 */
    nrf_ble_scan_stop();
}

static screen_life_ops_t detect_location_sl_ops = {
    .post = screen_life_post,
    .destory = screen_life_destory,
    .running = screen_life_running,
};

/**************************************************************************************************
 * @brief  : detect_location生命周期 - 销毁
 * @param  : None
 * @return : None
**************************************************************************************************/
static void up_key_event(void *arg)
{
    unused(arg);
    LOG_I("%s", __func__);
}

/**************************************************************************************************
 * @brief  : detect_location生命周期 - 销毁
 * @param  : None
 * @return : None
**************************************************************************************************/
static void down_key_event(void *arg)
{
    unused(arg);
    LOG_I("%s", __func__);
}

/**************************************************************************************************
 * @brief  : detect_location按键 - 控制
 * @param  : None
 * @return : None
**************************************************************************************************/
static void ctrl_key_event(void *arg)
{
    unused(arg);
    toggle_screen("main");
}


static screen_key_event_t detect_location_sk_ev = {
    .up   = up_key_event,
    .down = down_key_event,
    .ctrl = ctrl_key_event,
};

gui_screen_t screen_detect_location = {
    .name = "detect_location",
    .sl_ops = &detect_location_sl_ops,
    .sk_ev  = &detect_location_sk_ev,
};

