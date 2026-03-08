#ifndef __BEEP_MANAGER_H__
#define __BEEP_MANAGER_H__
#include <stdint.h>
#include <stdbool.h>

typedef struct _beep_manager_ops {
    void (*init)(void);                                                         /**@ 初始化 */
    void (*start)(void);                                                        /**@ 启动beep */
    void (*stop)(void);                                                         /**@ 关闭beep */
    void (*toggle)(void);                                                       /**@ 翻转 */
    uint8_t (*get_volume)(void);                                                /**@ 获取音量 */
    void (*set_volume)(uint8_t volume);                                         /**@ 设置音量 */
    void (*single_paly)(uint8_t time);                                          /**@ 单次播放 */
}beep_manager_ops_t;

beep_manager_ops_t *beep_manager(void);

#endif
