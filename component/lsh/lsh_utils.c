#include <stdio.h>
#include <stdlib.h>
#include "lsh_type.h"
#include "common.h"
#include "lsh.h"

/**************************************************************************************************
 * @brief  : 命令匹配表
 * @param  : None
 * @return : None
**************************************************************************************************/
int match_opt_table(void *tbl, const char *opt, int argc, char const *argv[])
{
    cmd_opt_map_t *item = tbl;

    while(item->func) 
    {
        if(!strcmp(item->opt, opt)) 
        {
            item->func(argc, argv);
            return 1;
        }
        item++;
    }

    return 0;
}