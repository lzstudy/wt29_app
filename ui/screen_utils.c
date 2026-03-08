#include <stdbool.h>
#include "screen_utils.h"
#include "screen_core.h"

/**************************************************************************************************
 * @brief  : 周期检测
 * @param  : 计数时间 : 时间 = time * 刷新周期 (单位ms)
 * @param  : 超时时间 : 超过多少触发复位, 单位ms
 * @return : None
**************************************************************************************************/
bool check_cycle(uint32_t *time, uint32_t cycle)
{
	*time = *time + 1;
    if(*time >= cycle / get_ui_refresh_time()) {
        *time = 0;
        return true;
    }

    return false;
}

/**************************************************************************************************
 * @brief  : 平滑滤波处理
 * @param  : None
 * @return : None
**************************************************************************************************/
int slider_filter_process(sliding_filter_t *filter, int rssi)
{
    return rssi;
#if 0
    int min, max, tmp;
    int sum = 0;

    if(filter->index == 0) {
        return rssi;
    }

    /* 更新缓冲区 */
    filter->buf[filter->index] = rssi;
    filter->index = (filter->index + 1) % FILTER_SIZE;

    /* 如果缓冲区尚未填满，则标记并直接计算当前平均值 */
    if(!filter->flag_full && filter->index == 0) {
        filter->flag_full = 1;
    }
    if(!filter->flag_full) 
    {
        sum = 0;
        for(int i = 0; i < filter->index; i++) 
        {
            sum += filter->buf[i];
        }
        
        return (int)(sum / filter->index);
    }

    /* 2. 查找最大值和最小值，并计算总和 */
    min = max = filter->buf[0];
    sum = filter->buf[0];

    for(int i = 1; i < FILTER_SIZE; i++) {
        tmp = filter->buf[i];
        sum += tmp;

        if(tmp < min) {
            min = tmp;
        } else if(tmp > max) {
            max = tmp;
        }
    }

    /* 3. 剔除最大值和最小值后求平均 */
    sum = sum - min - max;
    if(sum == 2)
        sum++;

    return (int)(sum / (FILTER_SIZE - 2));
#endif
}

/**************************************************************************************************
 * @brief  : 是否设置隐藏(1可见， 0隐藏)
 * @param  : None
 * @return : None
**************************************************************************************************/
void lv_obj_set_visual(lv_obj_t *obj, bool stat)
{
    if(stat)
        lv_obj_clear_flag(obj, LV_OBJ_FLAG_HIDDEN);
    else
        lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
}