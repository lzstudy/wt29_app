#include "common.h"
#include <string.h>
#include "slist.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "lsh_cmd_conf.h"
#include "lsh_type.h"


typedef struct _lsh_context {
    QueueHandle_t msg_q;                                                       /**@ 消息队列 */
    lsh_mgc_lsh mgc_lsh;                                                        /**@ lsh管理中心 */
    lsh_mgc_sym mgc_sym;                                                        /**@ 符号管理中心 */
    void *data;                                                                 /**@ 私有数据 */
}lsh_context;

static lsh_context lsh_cxt;

/**************************************************************************************************
 * @brief  : 串口接收中断
 * @param  : None
 * @return : None
**************************************************************************************************/
void uart_recv_isr(uint8_t ch)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xQueueSendFromISR(lsh_cxt.msg_q, &ch, &xHigherPriorityTaskWoken);
}



/**************************************************************************************************
 * @brief  : pty初始化
 * @param  : None
 * @return : None
**************************************************************************************************/
void lsh_cmd_clear(int argc, char *argv[])
{
    unused(argc);
    unused(argv);

    printf("\033[2J\033[H");
}

/**************************************************************************************************
 * @brief  : 帮助命令
 * @param  : None
 * @return : None
**************************************************************************************************/
void lsh_cmd_help(int argc, char *argv[])
{
    unused(argc);
    unused(argv);

    lsh_cmd_info_t *item;

    for(uint8_t i = 0 ; i < ARRAY_SIZE(sci_table) ; i++) {
        item = &sci_table[i];
        printf("    %-20s", item->cmd);
        printf("- %s\n", item->notice);
    }
}

/**************************************************************************************************
 * @brief  : 是否为系统命令
 * @param  : None
 * @return : None
**************************************************************************************************/
int process_cmd_sys(int argc, char *argv[])
{
    char *cmd;
    lsh_cmd_info_t *item;

    /* 默认在起始位置 */
    cmd = argv[0];

    /* 执行内置命令 */
    for(uint8_t i = 0 ; i < ARRAY_SIZE(sci_table) ; i++) 
    {
        item = &sci_table[i];

        /* 命令对比 */
        if(strcmp(item->cmd, cmd))
            continue;
    
        /* 处理函数 */
        item->func(argc, argv);
        return 1;
    }

    return 0;
}

/**************************************************************************************************
 * @brief  : 处理命理
 * @param  : None
 * @return : None
**************************************************************************************************/
void lsh_process_cmd(lsh_context *cxt, int argc, char *argv[])
{
    lsh_mgc_lsh *mgc = &cxt->mgc_lsh;

    if(process_cmd_sys(argc, argv))
        return;

    printf("%s: command not found\n", mgc->line_bak);
}


/**************************************************************************************************
 * @brief  : 打印提示
 * @param  : None
 * @return : None
**************************************************************************************************/
void lsh_print_prompt(lsh_context *cxt)
{
    unused(cxt);
    printf("lsh> ");
}

/**************************************************************************************************
 * @brief  : 命令解析
 * @param  : None
 * @return : None
**************************************************************************************************/
static int split_lsh_cmd(lsh_context *cxt, char *cmd, uint16_t length, char *argv[LSH_ARG_MAX])
{
    unused(argv)
    unused(length)
    strncpy(cxt->mgc_lsh.line, cmd, sizeof(cxt->mgc_lsh.line));

    return 0;
}

/**************************************************************************************************
 * @brief  : 执行命令
 * @param  : None
 * @return : None
**************************************************************************************************/
static int _lsh_exec_cmd(lsh_context *cxt, char *cmd, uint16_t length)
{
    int argc;
    uint16_t cmd0_size = 0;
    char *argv[LSH_ARG_MAX] = {0};
    lsh_mgc_lsh *mgc = &cxt->mgc_lsh;

    /* find the size of first command */
    while (cmd0_size < length && (cmd[cmd0_size] != ' ' && cmd[cmd0_size] != '\t'))
        cmd0_size ++;
    if (cmd0_size == 0)
        return -1;

    /* 提取参数 */
    argc = split_lsh_cmd(cxt, mgc->line, strlen(mgc->line), argv);
    if(argc == 0)
        return -1;

    printf("\n");

    /* 指令处理 */
    lsh_process_cmd(cxt, argc, argv);
    return 0;
}

/**************************************************************************************************
 * @brief  : 执行命令
 * @param  : None
 * @return : None
**************************************************************************************************/
int lsh_lsh_exec(lsh_context *cxt, char *cmd)
{
    int argc;
    char *argv[LSH_ARG_MAX] = {0};
    lsh_mgc_lsh *mgc = &cxt->mgc_lsh;

    /* 清空原来命令行 */
    memset(mgc->line, 0, sizeof(mgc->line));
    memset(mgc->line_bak, 0, sizeof(mgc->line_bak));

    /* 拷贝新命令 */
    strncpy(mgc->line, cmd, sizeof(mgc->line));
    strncpy(mgc->line_bak, cmd, sizeof(mgc->line));

    /* 处理命令 */
    argc = split_lsh_cmd(cxt, cmd, strlen(cmd), argv);
    if(argc == 0)
        return -1;

    /* 指令处理 */
    lsh_process_cmd(cxt, argc, argv);
    return 0;
}

/**************************************************************************************************
 * @brief  : 命令执行
 * @param  : None
 * @return : None
**************************************************************************************************/
int lsh_exec(lsh_context *cxt, char *cmd, uint16_t length)
{
    /* strim the beginning of command */
    while ((length > 0) && (*cmd  == ' ' || *cmd == '\t'))
    {
        cmd++;
        length--;
    }

    if (length == 0) {
        printf("\n");
        return 0;
    }

    _lsh_exec_cmd(cxt, cmd, length);
    return 0;
}

/**************************************************************************************************
 * @brief  : 历史记录
 * @param  : None
 * @return : None
**************************************************************************************************/
static bool shell_handle_history(lsh_context *cxt)
{
    lsh_mgc_lsh *mgc = &cxt->mgc_lsh;
    printf("\033[2K\r");
    printf("%s", "lsh> ");
    printf("%s", mgc->line);
    return false;
}

/**************************************************************************************************
 * @brief  : 保存历史
 * @param  : None
 * @return : None
**************************************************************************************************/
static void shell_push_history(lsh_context *cxt)
{
    lsh_mgc_lsh *mgc = &cxt->mgc_lsh;

    mgc->current_history = mgc->history_count;
}

/**************************************************************************************************
 * @brief  : 上侧按键
 * @param  : None
 * @return : None
**************************************************************************************************/
static void keyboard_event_up(lsh_context *cxt)
{
    lsh_mgc_lsh *mgc = &cxt->mgc_lsh;

    if (mgc->current_history > 0)
        mgc->current_history--;
    else {
        mgc->current_history = 0;
        return;
    }

    /* copy the history command */
    memcpy(mgc->line, &mgc->cmd_history[mgc->current_history][0], LSH_CMD_SIZE);
    mgc->line_curpos = mgc->line_pos = (uint16_t)strlen(mgc->line);
    shell_handle_history(cxt);
}

/**************************************************************************************************
 * @brief  : 下侧按键
 * @param  : None
 * @return : None
**************************************************************************************************/
static void keyboard_event_down(lsh_context *cxt)
{
    lsh_mgc_lsh *mgc = &cxt->mgc_lsh;

    if (mgc->current_history < mgc->history_count - 1)
        mgc->current_history ++;
    else
    {
        /* set to the end of history */
        if (mgc->history_count != 0)
            mgc->current_history = mgc->history_count - 1;

        return;
    }

    memcpy(mgc->line, &mgc->cmd_history[mgc->current_history][0],
            LSH_CMD_SIZE);
    mgc->line_curpos = mgc->line_pos = (uint16_t)strlen(mgc->line);
    shell_handle_history(cxt);
}

/**************************************************************************************************
 * @brief  : 右侧按键
 * @param  : None
 * @return : None
**************************************************************************************************/
static void keyboard_event_right(lsh_context *cxt)
{
    lsh_mgc_lsh *mgc = &cxt->mgc_lsh;

    if(mgc->line_curpos < mgc->line_pos)
    {
        printf("%c", mgc->line[mgc->line_curpos]);
        mgc->line_curpos++;
    }
}

/**************************************************************************************************
 * @brief  : 左按键事件
 * @param  : None
 * @return : None
**************************************************************************************************/
static void keyboard_event_left(lsh_context *cxt)
{
    lsh_mgc_lsh *mgc = &cxt->mgc_lsh;

    if(mgc->line_curpos)  
    {
        printf("\b");
        mgc->line_curpos--;
    }
}

/**************************************************************************************************
 * @brief  : 输入ctrl z, 进行行首和行尾跳转
 * @param  : None
 * @return : None
**************************************************************************************************/
static void quick_key_ctrl_c(lsh_context *cxt)
{
    lsh_mgc_lsh *mgc = &cxt->mgc_lsh;

    printf("^C\n");
    lsh_print_prompt(cxt);
    mgc->line_curpos = mgc->line_pos = 0;
}

/**************************************************************************************************
 * @brief  : 输入ctrl z, 进行行首和行尾跳转
 * @param  : None
 * @return : None
**************************************************************************************************/
static void quick_key_ctrl_x(lsh_context *cxt)
{
    lsh_mgc_lsh *mgc = &cxt->mgc_lsh;

    /* 都是在位置直接返回 */
    if(mgc->line_curpos == 0 && mgc->line_pos == 0)
        return;

    /* 如果当前位置在0处 */
    if(mgc->line_curpos == 0)
    {
        for(int i = 0; i < mgc->line_pos; i++)
            printf("\033[1C");

        mgc->line_curpos = mgc->line_pos;
        return;
    }

    /* 其他位置到行首 */
    for(int i = 0; i < mgc->line_curpos; i++)
        printf("\b");

    mgc->line_curpos = 0;   
}

static quick_key_mp_t mp_tbl[] = {
    {0x03,  quick_key_ctrl_c},                  // ctrl + c
    {0x18,  quick_key_ctrl_x},                  // ctrl + x
    {NULL,  NULL},
};

/**************************************************************************************************
 * @brief  : 检测是否为特殊码
 * @param  : None
 * @return : None
**************************************************************************************************/
static bool lsh_quick_key(lsh_context *cxt, uint8_t code)
{
    quick_key_mp_t *item = mp_tbl;

    while(item->func) 
    {
        if(item->code == code) 
        {
            item->func(cxt);
            return true;
        }
        item++;
    }

    return false;
}

/**************************************************************************************************
 * @brief  : 检测是否为特殊码
 * @param  : None
 * @return : None
**************************************************************************************************/
static int check_special_code(lsh_context *cxt, char ch)
{
    lsh_mgc_lsh *mgc = &cxt->mgc_lsh;

    /* 是否为快捷键 */
    if(lsh_quick_key(cxt, ch))
        return true;

    /*
    * handle control key
    * up key  : 0x1b 0x5b 0x41
    * down key: 0x1b 0x5b 0x42
    * right key:0x1b 0x5b 0x43
    * left key: 0x1b 0x5b 0x44
    */
    if (ch == 0x1b)
    {
        mgc->stat = WAIT_SPEC_KEY;
        return true;
    }
    else if (mgc->stat == WAIT_SPEC_KEY)
    {
        if (ch == 0x5b)
        {
            mgc->stat = WAIT_FUNC_KEY;
            return true;
        }

        mgc->stat = WAIT_NORMAL;
    }
    else if (mgc->stat == WAIT_FUNC_KEY)
    {
        mgc->stat = WAIT_NORMAL;

        if (ch == 0x41) /* up key */
        {
            keyboard_event_up(cxt);
            return true;
        }
        else if (ch == 0x42) /* down key */
        {
            keyboard_event_down(cxt);
            return true;
        }
        else if (ch == 0x44) /* left key */
        {
            keyboard_event_left(cxt);
            return true;
        }
        else if (ch == 0x43) /* right key */
        {
            keyboard_event_right(cxt);
            return true;
        }
    }

    return 0;
}

/**************************************************************************************************
 * @brief  : 命令解析
 * @param  : None
 * @return : None
**************************************************************************************************/
void lsh_cmd_parse(lsh_context *cxt, char ch)
{
    lsh_mgc_lsh *mgc = &cxt->mgc_lsh;

    /* 判断是否为特殊功能码 */
    if(check_special_code(cxt, ch))
        return;


    /* 结束符直接处理命令 */
    if(ch == '\n' || ch == '\r') 
    {
        strncpy(mgc->line_bak, mgc->line, sizeof(mgc->line));
        shell_push_history(cxt);
        lsh_exec(cxt, mgc->line, mgc->line_pos);
        lsh_print_prompt(cxt);
        memset(mgc->line, 0, sizeof(mgc->line));
        mgc->line_curpos = mgc->line_pos = 0;
        return;
    }

    /* 无效字符 */
    if(ch == '\0' || ch == 0xFF )
        return;

    /* 退格键处理 */
    if(ch == 0x7F || ch == 0x08)
    {
        /* note that shell->line_curpos >= 0 */
        if (mgc->line_curpos == 0)
            return;

        mgc->line_pos--;
        mgc->line_curpos--;

        if (mgc->line_pos > mgc->line_curpos)
        {
            int i;

            memmove(&mgc->line[mgc->line_curpos],
                    &mgc->line[mgc->line_curpos + 1],
                    mgc->line_pos - mgc->line_curpos);
            mgc->line[mgc->line_pos] = 0;

            printf("\b%s  \b", &mgc->line[mgc->line_curpos]);

            /* move the cursor to the origin position */
            for (i = mgc->line_curpos; i <= mgc->line_pos; i++)
            printf("\b");
        }
        else
        {
            printf("\b \b");
            mgc->line[mgc->line_pos] = 0;
        }

        return;
    }

    /* 回退后输入 */
    if(mgc->line_curpos < mgc->line_pos)
    {
        int i;

        memmove(&mgc->line[mgc->line_curpos + 1], &mgc->line[mgc->line_curpos],
                    mgc->line_pos - mgc->line_curpos);
        mgc->line[mgc->line_curpos] = ch;

        printf("%s", &mgc->line[mgc->line_curpos]);

        for (i = mgc->line_curpos; i < mgc->line_pos; i++)
            printf("\b");
    }
    else
    {
        mgc->line[mgc->line_pos] = ch;
        printf("%c", ch);
    }

    /* 保持用户输入的字符 */
    mgc->line_curpos++;
    mgc->line_pos++;
}


/**************************************************************************************************
 * @brief  : lsh设备初始化
 * @param  : None
 * @return : None
**************************************************************************************************/
static void lsh_dev_init(lsh_context *cxt)
{
    cxt->msg_q = xQueueCreate(10, sizeof(char));
}

/**************************************************************************************************
 * @brief  : BLE任务入口
 * @param  : None
 * @return : None
**************************************************************************************************/
void lsh_task_entry(void *arg)
{
    char ch;
    lsh_dev_init(&lsh_cxt);

    while(1)
    {
        xQueueReceive(lsh_cxt.msg_q, &ch, portMAX_DELAY);
        lsh_cmd_parse(&lsh_cxt, ch);
    }
}