#include "nrfx_timer.h"
#include "drv_timer.h"
#include "common.h"
#include "msgbus.h"

static nrfx_timer_t timer_mon = NRFX_TIMER_INSTANCE(1);

/**************************************************************************************************
 * @brief  : timer初始化
 * @param  : None
 * @return : None
**************************************************************************************************/
static void timer_handler(nrf_timer_event_t event_type, void *p_context)
{
    msgbus()->publish_topic("timer", NULL, 0);
}


/**************************************************************************************************
 * @brief  : 重启定时器
 * @param  : None
 * @return : None
**************************************************************************************************/
void timer_restart(uint16_t cycle)
{
    uint32_t ticks;

    /* 清0定时器 */
    nrfx_timer_clear(&timer_mon);

    /* 重新更新定时器时间 */
    ticks = nrfx_timer_ms_to_ticks(&timer_mon, cycle);
    nrfx_timer_compare(&timer_mon, NRF_TIMER_CC_CHANNEL0, ticks, true);

    /* 重新启动定时器 */
    nrfx_timer_resume(&timer_mon);
}

/**************************************************************************************************
 * @brief  : timer初始化
 * @param  : None
 * @return : None
**************************************************************************************************/
void timer_init(void)
{
    nrfx_timer_config_t timer_cfg = NRFX_TIMER_DEFAULT_CONFIG;
    
    /* 定时器配置 */
    timer_cfg.frequency = NRF_TIMER_FREQ_1MHz;      // 1MHz时钟
    timer_cfg.bit_width = NRF_TIMER_BIT_WIDTH_32;   // 32位模式
    timer_cfg.interrupt_priority = 7;               // 中断优先级
    
    /* 定时器初始化 */
    nrfx_timer_init(&timer_mon, &timer_cfg, timer_handler);

    
    /* 时间转为ms */
    uint32_t ticks = nrfx_timer_ms_to_ticks(&timer_mon, 10);
    
    /* 采用比较器0 */
    nrfx_timer_compare(&timer_mon, NRF_TIMER_CC_CHANNEL0, ticks, true);
}

/**************************************************************************************************
 * @brief  : timer开始
 * @param  : None
 * @return : None
**************************************************************************************************/
void timer_start(void)
{
    nrfx_timer_enable(&timer_mon);
    nrfx_timer_resume(&timer_mon);
}

/**************************************************************************************************
 * @brief  : timer停止
 * @param  : None
 * @return : None
**************************************************************************************************/
void timer_stop(void)
{
    nrfx_timer_disable(&timer_mon);

    /* 清除事件标志 */
    nrf_timer_event_clear(timer_mon.p_reg, NRF_TIMER_EVENT_COMPARE0);
}
