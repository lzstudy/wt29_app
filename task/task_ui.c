#include "common.h"
#include "lvgl.h"
#include "nrf_gpio.h"
#include "pic.h"
#include "screen_core.h"

/**************************************************************************************************
 * @brief  : GUI初始化
 * @param  : None
 * @return : None
**************************************************************************************************/
static void gui_init(void)
{
    /* lvgl初始化 */
    lv_init();

    /* lvgl硬件屏幕驱动初始化 */
    lv_port_disp_init();

    /* 设置背景颜色 */
    lv_obj_set_style_bg_color(lv_scr_act(), lv_color_white(), 0);
    lv_obj_set_style_bg_opa(lv_scr_act(), LV_OPA_COVER, 0);
    lv_timer_handler();
    lv_refr_now(NULL);

    /* 打开背光 */
    lcd_bl_ctrl(1);
}

/**************************************************************************************************
 * @brief  : 展示开机logo
 * @param  : None
 * @return : None
**************************************************************************************************/
static void disp_logo(void)
{
    lv_obj_t *img_logo;
    
    /* 创建logo图片 */
    img_logo = lv_img_create(lv_scr_act());
    lv_img_set_src(img_logo, &logo);

    /* 设置属性 */
    lv_obj_align(img_logo, LV_ALIGN_TOP_MID, 0, 150);
    lv_timer_handler();
    lv_refr_now(NULL);

    /* 销毁图像句柄 */
    if(img_logo && lv_obj_is_valid(img_logo)){
        lv_obj_del(img_logo);
        img_logo = NULL;
    }

    sleep(1);
}

/**************************************************************************************************
 * @brief  : UI任务入口
 * @param  : None
 * @return : None
**************************************************************************************************/
void gui_task_entry(void *arg)
{
    gui_init();                         // GUI硬件相关初始化
    disp_logo();                        // 展示开机LOGO
    // check_sn();                         // SN校验
    gui_frwk_start();                   // 进入GUI frwk

#if 0
    main_screen_init();                 // 主屏幕初始化
    toggle_screen("main");              // 切换到主屏幕

    while(1)
    {
        do_gui_work();
        lv_timer_handler();
        lv_refr_now(NULL);
        nrf_delay_ms(5);
    }
#endif
}



