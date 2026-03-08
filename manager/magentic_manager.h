#ifndef __MAGENTIC_MANAGER_H__
#define __MAGENTIC_MANAGER_H__
#include <stdint.h>
#include <stdbool.h>

typedef void (*magentic_callback_t)(void *arg);

typedef struct _magentic_manager_ops {
    void (*init)(void);                                                         /**@ 初始化 */
    int16_t (*read_x)(void);
    int16_t (*read_y)(void);
    int16_t (*read_z)(void);
}magentic_manager_ops_t;

magentic_manager_ops_t *magentic_manager(void);

#endif