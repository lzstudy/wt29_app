
#include "lsh.h"
#include "sys_config.h"
#include "common.h"
#include "lvgl.h"

static lv_obj_t *pic_obj;

/**************************************************************************************************
 * @brief  : 设置图片obj
 * @param  : None
 * @return : None
**************************************************************************************************/
void set_pic_obj(lv_obj_t *obj)
{
    pic_obj = obj;
}

/**************************************************************************************************
 * @brief  : 获取分辨率
 * @param  : None
 * @return : None
**************************************************************************************************/
void lsh_dbg_x(int argc, char *argv[])
{
    int x, offs;

    offs = atoi(argv[1]);

    x = lv_obj_get_x(pic_obj);
    x += offs;
    lv_obj_set_x(pic_obj, x);
}

/**************************************************************************************************
 * @brief  : 获取分辨率
 * @param  : None
 * @return : None
**************************************************************************************************/
void lsh_dbg_y(int argc, char *argv[])
{
    int y, offs;

    offs = atoi(argv[1]);

    y = lv_obj_get_y(pic_obj);
    y += offs;
    lv_obj_set_y(pic_obj, y);
}

/**************************************************************************************************
 * @brief  : 获取分辨率
 * @param  : None
 * @return : None
**************************************************************************************************/
void lsh_dbg_pos(int argc, char *argv[])
{
    int x = atoi(argv[1]);
    int y = atoi(argv[2]);

    if(argc == 1)
    {
        x = lv_obj_get_x(pic_obj);
        y = lv_obj_get_y(pic_obj);
        LOG_I("  [%d, %d]", x, y);
        return;
    }

    lv_obj_set_pos(pic_obj, x, y);
}