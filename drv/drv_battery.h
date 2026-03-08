#ifndef __DRV_BATTERY_H__
#define __DRV_BATTERY_H__
#include <stdint.h>

typedef enum _bat_state {
    BAT_STAT_UNCHARGE,                                                          /**@ 未充电 */
    BAT_STAT_CHARGING,                                                          /**@ 充电中 */
    BAT_STAT_FULLY,                                                             /**@ 充满电 */
}bat_state_t;

void bat_pow_init(void);                                                        /**# 电池供电初始化 */
bat_state_t get_bat_power_state(void);                                          /**# 电池状态初始化 */
void set_bat_power_state(uint8_t state);                                        /**# 设置电源状态 */



#endif