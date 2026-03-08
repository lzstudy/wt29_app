#ifndef __SYS_STORAGE_H__
#define __SYS_STORAGE_H__
#include <stdint.h>
#include "system.h"



void sys_fstorage_init(system_setting_info_t *ssi);
void sys_fstorage_earse(void);
void sys_fstorage_set(system_setting_info_t *ssi);
void sys_fstorage_get(system_setting_info_t *ssi);

void sys_fstorage_sn_set(uint64_t sn);
uint64_t sys_fstorage_sn_get(void);
void sys_fstorage_sn_erase(void);

#endif