#include <stdio.h>
#include "system.h"
#include "app_drv.h"
#include "nrf_delay.h"

/**************************************************************************************************
 * @brief  : 主函数入库
 * @param  : None
 * @return : None
**************************************************************************************************/
int main(int argc, char const *argv[])
{
    system_init();
		
    while(1)
    {
        nrf_delay_ms(500);
    }
    return 0;
}
