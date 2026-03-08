#ifndef __TIMER_MANAGER_H__
#define __TIMER_MANAGER_H__
#include <stdint.h>
#include <stdbool.h>
#include "drv_timer.h"

typedef void (*timer_cb_t)(void *priv);

typedef struct _timer_manager_ops {
    void (*init)(void);                                                         /**@ 初始化 */
    void (*start)(void);                                                        /**@ 启动 */
    void (*stop)(void);                                                         /**@ 停止 */
    bool (*get_state)(void);                                                    /**@ 获取状态 */
    void (*set_cycle)(uint16_t cycle);                                          /**@ 设置周期 */
    void (*set_cb)(timer_cb_t cb, void *priv);                                  /**@ 设置回调 */
}timer_manager_ops_t;

timer_manager_ops_t *timer_manager(void);

#endif