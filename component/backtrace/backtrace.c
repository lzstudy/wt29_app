#include <stdint.h>
#include <stdio.h>
#include "nrfx_uarte.h"
#include "app_error.h"
#include "nrf.h"
#include "bsp.h"
#include "nrf_drv_uart.h"

static nrfx_uarte_t uart = NRFX_UARTE_INSTANCE(1);

/**************************************************************************************************
 * @brief  : 
 * @param  : None
 * @return : None
**************************************************************************************************/
static void execption_uart_init(void)
{
    ret_code_t result = 0;
    nrfx_uarte_config_t uarte_config = NRFX_UARTE_DEFAULT_CONFIG;

    uarte_config.pseltxd = NRF_GPIO_PIN_MAP(1, 04);
    uarte_config.pselrxd = NRF_GPIO_PIN_MAP(1, 02);
    uarte_config.baudrate = NRF_UARTE_BAUDRATE_115200;
    uarte_config.hwfc = NRF_UARTE_HWFC_DISABLED;
    uarte_config.parity = NRF_UARTE_PARITY_EXCLUDED;
    

    result = nrfx_uarte_init(&uart, &uarte_config, NULL);
    printf("result = %d\n", result);
}

/**************************************************************************************************
 * @brief  : uart初始化
 * @param  : None
 * @return : None
**************************************************************************************************/
static void uart_reinit(void)
{
    nrfx_uarte_uninit(&uart);           // 去初始化
    execption_uart_init();
    nrfx_uarte_tx(&uart, "hello\n", 6);
}

/**************************************************************************************************
 * @brief  : 主函数入库
 * @param  : None
 * @return : None
**************************************************************************************************/
void hardfault_handler(uint32_t *stack_address)
{
#if 0
    uart_reinit();
#endif
}

