#include <stdbool.h>
#include "common.h"
#include "accel_manager.h"
#include "msgbus.h"

typedef struct _accel_manager_context {
    bool enable;                                                                /**@ 使能状态 */
}accel_manager_context_t;

static accel_manager_context_t accel_manager_context;

/**************************************************************************************************
 * @brief  : 获取上下文
 * @param  : None
 * @return : None
**************************************************************************************************/
static inline accel_manager_context_t *get_accel_manager_context(void)
{
    return &accel_manager_context;
}

/**************************************************************************************************
 * @brief  : 加速计事件中断
 * @param  : None
 * @return : None
**************************************************************************************************/
static void accel_event_isr(void)
{
    msgbus()->publish_topic("accel", NULL, 0);
}

/**************************************************************************************************
 * @brief  : accel manager初始化
 * @param  : None
 * @return : None
**************************************************************************************************/
static void accel_manager_init(void)
{
    accel_manager_context_t *cxt = get_accel_manager_context();
    cxt->enable = false;
    sc7a20h_init(accel_event_isr);
}

/**************************************************************************************************
 * @brief  : accel manager初始化
 * @param  : None
 * @return : None
**************************************************************************************************/
static void accel_manager_enable(bool enable)
{
    accel_manager_context_t *cxt = get_accel_manager_context();
    cxt->enable = enable;
}

static accel_manager_ops_t accel_manager_ops = {
    .init = accel_manager_init,
    .enable = accel_manager_enable,
};


/**************************************************************************************************
 * @brief  : beep manger
 * @param  : None
 * @return : None
**************************************************************************************************/
accel_manager_ops_t *accel_manager(void)
{
    return &accel_manager_ops;
}