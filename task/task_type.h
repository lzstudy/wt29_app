#ifndef __TASK_TYPE_H__
#define __TASK_TYPE_H__


typedef struct _task_cfg {
    void (*entry)(void *arg);                                                   /**@ 任务入口 */
    const char *name;                                                           /**@ 任务名称 */
    uint16_t stack_size;                                                        /**@ 堆栈大小 */
    uint8_t priority;                                                           /**@ 优先级 */
}task_cfg_t;

#endif