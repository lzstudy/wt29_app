#include "common.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "screen_core.h"
#include "screen_type.h"
#include "screen_config.h"
#include "screen_info.h"
#include "drv_key.h"
#include "drv_beep.h"
#include "beep_manager.h"
#include "beep_key_manager.h"

/* 遍历screen对象 */
#define list_for_each_gui_screen(scr, head)   \
            list_for_each_entry(scr, head, gui_screen_t, list)


typedef struct _gui_key_ev {
    SemaphoreHandle_t sem;                                                      /**@ 信号量 */
    uint8_t ev_up : 1;
    uint8_t ev_down : 1;
    uint8_t ev_ctrl : 1;
}gui_key_ev_t;

typedef enum _screen_state {
    SCREEN_STAT_POST = 1,                                                       /**@ 屏幕显示 */
    SCREEN_STAT_DESTORY = 2,                                                    /**@ 屏幕销毁 */
    SCREEN_STAT_RUNNING = 4,                                                    /**@ 运行态 */
    SCREEN_STAT_KEY_PRESS = 8,                                                  /**@ 按键按下 */
}screen_state_t;

typedef struct _screen_manager {
    gui_screen_t *current;                                                      /**@ 当前屏幕 */
    gui_key_ev_t kev;                                                           /**@ 按键事件 */
    struct list_head head;                                                      /**@ 所有屏幕链表头 */
    void *data;                                                                 /**@ 每个屏幕的私有数据 */
    screen_state_t stat;                                                        /**@ 屏幕状态 */
}screen_manager_t;

static screen_manager_t screen_manager;                                         /**# 页面管理 */ 

/**************************************************************************************************
 * @brief  : 获取ui刷新时间
 * @param  : None
 * @return : None
**************************************************************************************************/
uint8_t get_ui_refresh_time(void)
{
    return UI_REFRESH_CYCLE;
}

/**************************************************************************************************
 * @brief  : 保存上下文
 * @param  : None
 * @return : None
**************************************************************************************************/
void save_screen_cxt(void *cxt)
{
    screen_manager.data = cxt;
}

/**************************************************************************************************
 * @brief  : 注册屏幕
 * @param  : None
 * @return : None
**************************************************************************************************/
static void register_screen(gui_screen_t *screen)
{
    screen_manager_t *mg = &screen_manager;

    mg->current = NULL;
    list_init(&screen->list);
    list_add_before(&mg->head, &screen->list);
}

/**************************************************************************************************
 * @brief  : UI框架初始化
 * @param  : None
 * @return : None
**************************************************************************************************/
void gui_framework_init(void)
{
    screen_manager_t *mg = &screen_manager;
    gui_screen_config_t *cfg;

    list_init(&mg->head);

    /* 注册所有的屏幕 */
    for(int i = 0 ; i < ARRAY_SIZE(gui_screen_config_table) ; i++) {
        cfg = &gui_screen_config_table[i];
        register_screen(cfg->screen);
    }
}

/**************************************************************************************************
 * @brief  : 屏幕展示
 * @param  : None
 * @return : None
**************************************************************************************************/
static void do_screen_post(screen_manager_t *mg)
{
    gui_screen_t *scr = mg->current;

    /* 进入屏幕post事件 */
    scr->sl_ops->post(NULL);
}

/**************************************************************************************************
 * @brief  : 屏幕展示
 * @param  : None
 * @return : None
**************************************************************************************************/
static void do_screen_destory(screen_manager_t *mg)
{
    gui_screen_t *scr = mg->current;
    scr->sl_ops->destory(mg->data);
}

/**************************************************************************************************
 * @brief  : 切换屏幕, 主屏幕不销毁
 * @param  : None
 * @return : None
**************************************************************************************************/
void toggle_screen(const char *name)
{
    gui_screen_t *scr = NULL;
    screen_manager_t *mg = &screen_manager;

    /* 删除当前的屏幕, 主屏幕不删除 */
    if(mg->current)  
    {
        if(strcmp(mg->current->name, screen_main.name) != 0)
        {
            do_screen_destory(mg);
        }
    }

    /* 遍历scr链表, 匹配名字, 找到对应的gui_screen_t */
    list_for_each_gui_screen(scr, &mg->head)
    {
        if(!strncmp(scr->name, name, strlen(scr->name))) {
            mg->current = scr;
            do_screen_post(mg);
            return;
        }
    }

    LOG_I("[%s] : unknown screen name %s", __func__, name);
}


/**************************************************************************************************
 * @brief  : 上键事件
 * @param  : None
 * @return : None
**************************************************************************************************/
static void key_up_event(void *arg)
{
    gui_key_ev_t *kev = &screen_manager.kev;
    kev->ev_up = 1;
    screen_manager.stat = SCREEN_STAT_KEY_PRESS;
}

/**************************************************************************************************
 * @brief  : 上键事件
 * @param  : None
 * @return : None
**************************************************************************************************/
static void key_down_event(void *arg)
{
    gui_key_ev_t *kev = &screen_manager.kev;
    kev->ev_down = 1;
    screen_manager.stat = SCREEN_STAT_KEY_PRESS;
}

/**************************************************************************************************
 * @brief  : 上键事件
 * @param  : None
 * @return : None
**************************************************************************************************/
static void key_ctrl_event(void *arg)
{
    gui_key_ev_t *kev = &screen_manager.kev;
    kev->ev_ctrl = 1;
    screen_manager.stat |= SCREEN_STAT_KEY_PRESS;
}


/**************************************************************************************************
 * @brief  : 按键初始化
 * @param  : None
 * @return : None
**************************************************************************************************/
void gui_key_init(void)
{
    gui_key_ev_t *kev = &screen_manager.kev;

    /* 初始化信号量 */
    kev->sem = xSemaphoreCreateCounting(3, 0);  // xSemaphoreCreateMutex(); //xSemaphoreCreateCounting(3, 0);

    /* 初始化状态 */
    kev->ev_up   = 0;
    kev->ev_ctrl = 0;
    kev->ev_down = 0;

    /* 注册按键事件 */
    key_register(KEY_UP,   key_up_event,   NULL);
    key_register(KEY_DOWN, key_down_event, NULL);
    key_register(KEY_CTRL, key_ctrl_event, NULL);
}

/**************************************************************************************************
 * @brief  : 发布按键事件
 * @param  : None
 * @return : None
**************************************************************************************************/
static void distribute_key_event(screen_manager_t *mg)
{
    gui_key_ev_t *kev = &mg->kev;
    screen_key_event_t *sk_ev = mg->current->sk_ev;

    /* 发布上按键事件 */
    if(kev->ev_up) {
        sk_ev->up(mg->data);
        kev->ev_up = 0;
    }

    /* 发布下按键事件 */
    if(kev->ev_down) {
        sk_ev->down(mg->data);
        kev->ev_down = 0;
    }

    /* 发布控制事件 */
    if(kev->ev_ctrl) {
        sk_ev->ctrl(mg->data);
        kev->ev_ctrl = 0;
    }
}

/**************************************************************************************************
 * @brief  : 处理UI业务
 * @param  : None
 * @return : None
**************************************************************************************************/
void do_gui_work(screen_manager_t *mg)
{
    /* 如果监测到有按键按下，触发按键事件 */
    if(mg->stat & SCREEN_STAT_KEY_PRESS) {
		beep_key_manager()->play();
        distribute_key_event(mg);
        mg->stat &= ~SCREEN_STAT_KEY_PRESS;
    }

    /* 运行 */
    mg->current->sl_ops->running(mg->data);
}


/**************************************************************************************************
 * @brief  : GUI frwk启动
 * @param  : None
 * @return : None
**************************************************************************************************/
void gui_frwk_start(void)
{
    key_init();
    gui_key_init();

    /* frwk初始化 */
    gui_framework_init();

    /* 切换到主屏幕 */
    main_screen_init();
    toggle_screen("main");

    while(1)
    {
        do_gui_work(&screen_manager);
        lv_timer_handler();
        lv_refr_now(NULL);
        nrf_delay_ms(UI_REFRESH_CYCLE);        
    }

}

