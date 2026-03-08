#ifndef __LSH_TYPE_H__
#define __LSH_TYPE_H__
#include <stdint.h>
#include "slist.h"

#define LSH_ARG_MAX             32                                           /**# 参数最大数目 */
#define LSH_HISTORY_LINES       10                                           /**# 历史命令行 */
#define LSH_CMD_SIZE            80                                           /**# 命令大小 */

#define BUFFER_SIZE             1024                                         /**# 控制台缓冲大小 */

typedef struct _lsh_context lsh_context;
typedef void (*strcpy_cmd_t)(int argc, char *argv[]);   			         /**# 内置命令 */
typedef void (*strcpy_func_t)(int argc, char *argv[]);                       /**# 外部函数 */

typedef enum _input_stat
{
    WAIT_NORMAL,
    WAIT_SPEC_KEY,
    WAIT_FUNC_KEY,
}input_stat;

/* 命令信息 */
typedef struct _lsh_cmd_info {
    char *cmd;                                                                  /**@ 命令名称 */
    strcpy_cmd_t func;                                                          /**@ 命令函数 */
    const char *notice;                                                         /**@ 帮助信息 */
}lsh_cmd_info_t;

/* 函数结构 */
typedef struct _lsh_func_info {
    const char *name;                                                           /**@ 函数名称 */
    strcpy_func_t func;                                                         /**@ 处理函数 */
    struct list_head list;                                                      /**@ 链表节点 */
}lsh_func_info_t;

/* 变量结构 */
typedef struct _lsh_var_info {
    const char *name;                                                           /**@ 函数名称 */
    void *var;                                                                  /**@ 变量地址 */
    int type;                                                                   /**@ 类型 */
    struct list_head list;                                                      /**@ 链表节点 */
}lsh_var_info_t;

typedef struct _ob_var_info {
    void *start;                                                                /**@ 起始地址 */
    void *addr;                                                                 /**@ 观测的地址 */
    uint8_t width;                                                              /**@ 变量宽度 */
    uint8_t line;                                                               /**@ 行数 */
    uint16_t size;                                                              /**@ 观测大小 */
    uint8_t mode;                                                               /**@ 观测模式 */
}ob_var_info_t;

typedef struct _quick_key_mp {
    uint8_t code;                                                               /**@ 按键码 */
    void (*func)(lsh_context *cxt);                                          /**@ 处理函数 */
}quick_key_mp_t;

/* mgc - management cenger */
typedef struct _lsh_mgc_sym {
    char sym_file[128];                                                         /**@ 符号文件路径 */
    ob_var_info_t ovi;                                                          /**@ 观测信息 */
    struct list_head func_table;                                                /**@ 函数链表 */
    struct list_head var_table;                                                 /**@ 变量链表 */
    struct list_head rs_func_table;                                             /**@ 函数链表 */
    struct list_head rs_var_table;                                              /**@ 变量链表 */
}lsh_mgc_sym;

/* mgc - lsh 命令行管理 */
typedef struct _lsh_mgc_lsh {
    char prompt[64];                                                            /**@ 提示 */
    char line[128];                                                             /**@ 命令行 */
    char line_bak[128];                                                         /**@ 备份命令行 */
    input_stat stat;                                                            /**@ 输入状态 */
    uint16_t line_pos;                                                          /**@ 命令行位置 */
    uint16_t line_curpos;                                                       /**@ 命令行当前位置  */
    uint16_t current_history;                                                   /**@ 当前历史 */
    uint16_t history_count;                                                     /**@ 历史数量 */
    char cmd_history[LSH_HISTORY_LINES][LSH_CMD_SIZE];                          /**@ 历史记录 */
}lsh_mgc_lsh;


#endif

