#ifndef __DRV_KEY_H__
#define __DRV_KEY_H__
#include "nrf_gpio.h"

typedef enum _key_type{
    KEY_UP   = NRF_GPIO_PIN_MAP(1, 11),                                         /**# 43' 上键 */
    KEY_DOWN = NRF_GPIO_PIN_MAP(1, 06),                                         /**# 38' 下键 */
    KEY_CTRL = NRF_GPIO_PIN_MAP(1, 12),                                         /**# 44' 控制键 */
}key_type_t;

typedef void (*key_cb_t)(void *arg);                                            /**# 按键回调函数 */

void key_init(void);                                                            /**# 按键初始化 */
int key_register(key_type_t type, key_cb_t cb, void *arg);                      /**# 注册按键 */


#endif

