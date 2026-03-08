#ifndef __MOTOR_MANAGER_H__
#define __MOTOR_MANAGER_H__
#include <stdint.h>
#include <stdbool.h>

typedef struct _motor_manager_ops {
    void (*init)(void);                                                         /**@ 初始化 */
    void (*enable)(bool enable);                                                /**@ 使能 */
    void (*start)(void);                                                        /**@ 启动 */
    void (*stop)(void);                                                         /**@ 停止 */
}motor_manager_ops_t;

motor_manager_ops_t *motor_manager(void);

#endif