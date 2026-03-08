#include <stdbool.h>
#include "common.h"
#include "beep_manager.h"
#include "alarm_manager.h"

#define VOLUME_ALARM_HIGH     50
#define VOLUME_ALARM_LOW      10

typedef struct _alarm_manager_context {
    bool enable;                                                                /**@ 是否使能 */
    uint8_t alarm_volume;                                                       /**@ 报警音量, 1为高音 */
    uint8_t restore_volume;                                                     /**@ 之前的音量 */
    uint8_t stat;                                                               /**@ 状态 */
}alarm_manager_context_t;

static alarm_manager_context_t alarm_manager_context;

/**************************************************************************************************
 * @brief  : 获取上下文
 * @param  : None
 * @return : None
**************************************************************************************************/
static inline alarm_manager_context_t *getalarm_manager_context(void)
{
    return &alarm_manager_context;
}

/**************************************************************************************************
 * @brief  : 报警使能
 * @param  : None
 * @return : None
**************************************************************************************************/
static void alarm_manager_enable(bool enable)
{
    alarm_manager_context_t *cxt = getalarm_manager_context();
    cxt->enable = enable;
}

/**************************************************************************************************
 * @brief  : 报警初始化
 * @param  : None
 * @return : None
**************************************************************************************************/
static void alarm_manager_init(void)
{
	alarm_manager_context_t *cxt = getalarm_manager_context();
    cxt->restore_volume = beep_manager()->get_volume();
}

/**************************************************************************************************
 * @brief  : 退出
 * @param  : None
 * @return : None
**************************************************************************************************/
static void alarm_manager_exit(void)
{
	alarm_manager_context_t *cxt = getalarm_manager_context();
    beep_manager()->set_volume(cxt->restore_volume);
}


/**************************************************************************************************
 * @brief  : 设置模式
 * @param  : None
 * @return : None
**************************************************************************************************/
static void alarm_manager_set(bool type)
{
    alarm_manager_context_t *cxt = getalarm_manager_context();
    cxt->alarm_volume = type ? VOLUME_ALARM_HIGH : VOLUME_ALARM_LOW;
}

/**************************************************************************************************
 * @brief  : 报警启动
 * @param  : None
 * @return : None
**************************************************************************************************/
static void alarm_manager_start(void)
{
    alarm_manager_context_t *cxt = getalarm_manager_context();

    /* 如果没使能, 直接返回 */
    if(!cxt->enable)
        return;

    /* 设置音量并启动蜂鸣器 */
    beep_manager()->set_volume(cxt->alarm_volume);
    beep_manager()->start();
}

/**************************************************************************************************
 * @brief  : 报警结束
 * @param  : None
 * @return : None
**************************************************************************************************/
static void alarm_manager_stop(void)
{
    alarm_manager_context_t *cxt = getalarm_manager_context();
    
    /* 如果没使能, 直接返回 */
    if(!cxt->enable)
        return;

    /* 停止蜂鸣器, 并恢复之前的音量 */
    beep_manager()->stop();

}

/**************************************************************************************************
 * @brief  : alarm翻转
 * @param  : None
 * @return : None
**************************************************************************************************/
static void alarm_manager_toggle(void)
{
    alarm_manager_context_t *cxt = getalarm_manager_context();

    /* 如果没使能, 直接返回 */
    if(!cxt->enable)
        return;

    cxt->stat = !cxt->stat;

    /* 判断状态 */
    if(cxt->stat)
    {
        cxt->restore_volume = beep_manager()->get_volume();
        beep_manager()->set_volume(cxt->alarm_volume);
        beep_manager()->start();
    }
    else    /* 停止并恢复状体 */
    {
        beep_manager()->set_volume(cxt->restore_volume);
        beep_manager()->stop();
    }
}

/**************************************************************************************************
 * @brief  : 报警结束
 * @param  : None
 * @return : None
**************************************************************************************************/
static void alarm_manager_single_play(uint8_t time)
{
    alarm_manager_context_t *cxt = getalarm_manager_context();

    /* 如果没使能, 直接返回 */
    if(!cxt->enable)
        return;

    /* 设置音量 */
    cxt->restore_volume = beep_manager()->get_volume();
    beep_manager()->set_volume(cxt->alarm_volume);

    /* 播放音量 */
    beep_manager()->single_paly(time);

    /* 恢复状态 */
    beep_manager()->set_volume(cxt->restore_volume);
}

static alarm_manager_ops_t alarm_manager_ops = {
    .enable = alarm_manager_enable,
    .init = alarm_manager_init,
    .exit = alarm_manager_exit,
    .set = alarm_manager_set,
    .start = alarm_manager_start,
    .stop = alarm_manager_stop,
    .toggle = alarm_manager_toggle,
    .single_play = alarm_manager_single_play,
};


/**************************************************************************************************
 * @brief  : beep manger
 * @param  : None
 * @return : None
**************************************************************************************************/
alarm_manager_ops_t *alarm_manager(void)
{
    return &alarm_manager_ops;
}

#if 0
void lsh_cmd_dbg(int argc, char const *argv[])
{
    alarm_manager_context_t *cxt = getalarm_manager_context();

    LOG_I("enable = %d", cxt->enable);
    LOG_I("alarm volume = %d", cxt->alarm_volume);
    LOG_I("restore volume = %d", cxt->restore_volume);
}
#endif