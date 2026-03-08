#ifndef __ALARM_MANAGER_H__
#define __ALARM_MANAGER_H__
#include <stdbool.h>
#include <stdint.h>

typedef struct _alarm_manager_ops {
    void (*enable)(bool enable);                                                /**@ 是否使能 */
    void (*init)(void);                                                         /**@ 初始化 */
    void (*exit)(void);                                                         /**@ 退出 */
    void (*set)(bool type);                                                     /**@ 设置模式 : 高音/低音 */
    void (*start)(void);                                                        /**@ 启动 */
    void (*stop)(void);                                                         /**@ 结束 */
    void (*toggle)(void);                                                       /**@ 翻转 */
    void (*single_play)(uint8_t time);                                          /**@ 播放 */
}alarm_manager_ops_t;

alarm_manager_ops_t *alarm_manager(void);                                       /**# alarm Manager */

#endif