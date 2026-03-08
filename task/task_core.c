#include "common.h"
#include "FreeRTOS.h"
#include "task.h"
#include "task_type.h"
#include "task_conf.h"


/**************************************************************************************************
 * @brief  : 任务初始化
 * @param  : None
 * @return : None
**************************************************************************************************/
void task_init(void)
{
    const task_cfg_t *task;
    static TaskHandle_t tid[ARRAY_SIZE(task_cfg_table)];

    /* 创建所有的任务 */
    for(uint8_t i = 0 ; i < ARRAY_SIZE(task_cfg_table) ; i++)
    {
        task = &task_cfg_table[i];
        xTaskCreate(task->entry, task->name, task->stack_size, NULL, task->priority, &tid[i]);
    }

    LOG_I("press [ Enter key ] into console.\n");

    /* 启动调度 */
    vTaskStartScheduler();

    /* 永远不会进入到这里 */
    for (;;)
    {
        APP_ERROR_HANDLER(NRF_ERROR_FORBIDDEN);
    }
}


/**************************************************************************************************
 * @brief  : 空闲任务回调
 * @param  : None
 * @return : None
**************************************************************************************************/
void vApplicationIdleHook( void )
{
    static TaskHandle_t tid;
#if NRF_LOG_ENABLED
    vTaskResume(tid);
#endif
}