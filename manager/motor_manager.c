#include <stdbool.h>
#include "drv_misc.h"
#include "common.h"
#include "system.h"
#include "motor_manager.h"

typedef struct _motor_manager_context {
    bool enable;                                                                /**@ 全局使能 */
    bool stat;                                                                  /**@ motor状态 */
}motor_manager_context_t;

static motor_manager_context_t motor_manager_context;

/**************************************************************************************************
 * @brief  : motor manager初始化
 * @param  : None
 * @return : None
**************************************************************************************************/
static void motor_manager_init(void)
{
    system_setting_info_t *system_setting;
    motor_manager_context_t *cxt = &motor_manager_context;

    /* 获取设置信息 */
    system_setting = get_system_setting_info();
    cxt->enable = system_setting->flag_shake;
    cxt->stat = 0;

    motor_init();
}

/**************************************************************************************************
 * @brief  : motor manager师恩给你
 * @param  : None
 * @return : None
**************************************************************************************************/
static void motor_manager_enable(bool enable)
{
    motor_manager_context_t *cxt = &motor_manager_context;
    cxt->enable = enable;
}

/**************************************************************************************************
 * @brief  : 电机启动
 * @param  : None
 * @return : None
**************************************************************************************************/
static void motor_manager_start(void)
{
    motor_manager_context_t *cxt = &motor_manager_context;

    if(cxt->enable == 0 || cxt->stat == 1)
        return;

    cxt->stat = 1;
    motor_start();
}

/**************************************************************************************************
 * @brief  : 电机停止
 * @param  : None
 * @return : None
**************************************************************************************************/
static void motor_manager_stop(void)
{
    motor_manager_context_t *cxt = &motor_manager_context;

    if(cxt->enable == 0 || cxt->stat == 0)
        return;

    cxt->stat = 0;
    motor_stop();
}


static motor_manager_ops_t motor_manager_ops = {
    .init = motor_manager_init,
    .enable = motor_manager_enable,
    .start = motor_manager_start,
    .stop = motor_manager_stop,
};


/**************************************************************************************************
 * @brief  : beep manger
 * @param  : None
 * @return : None
**************************************************************************************************/
motor_manager_ops_t *motor_manager(void)
{
    return &motor_manager_ops;
}