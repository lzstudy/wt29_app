#include "common.h"
#include "lsh.h"
#include "FreeRTOS.h"
#include "task.h"

/**************************************************************************************************
 * @brief  : 注册变量
 * @param  : None
 * @return : None
**************************************************************************************************/
void lsh_cmd_ti(int argc, char const *argv[])
{
    unused(argc);
    unused(argv);

    char buf[512];

    vTaskList(buf);


    LOG_I("%s", buf);
}