#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "app_uart.h"
#include "app_error.h"
#include "nrf_delay.h"
#include "nrf.h"
#include "bsp.h"
#if defined (UART_PRESENT)
#include "nrf_uart.h"
#endif
#if defined (UARTE_PRESENT)
#include "nrf_uarte.h"
#endif

/* 串口引脚配置 */
#define RX_PIN_NUMBER           NRF_GPIO_PIN_MAP(1, 04)     // 8
#define TX_PIN_NUMBER           NRF_GPIO_PIN_MAP(1, 02)     // 6
#define CTS_PIN_NUMBER          7
#define RTS_PIN_NUMBER          5

/* 缓冲配置 */
#define MAX_TEST_DATA_BYTES     (15U)                       /**< max number of test bytes to be used for tx and rx. */
#define UART_TX_BUF_SIZE        1024                        /**< UART TX buffer size. */
#define UART_RX_BUF_SIZE        64                          /**< UART RX buffer size. */

#define UART_HWFC               APP_UART_FLOW_CONTROL_DISABLED

extern void uart_recv_isr(uint8_t ch);

/**************************************************************************************************
 * @brief  : 串口出错句柄
 * @param  : None
 * @return : None
**************************************************************************************************/
void uart_error_handle(app_uart_evt_t * p_event)
{
    uint8_t ch;

    if (p_event->evt_type == APP_UART_COMMUNICATION_ERROR)
    {
        APP_ERROR_HANDLER(p_event->data.error_communication);
    }
    else if (p_event->evt_type == APP_UART_FIFO_ERROR)
    {
        APP_ERROR_HANDLER(p_event->data.error_code);
    }
    else if (p_event->evt_type == APP_UART_DATA_READY)
    {
		/* 从FIFO中读取数据  并回显 */
        app_uart_get(&ch);
        uart_recv_isr(ch);
	    // printf("%c",ch);
    }
}


/**************************************************************************************************
 * @brief  : 调试串口初始化
 * @param  : None
 * @return : None
**************************************************************************************************/
void debug_uart_init(void)
{
    uint32_t err_code;

    /* 设置串口配置 */
    const app_uart_comm_params_t comm_params =
      {
          RX_PIN_NUMBER,
          TX_PIN_NUMBER,
          RTS_PIN_NUMBER,
          CTS_PIN_NUMBER,
          UART_HWFC,
          false,
          NRF_UARTE_BAUDRATE_115200
      };

    /* 串口初始化 */
    APP_UART_FIFO_INIT(&comm_params,
                         UART_RX_BUF_SIZE,
                         UART_TX_BUF_SIZE,
                         uart_error_handle,
                         APP_IRQ_PRIORITY_LOWEST,
                         err_code);

    APP_ERROR_CHECK(err_code);
}


