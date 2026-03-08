#ifndef __DRV_BEEP_H__
#define __DRV_BEEP_H__
#include <stdint.h>

void beep_init(void);                                                           /**# 蜂鳴器初始化 */
void beep_start(void);
void beep_stop(void);
uint8_t beep_get_volume(void);                                                  /**# 获取当前音量 */
void beep_set_volume(uint8_t volume);                                           /**# 设置声音 */
void beep_single_play(uint8_t time);                                            /**# 单次play */
#endif

