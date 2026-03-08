#ifndef __SCR_CORE_H__
#define __SCR_CORE_H__

/* 初始化操作 */
void gui_framework_init(void);

/* 切换屏幕操作 */
void toggle_screen(const char *name);                                           /**# 切换屏幕 */

/* 保存数据*/
void save_screen_cxt(void *cxt);                                                /**# 保存屏幕上下文 */

/* 获取屏幕刷新周期 */
uint8_t get_ui_refresh_time(void);
#endif