#include <stdbool.h>
#include "common.h"
#include "magentic_manager.h"
#include "drv_qmc5883p.h"

typedef struct _magentic_manager_context {
    bool enable;                                                                /**@ 使能状态 */
}magentic_manager_context_t;

static magentic_manager_context_t magentic_manager_context;

/**************************************************************************************************
 * @brief  : 获取上下文
 * @param  : None
 * @return : None
**************************************************************************************************/
static inline magentic_manager_context_t *get_magentic_manager_context(void)
{
    return &magentic_manager_context;
}

/**************************************************************************************************
 * @brief  : magentic manager初始化
 * @param  : None
 * @return : None
**************************************************************************************************/
static void magentic_manager_init(void)
{
    magentic_manager_context_t *cxt = get_magentic_manager_context();
    cxt->enable = false;
    qmc5883p_init();
}

/**************************************************************************************************
 * @brief  : 读取x
 * @param  : None
 * @return : None
**************************************************************************************************/
static int16_t magentic_manager_read_x(void)
{
    return qmc5883p_read_x();
}

/**************************************************************************************************
 * @brief  : 读取x
 * @param  : None
 * @return : None
**************************************************************************************************/
static int16_t magentic_manager_read_y(void)
{
    return qmc5883p_read_y();
}

/**************************************************************************************************
 * @brief  : 读取x
 * @param  : None
 * @return : None
**************************************************************************************************/
static int16_t magentic_manager_read_z(void)
{
    return qmc5883p_read_z();
}


static magentic_manager_ops_t magentic_manager_ops = {
    .init = magentic_manager_init,
    .read_x = magentic_manager_read_x,
    .read_y = magentic_manager_read_y,
    .read_z = magentic_manager_read_z,
};


/**************************************************************************************************
 * @brief  : beep manger
 * @param  : None
 * @return : None
**************************************************************************************************/
magentic_manager_ops_t *magentic_manager(void)
{
    return &magentic_manager_ops;
}


/*------------------------ 调试接口 ------------------------*/
void lsh_dbg_magentic(int argc, char *argv[])
{
    unused(argc);
    unused(argv);

    int16_t x, y, z;
    
    for(;;)
    {
        x = magentic_manager()->read_x();
        y = magentic_manager()->read_y();
        z = magentic_manager()->read_z();

        LOG_I("%d %d %d", x, y, z);
        sleep(1);
    }
}