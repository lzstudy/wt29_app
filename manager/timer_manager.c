#include "timer_manager.h"
#include "drv_timer.h"
#include "msgbus.h"
#include "common.h"
#include "beep_manager.h"

typedef struct _timer_manager_context {
    bool state;
    uint16_t cycle;
    timer_cb_t cb;
    void *priv;
    msgbus_client_t *client;
}timer_manager_context_t;

static timer_manager_context_t timer_manager_context;

/**************************************************************************************************
 * @brief  : 获取定时器上下文
 * @param  : None
 * @return : None
**************************************************************************************************/
static inline timer_manager_context_t *get_timer_manager_context(void)
{
    return &timer_manager_context;
}

/**************************************************************************************************
 * @brief  : 定时器中断
 * @param  : None
 * @return : None
**************************************************************************************************/
static void timer_event_isr(void *data, uint16_t len, void *priv)
{
    unused(data);
    unused(len);

    timer_manager_context_t *cxt = priv;

    /* 回调定时器事件 */
    if(cxt->cb) {
        cxt->cb(cxt->priv);                     // 回调上层
        timer_restart(cxt->cycle);              // 重启定时器
    }
}

/**************************************************************************************************
 * @brief  : 定时器初始化
 * @param  : None
 * @return : None
**************************************************************************************************/
static void timer_manager_init(void)
{
    timer_manager_context_t *cxt = get_timer_manager_context();
    cxt->state = 0;
    timer_init();
}

/**************************************************************************************************
 * @brief  : 定时器启动
 * @param  : None
 * @return : None
**************************************************************************************************/
static void timer_manager_start(void)
{
    timer_manager_context_t *cxt = get_timer_manager_context();

    if(cxt->state == 1)
        return;

    cxt->state = 1;
    timer_start();
}

/**************************************************************************************************
 * @brief  : 定时器停止
 * @param  : None
 * @return : None
**************************************************************************************************/
static void timer_manager_stop(void)
{
    timer_manager_context_t *cxt = get_timer_manager_context();

    if(cxt->state == 0)
        return;

    cxt->state = 0;
    timer_stop();
}

/**************************************************************************************************
 * @brief  : 获取状态
 * @param  : None
 * @return : None
**************************************************************************************************/
static bool timer_manager_get_state(void)
{
    timer_manager_context_t *cxt = get_timer_manager_context();
    return cxt->state;
}

/**************************************************************************************************
 * @brief  : 设置周期
 * @param  : None
 * @return : None
**************************************************************************************************/
static void timer_manager_set_cycle(uint16_t cycle)
{
    timer_manager_context_t *cxt = get_timer_manager_context();
    cxt->cycle = cycle;
}

/**************************************************************************************************
 * @brief  : 定时器回调
 * @param  : None
 * @return : None
**************************************************************************************************/
static void timer_manager_set_cb(timer_cb_t cb, void *priv)
{
    timer_manager_context_t *cxt = get_timer_manager_context();

    cxt->cb = cb;
    cxt->priv = priv;

    /* 订阅timer事件 */
    cxt->client = msgbus()->subscribe_topic("timer", timer_event_isr, cxt);
}

static timer_manager_ops_t timer_manager_ops = {
    .init = timer_manager_init,
    .start = timer_manager_start,
    .stop = timer_manager_stop,
    .get_state = timer_manager_get_state,
    .set_cycle = timer_manager_set_cycle,
    .set_cb = timer_manager_set_cb,
};

/**************************************************************************************************
 * @brief  : timer manger
 * @param  : None
 * @return : None
**************************************************************************************************/
timer_manager_ops_t *timer_manager(void)
{
    return &timer_manager_ops;
}

/*===============================================================================*/
static void test_timer_isr(void* priv)
{
    unused(priv);
    beep_manager()->toggle();
}

/**************************************************************************************************
 * @brief  : timer调试
 * @param  : None
 * @return : None
**************************************************************************************************/
void lsh_dbg_timer(int argc, char *argv[])
{
    unused(argc);
    int index = atoi(argv[1]);
    uint16_t cycle;

    switch(index)
    {
    case 1:
        timer_manager()->set_cb(test_timer_isr, NULL);
        timer_manager()->set_cycle(1000);
        timer_manager()->start();
        break;

    case 2:
        timer_manager()->stop();
        break;

    case 3:
        cycle = atoi(argv[2]);
        LOG_I("cycle = %d", cycle);
        timer_manager()->set_cycle(cycle);
        break;

    }
    
}