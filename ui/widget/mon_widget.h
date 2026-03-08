#ifndef __MON_WIDGET_H__
#define __MON_WIDGET_H__
#include "mon_widget_def.h"



/* 基础部分 */
void mon_widget_init(void);                                                     /**# 初始化 */
void mon_widget_disable(void);                                                  /**# 隐藏 */
void mon_widget_enable(void);                                                   /**# 显示 */
void mon_widget_set_mode(mon_widget_mode_t mode);                               /**# 设置模式 */

/* 主进度相关属性配置 */
void mon_widget_main_attr_set(mon_widget_main_attr_t attr, void *data);         /**# 设置属性 */
void mon_widget_main_attr_get(mon_widget_main_attr_t attr, void *data);         /**# 获取属性 */

/* 从进度相关属性配置 */
void mon_widget_sub_attr_set(mon_widget_sub_attr_t attr, void *data);

/* 其他属性相关配置 */
// void mon_widget_misc_attr_set

/* 主widget扫描 */
void mon_widget_main_scan(void);



#endif