#ifndef __SCREEN_TYPE_H__
#define __SCREEN_TYPE_H__
#include "slist.h"

#define IMPORT_SCREEN(x)               extern gui_screen_t x;

typedef struct _screen_life_ops {
    void (*post)(void *arg);                                                    /**@ 进入事件 */
    void (*destory)(void *arg);                                                 /**@ 销毁事件 */
    void (*running)(void *arg);                                                 /**@ 运行态 */
}screen_life_ops_t;

typedef struct _screen_key_event {
    void (*up)(void *arg);                                                      /**@ 上键 */
    void (*down)(void *arg);                                                    /**@ 下键 */
    void (*ctrl)(void *);                                                       /**@ 控制按键 */
}screen_key_event_t;

typedef struct _gui_screen {
    const char *name;                                                           /**@ 屏幕名称 */
    struct list_head list;                                                      /**@ 节点*/
    screen_life_ops_t *sl_ops;                                                  /**@ 生命周期管理 */
    screen_key_event_t *sk_ev;                                                  /**@ 按键事件 */
}gui_screen_t;

typedef struct _gui_screen_config {
    gui_screen_t *screen;                                                        /**@ 屏幕 */
}gui_screen_config_t;

#endif
