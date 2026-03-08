#include "lvgl.h"
#include "nrf_delay.h"
#include "screen_core.h"
#include "mon_widget.h"
#include "common.h"
#include "text.h"
#include "pic.h"
#include "screen_utils.h"
#include "mon_widget_type.h"
#include "beep_manager.h"
#include "timer_manager.h"
#include "motor_manager.h"

#define PROG_FLASH_TIME_HIDDEN              30                                  /**< 顶部闪烁隐藏时间*/
#define PROG_FLASH_TIME_VISIBLE             170                                 /**< 顶部闪烁显示时间 */

/* beep 周期, 单位ms, 对应16个进度 */
static uint32_t beep_cycle[] = {
    200, 180, 160, 150, 
    130, 120, 110, 100, 
    80,  70,  65,  60,   
    55,  50,  45,  40
};

/**************************************************************************************************
 * @brief  : 定时器事件, 主要用于报警, 中断中将beep翻转
 * @param  : None
 * @return : None
**************************************************************************************************/
static void mon_timer_event(void *priv)
{
    LOG_I("addr = %p", priv);
    beep_manager()->toggle();
}

/**************************************************************************************************
 * @brief  : 创建mmp
 * @param  : None
 * @return : None
**************************************************************************************************/
static lv_obj_t *create_mon_mmp_cell(lv_obj_t *parent)
{
    lv_obj_t *image = lv_img_create(parent);
    lv_img_set_src(image, &pic_blue_item);

    lv_obj_align(image, LV_ALIGN_TOP_MID, 0, 0);
    return image;
}

/**************************************************************************************************
 * @brief  : 最顶层的进度闪烁
 * @param  : None
 * @return : None
**************************************************************************************************/
static void top_progress_flash(mon_main_prog_t *mmp, uint8_t progress)
{
    bool flag;
    lv_obj_t *obj = mmp->item[progress];

    flag = lv_obj_has_flag(obj, LV_OBJ_FLAG_HIDDEN);

    /* 计算周期, 闪烁 */
    if(flag) {
        if(check_cycle(&mmp->top_prog_flash_time, PROG_FLASH_TIME_HIDDEN)) {
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_HIDDEN);
        }
    } else {
        if(check_cycle(&mmp->top_prog_flash_time, PROG_FLASH_TIME_VISIBLE))
            lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
    }
}

/**************************************************************************************************
 * @brief  : 根据定时周期设置进度
 * @param  : None
 * @return : None
**************************************************************************************************/
static inline void set_timer_cycle_by_progress(uint8_t progress)
{
    uint16_t cycle = beep_cycle[progress];
    timer_manager()->set_cycle(cycle);
}


/**************************************************************************************************
 * @brief  : 设置进度
 * @param  : None
 * @return : None
**************************************************************************************************/
void set_main_progress(mon_main_prog_t *mmp, uint8_t progress)
{
    /* 超过最大值，按最大值计算 */
    if(progress > MON_MAIN_CNT) 
        progress = MON_MAIN_CNT;

    /* 开启电机 */
    motor_manager()->start();
    timer_manager()->start();

    /* 设置定时器周期 */
    set_timer_cycle_by_progress(progress);


    /* 如果两次相同, 顶层闪烁 */
    if(progress == mmp->prog)
    {
        top_progress_flash(mmp, progress);
        return;
    }

    /* 进度大于上次的进度, 则向上增长 */
    if(progress > mmp->prog)
    {
        for(int i = mmp->prog ; i <= progress ; i++)
        {
            lv_obj_clear_flag(mmp->item[i], LV_OBJ_FLAG_HIDDEN);
            mon_update_now(10);
        }

        mmp->prog = progress;
        return;
    }

    /* 减弱 */
    if(progress < mmp->prog)
    {
        for(int i = mmp->prog ; i >= progress ; i--)
        {
            lv_obj_add_flag(mmp->item[i], LV_OBJ_FLAG_HIDDEN);
            mon_update_now(10);
        }

        /* 如果退到0, 则关闭相关硬件 */
        if(progress == 0)
        {
            motor_manager()->stop();
            beep_manager()->stop();
            timer_manager()->stop();
        }

        mmp->prog = progress;
        return;
    } 
}

/**************************************************************************************************
 * @brief  : 设置模式 - level
 * @param  : None
 * @return : None
**************************************************************************************************/
void set_mon_mode_prog(mon_widget_t *widget)
{
    mon_main_prog_t *mmp = &widget->mmp;

    /* 设置子进度条 */
    set_mon_sub_progress(&widget->msp, 0);                          // 清子进度条

    /* 设置基础参数 */
    mmp->top_prog_flash_time = 0;                                   // 刷新时间为0
    mmp->prog = 0;                                                  // 设置prog为0
}


/**************************************************************************************************
 * @brief  : 创建MMP - item
 * @param  : None
 * @return : None
**************************************************************************************************/
static void create_mon_mmp_item(mon_widget_t *widget)
{
    int pos_y = MON_MAIN_POS_START;
    mon_main_prog_t *mmp = &widget->mmp;

    /* 创建16个进度 - 默认为蓝色 */
    for(int i = 0 ; i < MON_MAIN_CNT ; i++)
    {
        mmp->item[i] = create_mon_mmp_cell(widget->parent);
        lv_obj_set_y(mmp->item[i], pos_y);
        pos_y -= MON_MAIN_INTERVAL;
    }
}

/**************************************************************************************************
 * @brief  : MON - MMP初始化, 主要PROGRESS模式相关的初始化(使用图片作为进度条)
 * @param  : None
 * @return : None
**************************************************************************************************/
void mon_mmp_init(mon_widget_t *widget)
{
    mon_main_prog_t *mmp = &widget->mmp;

    mmp->prog = 0;
    mmp->top_prog_flash_time = 0;
    create_mon_mmp_item(widget);                        // 创建进度条
    timer_manager()->set_cb(mon_timer_event, mmp);      // 设置定时器回调, 用于蜂鸣器控制
}


/**************************************************************************************************
 * @brief  : 注册变量
 * @param  : None
 * @return : None
**************************************************************************************************/
void lsh_dbg_cc(int argc, char const *argv[])
{
    unused(argc);
    unused(argv);
    LOG_I("%s", __func__);
}