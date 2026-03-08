#ifndef __SCREEN_UTILS_H__
#define __SCREEN_UTILS_H__
#include <stdint.h>
#include "lvgl.h"

#define FILTER_SIZE           10                                                /**< 滤波器大小 */

typedef struct _sliding_filter {
    int buf[FILTER_SIZE];                                                       /**@ 滤波大小 */
    uint8_t index;                                                              /**@ 当前位置 */
    uint8_t flag_full;                                                          /**@ 是否填满 */
}sliding_filter_t;

bool check_cycle(uint32_t *time, uint32_t cycle);
int slider_filter_process(sliding_filter_t *filter, int value);
void lv_obj_set_visual(lv_obj_t *obj, bool stat);


#endif