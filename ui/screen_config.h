#ifndef __SCREEN_CONFIG_H__
#define __SCREEN_CONFIG_H__
#include "screen_type.h"


IMPORT_SCREEN(screen_main);
IMPORT_SCREEN(screen_detect_camera);
IMPORT_SCREEN(screen_detect_ir);
IMPORT_SCREEN(screen_detect_wless);
IMPORT_SCREEN(screen_detect_location);
IMPORT_SCREEN(screen_detect_magentic);
IMPORT_SCREEN(screen_shake_alarm);
IMPORT_SCREEN(screen_setting);

/* 屏幕注册 */
static gui_screen_config_t gui_screen_config_table[] = {
    {&screen_main},
    {&screen_detect_camera},
    {&screen_detect_ir},
    {&screen_detect_wless},
    {&screen_detect_location},
    {&screen_detect_magentic},
    {&screen_shake_alarm},
    {&screen_setting},
};


#endif