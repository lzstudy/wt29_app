#ifndef __MSGBUS_CONF_H__
#define __MSGBUS_CONF_H__
#include "msgbus_type.h"

typedef struct msgbus_conf {
    const char *name;                                                           /**@ 总线名称 */
    uint16_t q_cnt;                                                             /**@ 队列元素个数 */
    uint16_t q_size;                                                            /**@ 队列元素大小 */
    uint16_t priority;                                                          /**@ 任务优先级 */
    uint16_t stack_size;                                                        /**@ 任务堆栈大小 */
}msgbus_conf_t;

static const msgbus_conf_t msgbus_conf_tbl[] = {
    {"msgbus_normal",   10, sizeof(msgbus_msg_t), 4, 512},
    {"msgbus_priority", 10, sizeof(msgbus_msg_t), 5, 512},
    {"msgbus_urgent",   10, sizeof(msgbus_msg_t), 6, 512},
};

#endif
