#include "common.h"
#include "drv_beep.h"

int beep_flag = 0;

/**************************************************************************************************
 * @brief  : 按键任务
 * @param  : None
 * @return : None
**************************************************************************************************/
void beep_task_entry(void *arg)
{
    while(1)
    {
        if(beep_flag == 1)
        {
            beep_start();
            nrf_delay_ms(10);
            beep_stop();
            beep_flag = 0;
        }

        // sleep(1);
    }
}