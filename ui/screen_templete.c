#include "common.h"
#include "screen_type.h"


/**************************************************************************************************
 * @brief  : main生命周期 - 进入
 * @param  : None
 * @return : None
**************************************************************************************************/
static void main_sl_post(void *arg)
{
    LOG_I("%s", __func__);
}

/**************************************************************************************************
 * @brief  : main生命周期 - 销毁
 * @param  : None
 * @return : None
**************************************************************************************************/
static void main_sl_destory(void *arg)
{
    LOG_I("%s", __func__);
}


static screen_life_ops_t main_sl_ops = {
    .post = main_sl_post,
    .destory = main_sl_destory
};

/**************************************************************************************************
 * @brief  : main生命周期 - 销毁
 * @param  : None
 * @return : None
**************************************************************************************************/
static void main_scr_kev_up(void *arg)
{
    LOG_I("%s", __func__);
}

/**************************************************************************************************
 * @brief  : main生命周期 - 销毁
 * @param  : None
 * @return : None
**************************************************************************************************/
static void main_scr_kev_down(void *arg)
{
    LOG_I("%s", __func__);
}

/**************************************************************************************************
 * @brief  : main按键 - 控制
 * @param  : None
 * @return : None
**************************************************************************************************/
static void main_scr_kev_ctrl(void *arg)
{
    LOG_I("%s", __func__);
}


static screen_key_event_t main_sk_ev = {
    .up   = main_scr_kev_up,
    .down = main_scr_kev_down,
    .ctrl = main_scr_kev_ctrl,
};

static gui_screen_t main_screen = {
    .name = "main",
    .sl_ops = &main_sl_ops,
    .sk_ev  = &main_sk_ev,
};



