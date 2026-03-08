#include "common.h"
#include "lvgl.h"
#include "screen_type.h"
#include "pic.h"
#include "screen_core.h"
#include "system.h"
#include "beep_key_manager.h"
#include "motor_manager.h"
#include "alarm_manager.h"

#define KEY_CNT       6                                                         /**@ 复选框数量 */
#define KEY_POS_X     141                                                       /**@ 复选按钮水平位置固定 */

typedef enum _setting_index {
    ID_KEY_NOTICE,
    ID_ALARM,
    ID_ALARM_HIGH,
    ID_ALARM_LOW,
    ID_SHAKE,
    ID_BACK_HOME,
}setting_index_t;

typedef struct _scr_setting_cxt {
    lv_obj_t *bg;                                                               /**@ 基础背景 */
    lv_obj_t *border;                                                           /**@ 边框 */
    lv_obj_t *key[KEY_CNT];                                                     /**@ 按钮 */
    uint8_t index;                                                              /**@ 当前位置 */
    system_setting_info_t *setting;                                             /**@ 设置 */
}scr_setting_cxt_t;

typedef struct _item_config {
    setting_index_t index;
    int pos_border;
    int pos_key;
    void (*func_oke)(scr_setting_cxt_t *cxt, system_setting_info_t *setting);   /**@ 按键事件 */
}item_config_t;

/**************************************************************************************************
 * @brief  : 切换报警音量类型
 * @param  : None
 * @return : None
**************************************************************************************************/
static void toggle_alarm_volume_type(scr_setting_cxt_t *cxt, system_setting_info_t *setting)
{
    lv_obj_t *uikey_alarm_low = cxt->key[ID_ALARM_LOW];
    lv_obj_t *uikey_alarm_high = cxt->key[ID_ALARM_HIGH];

    /* 如果没开启报警音量, 直接返回 */
    if(!setting->flag_volume_alarm)
        return;

    setting->flag_volume_alarm_type = !setting->flag_volume_alarm_type;

    /* 设置音量类型 */
    if(setting->flag_volume_alarm_type)  {
        lv_obj_set_visual(uikey_alarm_low,  0);
        lv_obj_set_visual(uikey_alarm_high, 1);
        alarm_manager()->set(ALARM_VOLUME_HIGH);
    } else {
        lv_obj_set_visual(uikey_alarm_low,  1);
        lv_obj_set_visual(uikey_alarm_high, 0);
        alarm_manager()->set(ALARM_VOLUME_LOW);
    } 
}

/**************************************************************************************************
 * @brief  : 按键提示音 oke - opttion key event
 * @param  : None
 * @return : None
**************************************************************************************************/
static void toggle_key_notice(scr_setting_cxt_t *cxt, system_setting_info_t *setting)
{
    lv_obj_t *uikey = cxt->key[ID_KEY_NOTICE];

    /* toggle UI */
    setting->flag_volume_key = !setting->flag_volume_key;

    /* 设置功能, 并设置隐藏属性 */
    beep_key_manager()->enable(setting->flag_volume_key);
    lv_obj_set_visual(uikey, setting->flag_volume_key);
}

/**************************************************************************************************
 * @brief  : 报警提示音
 * @param  : None
 * @return : None
**************************************************************************************************/
static void toggle_alarm_notice(scr_setting_cxt_t *cxt, system_setting_info_t *setting)
{
    lv_obj_t *uikey_alarm = cxt->key[ID_ALARM];
    lv_obj_t *uikey_alarm_low = cxt->key[ID_ALARM_LOW];
    lv_obj_t *uikey_alarm_high = cxt->key[ID_ALARM_HIGH];

    /* 更新UI */
    setting->flag_volume_alarm = !setting->flag_volume_alarm;
    lv_obj_set_visual(uikey_alarm, setting->flag_volume_alarm);

    /* 如果报警状态为0, 关闭状态 */
    if(setting->flag_volume_alarm == ALARM_VOLUME_LOW) {
        lv_obj_set_visual(uikey_alarm_low, 0);
        lv_obj_set_visual(uikey_alarm_high, 0);
        alarm_manager()->enable(ALARM_VOLUME_LOW);
        return;
    }

    /* 设置音量类型 */
    if(setting->flag_volume_alarm_type)  {
        lv_obj_set_visual(uikey_alarm_low,  0);
        lv_obj_set_visual(uikey_alarm_high, 1);
        alarm_manager()->set(ALARM_VOLUME_HIGH);
    } else {
        lv_obj_set_visual(uikey_alarm_low,  1);
        lv_obj_set_visual(uikey_alarm_high, 0);
        alarm_manager()->set(ALARM_VOLUME_LOW);
    }

}

/**************************************************************************************************
 * @brief  : 报警提示音 - 高
 * @param  : None
 * @return : None
**************************************************************************************************/
static void toggle_alarm_volume_high(scr_setting_cxt_t *cxt, system_setting_info_t *setting)
{
    toggle_alarm_volume_type(cxt, setting);
}

/**************************************************************************************************
 * @brief  : 报警提示音 - 低
 * @param  : None
 * @return : None
**************************************************************************************************/
static void toggle_alarm_volume_low(scr_setting_cxt_t *cxt, system_setting_info_t *setting)
{
    toggle_alarm_volume_type(cxt, setting);
}

/**************************************************************************************************
 * @brief  : 震动提示
 * @param  : None
 * @return : None
**************************************************************************************************/
static void toggle_shake_notice(scr_setting_cxt_t *cxt, system_setting_info_t *setting)
{
    lv_obj_t *uikey = cxt->key[ID_SHAKE];

    /* toggle震动 */
    setting->flag_shake = !setting->flag_shake;

    /* 设置震动 */
    motor_manager()->enable(setting->flag_shake);
    lv_obj_set_visual(uikey, setting->flag_shake);
}

/**************************************************************************************************
 * @brief  : 返回上层
 * @param  : None
 * @return : None
**************************************************************************************************/
static void back_home_event(scr_setting_cxt_t *cxt, system_setting_info_t *setting)
{
    save_system_setting_info();
    toggle_screen("main");
}

static item_config_t item_config_table[] = {
    {ID_KEY_NOTICE, 58,  70,   toggle_key_notice},
    {ID_ALARM,      98,  107,  toggle_alarm_notice},
    {ID_ALARM_HIGH, 134, 145,  toggle_alarm_volume_high},
    {ID_ALARM_LOW,  170, 181,  toggle_alarm_volume_low},
    {ID_SHAKE,      209, 218,  toggle_shake_notice},
    {ID_BACK_HOME,  244, -1,   back_home_event},
};

/**************************************************************************************************
 * @brief  : 设置key初始化
 * @param  : None
 * @return : None
**************************************************************************************************/
static void inline screen_setting_key_init(scr_setting_cxt_t *cxt, system_setting_info_t *setting)
{
    lv_obj_t *uikey = cxt->key[ID_KEY_NOTICE];
    lv_obj_set_visual(uikey, setting->flag_volume_key);
}

/**************************************************************************************************
 * @brief  : 设置key初始化
 * @param  : None
 * @return : None
**************************************************************************************************/
static void inline screen_setting_alarm_init(scr_setting_cxt_t *cxt, system_setting_info_t *setting)
{
    lv_obj_t *uikey_alarm = cxt->key[ID_ALARM];
    lv_obj_t *uikey_alarm_low = cxt->key[ID_ALARM_LOW];
    lv_obj_t *uikey_alarm_high = cxt->key[ID_ALARM_HIGH];

    /* 设置UI */
    lv_obj_set_visual(uikey_alarm, setting->flag_volume_alarm);

    /* 如果没开启, 直接返回 */
    if(!setting->flag_volume_alarm)
        return;

    /* 更新状态 */
    if(setting->flag_volume_alarm_type) {
        lv_obj_set_visual(uikey_alarm_low,  0);
        lv_obj_set_visual(uikey_alarm_high, 1);
    } else {
        lv_obj_set_visual(uikey_alarm_low,  1);
        lv_obj_set_visual(uikey_alarm_high, 0);
    }
}

/**************************************************************************************************
 * @brief  : 设置key初始化
 * @param  : None
 * @return : None
**************************************************************************************************/
static void inline screen_setting_shake_init(scr_setting_cxt_t *cxt, system_setting_info_t *setting)
{
    lv_obj_t *uikey = cxt->key[ID_SHAKE];
    lv_obj_set_visual(uikey, setting->flag_shake);
}

/**************************************************************************************************
 * @brief  : 设置页面初始化
 * @param  : None
 * @return : None
**************************************************************************************************/
static void screen_setting_init(scr_setting_cxt_t *cxt, system_setting_info_t *setting)
{
    screen_setting_key_init(cxt, setting);
    screen_setting_alarm_init(cxt, setting);
    screen_setting_shake_init(cxt, setting);
}

/**************************************************************************************************
 * @brief  : 创建进度条背景
 * @param  : None
 * @return : None
**************************************************************************************************/
static void create_setting_bg(scr_setting_cxt_t *cxt)
{
    /* 背景 */
    cxt->bg = lv_img_create(lv_scr_act());
    lv_img_set_src(cxt->bg, &pic_setting);

    /* 创建边框 */
    cxt->border = lv_img_create(cxt->bg);
    lv_img_set_src(cxt->border, &pic_setting_border);
    lv_obj_align(cxt->border, LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_set_y(cxt->border, item_config_table[0].pos_border);

    /* 创建按钮 */
    for(int i = 0 ; i < KEY_CNT ; i++)
    {
        cxt->key[i] = lv_img_create(cxt->bg);
        lv_img_set_src(cxt->key[i], &pic_setting_key);
        lv_obj_set_pos(cxt->key[i], KEY_POS_X, item_config_table[i].pos_key);
        lv_obj_add_flag(cxt->key[i], LV_OBJ_FLAG_HIDDEN);
    }
}

/**************************************************************************************************
 * @brief  : setting生命周期 - 进入
 * @param  : None
 * @return : None
**************************************************************************************************/
static void screen_life_post(void *arg)
{
    scr_setting_cxt_t *cxt = calloc(sizeof(*cxt), 1);
    cxt->index = 0;

    /* 获取设置信息 */
    cxt->setting = get_system_setting_info();
	
    /* 设置背景 */
    create_setting_bg(cxt);

    /* 设置初始化 */
    screen_setting_init(cxt, cxt->setting);

    /* 保存上下文 */
    save_screen_cxt(cxt);
}

/**************************************************************************************************
 * @brief  : setting生命周期 - 销毁
 * @param  : None
 * @return : None
**************************************************************************************************/
static void screen_life_destory(void *arg)
{
    scr_setting_cxt_t *cxt = arg;

    lv_obj_del(cxt->bg);
    free(cxt);
}

/**************************************************************************************************
 * @brief  : setting生命周期 - 运行
 * @param  : None
 * @return : None
**************************************************************************************************/
static void screen_life_running(void *arg)
{
    LOG_I("%s", __func__);
}


static screen_life_ops_t setting_sl_ops = {
    .post = screen_life_post,
    .destory = screen_life_destory,
    .running = screen_life_running,
};

/**************************************************************************************************
 * @brief  : setting生命周期 - 销毁
 * @param  : None
 * @return : None
**************************************************************************************************/
static void key_up_event(void *arg)
{
    scr_setting_cxt_t *cxt = arg;
    item_config_t *cfg;

    /* 更新index */
    if(cxt->index-- == 0)
        cxt->index = ARRAY_SIZE(item_config_table) - 1;

    cfg = &item_config_table[cxt->index];
    lv_obj_set_y(cxt->border, cfg->pos_border);
}

/**************************************************************************************************
 * @brief  : setting生命周期 - 销毁
 * @param  : None
 * @return : None
**************************************************************************************************/
static void key_down_event(void *arg)
{
    scr_setting_cxt_t *cxt = arg;
    item_config_t *cfg;

    /* 更新index */
    if(cxt->index++ >= ARRAY_SIZE(item_config_table) - 1)
        cxt->index = 0;

    cfg = &item_config_table[cxt->index];
    lv_obj_set_y(cxt->border, cfg->pos_border);
}

/**************************************************************************************************
 * @brief  : setting按键 - 控制
 * @param  : None
 * @return : None
**************************************************************************************************/
static void key_ctrl_event(void *arg)
{
    scr_setting_cxt_t *cxt = arg;
    item_config_t *cfg = &item_config_table[cxt->index];

    /* 处理事件 */
    cfg->func_oke(cxt, cxt->setting);
}


static screen_key_event_t setting_sk_ev = {
    .up   = key_up_event,
    .down = key_down_event,
    .ctrl = key_ctrl_event,
};

gui_screen_t screen_setting = {
    .name = "setting",
    .sl_ops = &setting_sl_ops,
    .sk_ev  = &setting_sk_ev,
};


