#ifndef __SYSTEM_H__
#define __SYSTEM_H__
#include <stdint.h>

typedef enum _alarm_volume_type {
    ALARM_VOLUME_LOW,
    ALARM_VOLUME_HIGH,
}alarm_volume_type_t;

typedef struct _system_setting_info {
    uint8_t magic;                                                              /**@ 写入幻数 */
    uint8_t flag_volume_key        : 1;                                         /**@ 按键提示音 */
    uint8_t flag_volume_alarm      : 1;                                         /**@ 报警提示音 */
    uint8_t flag_volume_alarm_type : 1;                                         /**@ 报警类型, 0表示低音, 1表示高音 */
    uint8_t flag_shake             : 1;                                         /**@ 震动状态 */
    uint8_t test;
}__attribute__((aligned(4))) system_setting_info_t;

typedef struct _system_info {
    system_setting_info_t setting;                                              /**# 系统设置 */
}system_info_t;

void system_init(void);                                                         /**# 系统初始化 */
system_setting_info_t *get_system_setting_info(void);                           /**# 获取系统设置信息 */
void save_system_setting_info(void);                                            /**# 保存系统设置 */

void set_sn_info(uint64_t sn);
uint64_t get_sn_info(void);
void earse_sn_info(void);
void check_sn(void);

#endif
