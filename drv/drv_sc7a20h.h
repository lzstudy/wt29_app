#ifndef __DRV_SC7A20H_H__
#define __DRV_SC7A20H_H__

typedef void (*accel_cb_t)(void);                                                 /**# 回调 */
void sc7a20h_init(accel_cb_t cb);                                                 /**# 初始化 */

#endif