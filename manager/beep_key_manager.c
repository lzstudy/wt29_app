#include <stdbool.h>
#include "drv_beep.h"
#include "common.h"
#include "system.h"
#include "beep_manager.h"
#include "beep_key_manager.h"

#define TIME_BEEP_KEY       10                                                  /**@ 按键声音持续时间, 单位ms */

typedef struct _bepp_key_manager_context {
    bool enable;                                                                /**@ 全局使能 */
}beep_key_manager_context_t;

static beep_key_manager_context_t beep_key_manager_context;

/**************************************************************************************************
 * @brief  : 是否使能
 * @param  : None
 * @return : None
**************************************************************************************************/
static void beep_key_manager_enable(bool enable)
{
    beep_key_manager_context_t *cxt = &beep_key_manager_context;

    cxt->enable = enable;
}

/**************************************************************************************************
 * @brief  : 播放声音
 * @param  : None
 * @return : None
**************************************************************************************************/
static void beep_key_manager_play(void)
{
    beep_key_manager_context_t *cxt = &beep_key_manager_context;

    /* 如果没有使能, 则返回 */
    if(cxt->enable == 0)
        return;

    beep_manager()->single_paly(TIME_BEEP_KEY);
}



static beep_key_manager_ops_t beep_key_manager_ops = {
    .enable = beep_key_manager_enable,
    .play = beep_key_manager_play,
};

/**************************************************************************************************
 * @brief  : beep manger
 * @param  : None
 * @return : None
**************************************************************************************************/
beep_key_manager_ops_t *beep_key_manager(void)
{
    return &beep_key_manager_ops;
}
