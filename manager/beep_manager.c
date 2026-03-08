#include <stdbool.h>
#include "drv_beep.h"
#include "common.h"
#include "beep_manager.h"
#include "system.h"

typedef struct _bepp_manager_context {
    bool state;                                                                 /**@ beep状态 */
}beep_manager_context_t;

static beep_manager_context_t beep_manager_context;

/**************************************************************************************************
 * @brief  : 获取上下文
 * @param  : None
 * @return : None
**************************************************************************************************/
static inline beep_manager_context_t *get_beep_manager_context(void)
{
    return &beep_manager_context;
}

/**************************************************************************************************
 * @brief  : 电源模式初始化
 * @param  : None
 * @return : None
**************************************************************************************************/
static void beep_manager_init(void)
{
    beep_manager_context_t *cxt = get_beep_manager_context();


    cxt->state = 0;
    beep_init();
}

/**************************************************************************************************
 * @brief  : 电源模式初始化
 * @param  : None
 * @return : None
**************************************************************************************************/
static void beep_manager_start(void)
{
    beep_manager_context_t *cxt = get_beep_manager_context();

    if(cxt->state == 1)
        return;

    cxt->state = 1;
    beep_start();
}

/**************************************************************************************************
 * @brief  : 电源模式初始化
 * @param  : None
 * @return : None
**************************************************************************************************/
static void beep_manager_stop(void)
{
    beep_manager_context_t *cxt = get_beep_manager_context();

    if(cxt->state == 0)
        return;

    cxt->state = 0;
    beep_stop();
}

/**************************************************************************************************
 * @brief  : 电源模式初始化
 * @param  : None
 * @return : None
**************************************************************************************************/
static void beep_manager_toggle(void)
{
    beep_manager_context_t *cxt = get_beep_manager_context();

    cxt->state = !cxt->state;
    
    if(cxt->state)
        beep_start();
    else
        beep_stop();
}

/**************************************************************************************************
 * @brief  : 电源模式初始化
 * @param  : None
 * @return : None
**************************************************************************************************/
static uint8_t beep_manager_get_volume(void)
{
    return beep_get_volume();
}

/**************************************************************************************************
 * @brief  : 电源模式初始化
 * @param  : None
 * @return : None
**************************************************************************************************/
static void beep_manager_set_volume(uint8_t volume)
{
    beep_set_volume(volume);
}

/**************************************************************************************************
 * @brief  : 电源模式初始化
 * @param  : None
 * @return : None
**************************************************************************************************/
static void beep_manager_single_paly(uint8_t time)
{
	beep_manager_context_t *cxt = get_beep_manager_context();

    beep_single_play(time);
    cxt->state = 0;
}



static beep_manager_ops_t beep_manager_ops = {
    .init = beep_manager_init,
    .start = beep_manager_start,
    .stop = beep_manager_stop,
    .toggle = beep_manager_toggle,
    .get_volume = beep_manager_get_volume,
    .set_volume = beep_manager_set_volume,
    .single_paly = beep_manager_single_paly,
};


/**************************************************************************************************
 * @brief  : beep manger
 * @param  : None
 * @return : None
**************************************************************************************************/
beep_manager_ops_t *beep_manager(void)
{
    return &beep_manager_ops;
}