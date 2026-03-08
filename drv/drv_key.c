#include "common.h"
#include "nrf.h"
#include "nrf_drv_gpiote.h"
#include "app_error.h"
#include "drv_key.h"

typedef struct _key_cxt {
    key_cb_t cb;
    void *arg;
}key_cxt_t;

typedef struct _key_manager {
    key_cxt_t up;                                                               /**@ 上按键上下文 */
    key_cxt_t down;                                                             /**@ 下按键上下文 */
    key_cxt_t ctrl;                                                             /**@ 控制按键上下文 */
}key_manager_t;

typedef struct _key_cfg {
    key_type_t key;                                                             /**@ 按键类型 */
    uint16_t offs_cxt;                                                          /**@ 上下文偏移 */
}key_cfg_t;

static key_manager_t key_manager;

#define OFFS_KMG(x)             offsetof(key_manager_t, x)
#define offsetto(base, offs)    (((uint8_t *)base) + offs)
#define TO_CXT(base, offs)      ((key_cxt_t *)offsetto(base, offs))

static key_cfg_t key_cfg_tbl[] = {
    {KEY_UP,   OFFS_KMG(up)},
    {KEY_DOWN, OFFS_KMG(down)},
    {KEY_CTRL, OFFS_KMG(ctrl)},
};


/**************************************************************************************************
 * @brief  : 按键事件
 * @param  : None
 * @return : None
**************************************************************************************************/
static void key_event(nrf_drv_gpiote_pin_t key, nrf_gpiote_polarity_t action)
{
    key_cxt_t *cxt;
    key_cfg_t *cfg;
    key_manager_t *mg = &key_manager;

    if(nrf_gpio_pin_read(key) != 0)
        return;

    /* 找到该按键的上下文并返回 */
    for(int i = 0 ; i < ARRAY_SIZE(key_cfg_tbl) ; i++)
    {
        cfg = &key_cfg_tbl[i];
        if(cfg->key == key) {
            cxt = TO_CXT(mg, cfg->offs_cxt);
            cxt->cb(cxt->arg);
            return;
        }
    }
}

/**************************************************************************************************
 * @brief  : 按键驱动初始化
 * @param  : None
 * @return : None
**************************************************************************************************/
static void key_drv_init(key_type_t key)
{
    ret_code_t err_code;
    nrf_drv_gpiote_in_config_t cfg = NRFX_GPIOTE_CONFIG_IN_SENSE_HITOLO(true);

    /* 配置输入引脚 */
    err_code = nrf_drv_gpiote_in_init(key , &cfg, key_event);
    APP_ERROR_CHECK(err_code);

    /* 驱动使能 */
    cfg.pull = NRF_GPIO_PIN_PULLUP;
    nrf_drv_gpiote_in_event_enable(key, true);
}

/**************************************************************************************************
 * @brief  : 按键初始化
 * @param  : None
 * @return : None
**************************************************************************************************/
void key_init(void)
{
    ret_code_t err_code;
    
    /* 注册按键驱动 */
    key_drv_init(KEY_UP);
    key_drv_init(KEY_DOWN);
    key_drv_init(KEY_CTRL);
}

/**************************************************************************************************
 * @brief  : 按键注册
 * @param  : None
 * @return : None
**************************************************************************************************/
int key_register(key_type_t key, key_cb_t cb, void *arg)
{
    key_cxt_t *cxt;
    key_cfg_t *cfg;
    key_manager_t *mg = &key_manager;

    /* 找到该按键的上下文并返回 */
    for(int i = 0 ; i < ARRAY_SIZE(key_cfg_tbl) ; i++)
    {
        cfg = &key_cfg_tbl[i];
        if(cfg->key == key) {
            cxt = TO_CXT(mg, cfg->offs_cxt);
            cxt->cb  = cb;
            cxt->arg = arg;
            return 0;
        }
    }

    /* 不支持的按键类型 */
    LOG_I("[%s]: unknown key %d", __func__, key);
    return -1;
}


/*=================================== key ========================================== */
#define KEY_ACTIVE_LEVEL 0       // 0:低电平有效, 1:高电平有效
#define KEY_SHORT_MS     50       // 短按最小时间(消抖+确认)
#define KEY_LONG_MS      1000     // 长按时间
#define KEY_SCAN_MS      10       // 扫描间隔

static uint32_t key_press_time = 0;
static uint8_t key_last_state = 0;
static uint8_t key_handled = 0;  // 防止重复处理

static uint8_t key_read(void)
{
    uint8_t pin_state = nrf_gpio_pin_read(KEY_UP);
    return (KEY_ACTIVE_LEVEL ? (pin_state == 1) : (pin_state == 0));
}

// 短按回调函数（需实现）
void on_key_short(void)
{
    // 在这里写短按处理逻辑
    // 例如：切换LED
    LOG_I("short");
}

// 长按回调函数（需实现）
void on_key_long(void)
{
    // 在这里写长按处理逻辑
    // 例如：进入配置模式
    LOG_I("long");
}

static void key_scan(void)
{
    uint8_t current = key_read();
    
    if (current && !key_last_state)  // 按下瞬间
    {
        key_press_time = 0;
        key_handled = 0;
    }
    else if (current && key_last_state)  // 持续按下
    {
        key_press_time += KEY_SCAN_MS;
        
        // 长按检测（按住1秒触发一次）
        if (!key_handled && key_press_time >= KEY_LONG_MS)
        {
            // 长按事件
            on_key_long();
            key_handled = 1;  // 标记已处理
        }
    }
    else if (!current && key_last_state)  // 释放瞬间
    {
        // 短按检测（按下50ms-1000ms之间释放）
        if (!key_handled && 
            key_press_time >= KEY_SHORT_MS && 
            key_press_time < KEY_LONG_MS)
        {
            // 短按事件
            on_key_short();
        }
    }
    else {
        LOG_I("unkonwn cmd");
    } 
    
    key_last_state = current;   
}

void lsh_dbg_key(void)
{
    for(;;)
    {
        key_scan();
        nrf_delay_ms(10);
    }
}