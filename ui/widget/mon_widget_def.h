#ifndef __MON_WIDGET_DEF_H__
#define __MON_WIDGET_DEF_H__

#define MON_MAIN_LEVEL_STEP         4                                           /**< LEVEL步长 */
#define MON_MAIN_PROG_MAX           16                                          /**< 最大进度 */

typedef enum _mon_main_level_type {
    MON_MAIN_LEVEL0,
    MON_MAIN_LEVEL1,
    MON_MAIN_LEVEL2,
    MON_MAIN_LEVEL3,
    MON_MAIN_LEVEL_MAX,                                                         /**< 最大level */
    MON_MAIN_LEVEL_ADD,                                                         /**< 减去一个level */
    MON_MAIN_LEVEL_SUB,                                                         /**< 添加一个level */
}mon_main_level_type_t;

typedef enum _mon_widget_mode {
    MON_MODE_SCAN,                                                              /**# 扫描模式 */
    MON_MODE_LEVEL,                                                             /**# LEVEL模式 */
    MON_MODE_PROG,                                                              /**# 进度模式 */
}mon_widget_mode_t;

typedef enum _mon_widget_main_attr {
    MON_MAIN_ATTR_PROG,                                                         /**< 设置进度 */
    MON_MAIN_ATTR_LEVEL,                                                        /**< 设置LEVEL */
    MON_MAIN_ATTR_COLOR,                                                        /**< 设置主进度颜色 */
}mon_widget_main_attr_t;

typedef enum _mon_widget_sub_attr {
    MON_SUB_ATTR_ENABLE,                                                        /**< 是否隐藏 */
    MON_SUB_ATTR_PROG,                                                          /**< 子进度 */
}mon_widget_sub_attr_t;

#endif
