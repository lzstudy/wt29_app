#ifndef __BEEP_KEY_MANAGER_H__
#define __BEEP_KEY_MANAGER_H__
#include <stdint.h>
#include <stdbool.h>

typedef struct _beep_key_manager_ops {
    void (*enable)(bool state);                                                 /**@ 设置音量 */
    void (*play)(void);                                                         /**@ 播放音量 */
}beep_key_manager_ops_t;

beep_key_manager_ops_t *beep_key_manager(void);

#endif