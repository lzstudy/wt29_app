#ifndef __MON_WIDGET_TYPE_H__
#define __MON_WIDGET_TYPE_H__
#include <stdint.h>
#include "lvgl.h"
#include "mon_widget_def.h"

#define MON_MAIN_CNT            16                                              /**< 主mon数量 */
#define MON_SUB_CNT             14                                              /**< 子mon数量 */

/* 主main配置 */
#define MON_MAIN_POS_START      220                                             /**< 起始位置(由下向上) */
#define MON_MAIN_INTERVAL       11                                              /**< MON间隔 */
#define MON_MAIN_W              45                                              /**< 主MAIN宽度 */
#define MON_MAIN_H              5                                               /**< 主MAIN高度 */

/* 核心信息 */
typedef struct _mon_core {
    lv_style_t style;                                                           /**@ 风格 */
    mon_widget_mode_t mode;                                                     /**@ 工作模式 */
    uint16_t main_pos_end;                                                      /**@ main pos end */
    lv_obj_t *base[MON_MAIN_CNT];                                               /**@ BASE */
}mon_core_t;

/* 主进度 - level模式 */
typedef struct _mon_main_level {
    uint8_t level;                                                              /**@ level */
    lv_palette_t color;                                                         /**@ 颜色 */
    lv_obj_t *item[MON_MAIN_CNT];                                               /**@ 主ITEM */
}mon_main_level_t;

/* 主进度 - prog模式 */
typedef struct _mon_main_prog {
    uint8_t prog;                                                               /**@ 当前进度 */
    uint32_t top_prog_flash_time;                                               /**@ 当前时间 */
    uint32_t beep_cycle;                                                        /**@ beep周期 */
    uint32_t beep_time;                                                         /**@ beep时间 - 累计使用, 不是持续时间 */
    lv_obj_t *item[MON_MAIN_CNT];                                               /**@ 主ITEM */
    bool timer_enable;                                                          /**@ 定时器状态 */
}mon_main_prog_t;

/* 主进度 - 扫描模式 */
typedef struct _mon_main_scan {
    lv_obj_t *item;                                                             /**@ item */
    uint16_t pos;                                                               /**@ 当前位置 */
    uint16_t direct;                                                            /**@ 方向 */
    uint16_t time;                                                              /**@ 扫描周期 */
    lv_palette_t color;                                                         /**@ 颜色 */
}mon_main_scan_t;

/* 子进度 */
typedef struct _mon_sub_prog {
    uint8_t prog;
    lv_obj_t *sym_p;                                                            /**@ 正号 */
    lv_obj_t *sym_n;                                                            /**@ 负号 */
    lv_obj_t *base[MON_SUB_CNT];                                                /**@ BASE */
    lv_obj_t *item[MON_SUB_CNT];                                                /**@ 从ITEM */
}mon_sub_prog_t;

/* 其他内容 */
typedef struct _mon_misc {
    lv_obj_t *footer;                                                           /**@ 页脚 */
    lv_obj_t *home;                                                             /**@ home图标 */
}mon_misc_t;

/* mon widget 主题 */
typedef struct _mon_widget {
    lv_obj_t *parent;                                                           /**@ 背景 */
    mon_core_t core;                                                            /**@ 核心信息 */
    mon_main_prog_t mmp;                                                        /**@ 主进度 - prog模式 */
    mon_main_level_t mml;                                                       /**@ 主进度 - level模式 */
    mon_main_scan_t mms;                                                        /**@ 主进度 - scan模式 */
    mon_sub_prog_t msp;                                                         /**@ 从进度 */
    mon_misc_t misc;                                                            /**@ 其他部分 */
}mon_widget_t;

mon_widget_t *get_mon_widget(void);

#endif

