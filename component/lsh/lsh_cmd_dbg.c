#include <stdio.h>
#include <string.h>
#include "lsh.h"
#include "common.h"
#include "lsh_dbg_conf.h"

/**************************************************************************************************
 * @brief  : 打印帮助信息
 * @param  : None
 * @return : None
**************************************************************************************************/
void lsh_dbg_test(int argc, char *argv[])
{
    LOG_I("dbg teest %d", argc);
}

/**************************************************************************************************
 * @brief  : 打印帮助信息
 * @param  : None
 * @return : None
**************************************************************************************************/
static void lsh_dbg_help(void)
{
    lsh_cmd_info_t *item;

    for(uint8_t i = 0 ; i < ARRAY_SIZE(lsh_dbg_cmd_table) ; i++) {
        item = &lsh_dbg_cmd_table[i];
        printf("  %02d", i + 1);
        printf("  %-20s", item->cmd);
        printf("- %s\n", item->notice);
    }
}

/**************************************************************************************************
 * @brief  : dbg属性
 * @param  : None
 * @return : None
**************************************************************************************************/
void lsh_cmd_dbg(int argc, char *argv[])
{
    int number;
    char *cmd;
    lsh_cmd_info_t *item;

    /* 只有1个参数时, 打印调试信息 */
    if(argc == 1) {
        lsh_dbg_help();
        return;
    }

    
    /* 设置命令 */
    cmd = argv[1];

    /* 通过数字索引 */
    number = strtol(cmd, NULL, 10);
    if(number != 0)
    {
        item = &lsh_dbg_cmd_table[number - 1];
        item->func(argc - 1, &argv[1]);
        return;
    }

    /* 通过字符串索引 */
    for(int i = 0 ; i < ARRAY_SIZE(lsh_dbg_cmd_table) ; i++)
    {
        item = &lsh_dbg_cmd_table[i];

        /* 命令对比 */
        if(strcmp(item->cmd, cmd))
            continue;
            
        /* 处理函数 */
        item->func(argc - 1, &argv[1]);
        return;
    }

    LOG_I("unsupport dbg cmd %s", cmd);
}