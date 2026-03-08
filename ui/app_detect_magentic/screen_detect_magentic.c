#include "common.h"
#include "lvgl.h"
#include "screen_type.h"
#include "pic.h"
#include "screen_core.h"
#include "mon_widget.h"
#include "screen_utils.h"

typedef struct _scr_detect_magentic_cxt {
    uint8_t prog_slave;
    uint16_t time_read_sensor;                                                  /**@ 读取磁力计周期 */
    sliding_filter_t filter;                                                    /**@ 滤波器 */
}scr_detect_magentic_cxt_t;

/**************************************************************************************************
 * @brief  : detect_magentic生命周期 - 进入
 * @param  : None
 * @return : None
**************************************************************************************************/
static void screen_life_post(void *arg)
{
    unused(arg);
    scr_detect_magentic_cxt_t *cxt = calloc(sizeof(*cxt), 1);
	
    /* 设置mon参数 */
    mon_widget_set_mode(MON_MODE_PROG);                         // 设置 进度 模式
    mon_widget_enable();                                        // 使能widget
    mon_widget_sub_attr_set(MON_SUB_ATTR_ENABLE, (void *)1);    // 使能子进度

    /* 设置默认进度 */
    cxt->prog_slave = 0;

    /* 保存上下文 */
    save_screen_cxt(cxt);
}

/**************************************************************************************************
 * @brief  : detect_magentic生命周期 - 销毁
 * @param  : None
 * @return : None
**************************************************************************************************/
static void screen_life_destory(void *arg)
{
    scr_detect_magentic_cxt_t *cxt = arg;

    mon_widget_disable();                               // 隐藏mon
    free(cxt);                                          // 释放cxt
}

/**************************************************************************************************
 * @brief  : detect_magentic生命周期 - 运行
 * @param  : None
 * @return : None
**************************************************************************************************/
static void screen_life_running(void *arg)
{
    unused(arg);
    mon_widget_main_attr_set(MON_MAIN_ATTR_PROG, (void *)1);
}


static screen_life_ops_t detect_magentic_sl_ops = {
    .post = screen_life_post,
    .destory = screen_life_destory,
    .running = screen_life_running,
};

/**************************************************************************************************
 * @brief  : detect_magentic生命周期 - 销毁
 * @param  : None
 * @return : None
**************************************************************************************************/
static void key_up_event(void *arg)
{
    scr_detect_magentic_cxt_t *cxt = arg;
    cxt->prog_slave++;
    mon_widget_sub_attr_set(MON_SUB_ATTR_PROG, (void *)cxt->prog_slave);
}

/**************************************************************************************************
 * @brief  : detect_magentic生命周期 - 销毁
 * @param  : None
 * @return : None
**************************************************************************************************/
static void key_down_event(void *arg)
{
    scr_detect_magentic_cxt_t *cxt = arg;
    cxt->prog_slave--;
    mon_widget_sub_attr_set(MON_SUB_ATTR_PROG, (void *)cxt->prog_slave);
}

/**************************************************************************************************
 * @brief  : detect_magentic按键 - 控制
 * @param  : None
 * @return : None
**************************************************************************************************/
static void key_ctrl_event(void *arg)
{
    unused(arg);
    LOG_I("%s", __func__);
    toggle_screen("main");
}


static screen_key_event_t detect_magentic_sk_ev = {
    .up   = key_up_event,
    .down = key_down_event,
    .ctrl = key_ctrl_event,
};

gui_screen_t screen_detect_magentic = {
    .name = "detect_magentic",
    .sl_ops = &detect_magentic_sl_ops,
    .sk_ev  = &detect_magentic_sk_ev,
};

/**************************************************************************************************
 * @brief  : 注册变量
 * @param  : None
 * @return : None
**************************************************************************************************/
void lsh_dbg_mon(int argc, char const *argv[])
{
    unused(argc);
    zw_test = atoi(argv[1]);
}

