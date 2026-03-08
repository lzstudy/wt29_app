#ifndef __ACCEL_MANAGER_H___
#define __ACCEL_MANAGER_H___
#include <stdbool.h>
#include "drv_sc7a20h.h"

typedef void (*accel_callback_t)(void *arg);

typedef struct _accel_manager_ops {
    void (*init)(void);                                                         /**@ 初始化 */
    void (*enable)(bool enable);                                                /**@ 是否使能 */
}accel_manager_ops_t;

accel_manager_ops_t *accel_manager(void);

#endif