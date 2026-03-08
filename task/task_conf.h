#ifndef __TASK_CONF_H__
#define __TASK_CONF_H__
#include "task_type.h"


/* 任务入口 */
extern void gui_task_entry(void *arg);
extern void key_task_entry(void *arg);
extern void lsh_task_entry(void *arg);

/* 任务配置表 */
static const task_cfg_t task_cfg_table[] = {
    {gui_task_entry,    "gui",      1024 * 20,    5},
    {lsh_task_entry,    "lsh",      1024,         4},
};


#endif
