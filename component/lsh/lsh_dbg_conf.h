#ifndef __LSH_DBG_CONF_H__
#define __LSH_DBG_CONF_H__
#include "lsh_type.h"

#define LSH_DBG_CMD_IMPORT(func)    extern void lsh_dbg_##func(int argc, char *argv[]);
#define LSH_DGB_CMD(func, help)     {#func, lsh_dbg_##func, help}

/* 命令导入 */
LSH_DBG_CMD_IMPORT(x);
LSH_DBG_CMD_IMPORT(y);
LSH_DBG_CMD_IMPORT(pos);
LSH_DBG_CMD_IMPORT(volume);
LSH_DBG_CMD_IMPORT(mon);
LSH_DBG_CMD_IMPORT(cc);
LSH_DBG_CMD_IMPORT(test);
LSH_DBG_CMD_IMPORT(bat_stat);
LSH_DBG_CMD_IMPORT(sys_setting);
LSH_DBG_CMD_IMPORT(qmc5883p);
LSH_DBG_CMD_IMPORT(timer);
LSH_DBG_CMD_IMPORT(magentic);
LSH_DBG_CMD_IMPORT(key);


/* 命令表 */
static lsh_cmd_info_t lsh_dbg_cmd_table[] = {
    LSH_DGB_CMD(x,            "set pic x"),
    LSH_DGB_CMD(y,            "set pic y"),
    LSH_DGB_CMD(pos,          "set pic pos"),
    LSH_DGB_CMD(volume,       "beep volume dbg"),
    LSH_DGB_CMD(mon,          "set mon progress"),
    LSH_DGB_CMD(cc,           "set mon progress"),
    LSH_DGB_CMD(test,         "test debug"),
    LSH_DGB_CMD(bat_stat,     "batter state"),
    LSH_DGB_CMD(sys_setting,  "system setting"),
    LSH_DGB_CMD(qmc5883p,     "qmc5883p dbg"),
    LSH_DGB_CMD(timer,        "timer dbg"),
    LSH_DGB_CMD(magentic,     "magentic dbg"),
    LSH_DGB_CMD(key,          "key dbg"),
};

#endif
