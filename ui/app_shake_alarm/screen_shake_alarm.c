#include "common.h"
#include "lvgl.h"
#include "screen_type.h"
#include "pic.h"
#include "screen_core.h"
#include "accel_manager.h"
#include "drv_misc.h"
#include "screen_utils.h"
#include "msgbus.h"
#include "alarm_manager.h"
#include "motor_manager.h"
#include "drv_st7789.h"

#define TIMEOUT_ALARM           50                                              /**@ 报警周期 500ms */
#define TIMEOUT_RECOVERY        (30 * 200)                                      /**@ 30s后恢复 */
#define TIMEOUT_COUNT_DOWN      (10 * 1000)                                     /**@ 10s倒计时 */

typedef enum _sha_stat_type {
    SHA_STAT_IDLE,                                                              /**@ 空闲态 */
    SHA_STAT_CNT_DOWN,                                                          /**@ 倒计时状态 */
    SHA_STAT_READY,                                                             /**@ 就绪态 */
    SHA_STAT_ALARM,                                                             /**@ 报警态 */
}sha_stat_type_t;

typedef struct _scr_shake_alarm_cxt {
    lv_obj_t *scr;                                                              /**@ 屏幕 */
    lv_obj_t *label;                                                            /**@ 标签 */
    sha_stat_type_t stat;                                                       /**@ 状态 */
    uint32_t time_alarm;                                                        /**@ 报警时间累计. 单位ms */
    uint32_t time_recovery;                                                     /**@ 报警恢复时间累计. 单位TIMEOUT_ALARM, 当前50ms*/
    uint32_t time_cnt_down;                                                     /**@ 倒计时累计. 单位ms */
    msgbus_client_t *mb_client;                                                 /**@ mb客户端 */
}scr_shake_alarm_cxt_t;

extern const lv_font_t chinese;

/**************************************************************************************************
 * @brief  : 创建屏幕, 黑色背景、无边框
 * @param  : None
 * @return : None
**************************************************************************************************/
static void create_shake_alarm_screen(scr_shake_alarm_cxt_t *cxt)
{
    cxt->scr = lv_obj_create(lv_scr_act());
    lv_obj_set_style_bg_color(cxt->scr, lv_color_black(), 0);
    lv_obj_set_size(cxt->scr, lv_disp_get_hor_res(NULL), lv_disp_get_ver_res(NULL));
    lv_obj_set_style_border_width(cxt->scr, 0, 0);
}

/**************************************************************************************************
 * @brief  : 创建提示标签
 * @param  : None
 * @return : None
**************************************************************************************************/
static void create_shake_alarm_label(scr_shake_alarm_cxt_t *cxt)
{
    /* 创建标签  */
    cxt->label = lv_label_create(cxt->scr);

    /* 设置字体和颜色 */
    lv_obj_set_style_text_color(cxt->label, lv_color_hex(0x70DBFF), 0);
    lv_obj_set_style_text_font(cxt->label, &chinese, 0);

    /* 设置文字内容 */
    lv_label_set_text(cxt->label, "10S后开始侦测 ");
    lv_obj_align(cxt->label, LV_ALIGN_TOP_MID, 0, 100);
}

/**************************************************************************************************
 * @brief  : detect_location生命周期 - 进入
 * @param  : None
 * @return : None
**************************************************************************************************/
static void accel_topic_event(void *data, uint16_t len, void *priv)
{
    scr_shake_alarm_cxt_t *cxt = priv;

    /* 之前在就绪态, 则进入报警态 */
    if(cxt->stat == SHA_STAT_READY)
    {
        cxt->time_recovery = 0;
        cxt->stat = SHA_STAT_ALARM;
        motor_manager()->start();            // 启动震动
        LOG_I("alarm screen : ready to alarm");
        return;
    }
}

/**************************************************************************************************
 * @brief  : shake_alarm生命周期 - 进入
 * @param  : None
 * @return : None
**************************************************************************************************/
static void screen_life_post(void *arg)
{
    scr_shake_alarm_cxt_t *cxt = calloc(sizeof(*cxt), 1);
	
    /* 创建屏幕 + 创建标签 */
    create_shake_alarm_screen(cxt);
    create_shake_alarm_label(cxt);

    /* 初始化基础参数 */
    cxt->time_alarm = 0;                        // 每次到达周期后, 触发红外翻转、蜂鸣器报警
    cxt->time_recovery = 0;                     // 累计n个周期后, 无震荡事件, 恢复ready状态
    cxt->time_cnt_down = 0;                     // 累计到倒计时, 倒计时结束进入就ready状态
    cxt->stat = SHA_STAT_CNT_DOWN;              // 设置倒计时模式

    /* 保存上下文 */
    save_screen_cxt(cxt);

    /* 设置accel */
    cxt->mb_client = msgbus()->subscribe_topic("accel", accel_topic_event, cxt);
    msgbus()->set_client_attr(cxt->mb_client, MBC_ATTR_NAME, "screen_shake_alarm");

    /* 初始化alarm manager */
    alarm_manager()->init();
}

/**************************************************************************************************
 * @brief  : shake_alarm生命周期 - 销毁
 * @param  : None
 * @return : None
**************************************************************************************************/
static void screen_life_destory(void *arg)
{
    scr_shake_alarm_cxt_t *cxt = arg;

    /* 关闭相关外设 */
    msgbus()->unsubscribe_topic(cxt->mb_client);
    ir_set(0);
    motor_manager()->stop();
    alarm_manager()->exit();

    /* 删除相关UI控件 */
    lv_obj_del(cxt->label);
    lv_obj_del(cxt->scr);
    free(cxt);

    /* 刷新屏幕 */
    lv_timer_handler();
    lv_refr_now(NULL);

    /* 打开屏幕 */
    set_lcd_power_state(1);
}

/**************************************************************************************************
 * @brief  : 倒计时业务
 * @param  : None
 * @return : None
**************************************************************************************************/
static void do_count_down_work(scr_shake_alarm_cxt_t *cxt)
{
    if(!check_cycle(&cxt->time_cnt_down, TIMEOUT_COUNT_DOWN))
        return; 

    /* 报警 */
    alarm_manager()->single_play(500);
    alarm_manager()->single_play(500);

    /* 倒计时结束, 熄屏, 进入就绪态 */
    set_lcd_power_state(0);
    cxt->stat = SHA_STAT_READY;
    cxt->time_recovery = 0;
    LOG_I("screen_shake_alarm : count down -> ready");
}

/**************************************************************************************************
 * @brief  : 处理报警业务
 * @param  : None
 * @return : None
**************************************************************************************************/
static void do_alarm_work(scr_shake_alarm_cxt_t *cxt)
{
    if(!check_cycle(&cxt->time_alarm, TIMEOUT_ALARM))
        return;

    /* 报警 */
    ir_troggle();
    alarm_manager()->toggle();

#if 0
    LOG_I("time recovery = %d, %d", cxt->time_recovery, TIMEOUT_RECOVERY / get_ui_refresh_time());
#endif

    /* 是否到达恢复时间 */
    if(!check_cycle(&cxt->time_recovery, TIMEOUT_RECOVERY))
        return;

    /* 关闭相关外设 */
    ir_set(0);
    alarm_manager()->stop();
    motor_manager()->stop();

    /* 重回就绪态 */
    cxt->time_alarm = 0;
    cxt->time_recovery = 0;
    
    /* 更新状态 */
    nrf_delay_ms(500);
    cxt->stat = SHA_STAT_READY;
    LOG_I("screen_shake_alarm : alarm -> ready");
}

/**************************************************************************************************
 * @brief  : shake_alarm生命周期 - 运行
 * @param  : None
 * @return : None
**************************************************************************************************/
static void screen_life_running(void *arg)
{
    scr_shake_alarm_cxt_t *cxt = arg;

    switch(cxt->stat)
    {
    case SHA_STAT_CNT_DOWN:
        do_count_down_work(cxt);
        break;

    case SHA_STAT_READY:
        break;

    case SHA_STAT_ALARM:
        do_alarm_work(cxt);
        break;

    default:
        break;
    }
}


static screen_life_ops_t shake_alarm_sl_ops = {
    .post = screen_life_post,
    .destory = screen_life_destory,
    .running = screen_life_running,
};

/**************************************************************************************************
 * @brief  : shake_alarm生命周期 - 销毁
 * @param  : None
 * @return : None
**************************************************************************************************/
static void up_key_event(void *arg)
{
    toggle_screen("main");
}

/**************************************************************************************************
 * @brief  : shake_alarm生命周期 - 销毁
 * @param  : None
 * @return : None
**************************************************************************************************/
static void down_key_event(void *arg)
{
    toggle_screen("main");
}

/**************************************************************************************************
 * @brief  : shake_alarm按键 - 控制
 * @param  : None
 * @return : None
**************************************************************************************************/
static void ctrl_key_event(void *arg)
{
    toggle_screen("main");
}


static screen_key_event_t shake_alarm_sk_ev = {
    .up   = up_key_event,
    .down = down_key_event,
    .ctrl = ctrl_key_event,
};

gui_screen_t screen_shake_alarm = {
    .name = "shake_alarm",
    .sl_ops = &shake_alarm_sl_ops,
    .sk_ev  = &shake_alarm_sk_ev,
};



