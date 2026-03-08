#include "lsh.h"
#include "sys_config.h"
#include "common.h"
#include "lvgl.h"
#include "nrf_nvic.h"

/**************************************************************************************************
 * @brief  : pin命令
 * @param  : None
 * @return : None
**************************************************************************************************/
void lsh_cmd_version(int argc, char *argv[])
{
    unused(argc);
    unused(argv);

    LOG_I("%d.%d.%d", MAJOR_VERSION, SLAVE_VERSION, PATH_VERSION);
}

/**************************************************************************************************
 * @brief  : 获取分辨率
 * @param  : None
 * @return : None
**************************************************************************************************/
void lsh_cmd_res(int argc, char *argv[])
{
    unused(argc);
    unused(argv);
    LOG_I("%d x %d", lv_disp_get_hor_res(NULL), lv_disp_get_ver_res(NULL));
}

/**************************************************************************************************
 * @brief  : 获取分辨率
 * @param  : None
 * @return : None
**************************************************************************************************/
void lsh_cmd_reset(int argc, char *argv[])
{
    unused(argc);
    unused(argv);
    NVIC_SystemReset();
}