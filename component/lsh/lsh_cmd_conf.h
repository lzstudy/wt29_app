#ifndef __LSH_CMD_CONF_H__
#define __LSH_CMD_CONF_H__
#include "lsh_type.h"

#define LSH_CMD_IMPORT(func)     extern void lsh_cmd_##func(int argc, char *argv[]);
#define LSH_CMD(func, help)      {#func, lsh_cmd_##func, help}
#define LSH_QUE(func, help)      {#func, lsh_cmd_help, help}

/* 命令导入 */
LSH_CMD_IMPORT(help);
LSH_CMD_IMPORT(mm);
LSH_CMD_IMPORT(pin);
LSH_CMD_IMPORT(pinmap);
LSH_CMD_IMPORT(topic);
LSH_CMD_IMPORT(ti);
LSH_CMD_IMPORT(sn);
LSH_CMD_IMPORT(res);
LSH_CMD_IMPORT(dbg);
LSH_CMD_IMPORT(version);
LSH_CMD_IMPORT(reset);
LSH_CMD_IMPORT(clear);
LSH_CMD_IMPORT(i2c0);
LSH_CMD_IMPORT(i2c1);

LSH_CMD_IMPORT(ir);
LSH_CMD_IMPORT(motor);
LSH_CMD_IMPORT(test);



static lsh_cmd_info_t sci_table[] = {
    LSH_QUE(?,                  "help cmd"),
    LSH_CMD(help,               "help cmd"),
    LSH_CMD(pin,                "pin cmd"),
    LSH_CMD(pinmap,             "pin map calculate"),
    LSH_CMD(ti,                 "list thread info"),
    LSH_CMD(sn,                 "sn control"),
    LSH_CMD(mm,                 "memory operation"),
    LSH_CMD(topic,              "msgbus topic control"),
    LSH_CMD(res,                "screen resoluton"),
    LSH_CMD(dbg,                "lsh cmd dbg"),
    LSH_CMD(version,            "application verion"),
    LSH_CMD(reset,              "system reset"),
    LSH_CMD(clear,              "clear screen"),
    LSH_CMD(i2c0,               "i2c0 control"),
    LSH_CMD(i2c1,               "i2c1 control"),
    LSH_CMD(ir,                 "ir control"),
    LSH_CMD(motor,              "motor control"),
    LSH_CMD(test,               "test cmd"),
};

#endif
