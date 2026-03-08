#ifndef __DRV_FLASH_H__
#define __DRV_FLASH_H__
#include <stdint.h>

void flash_earse_page(uint32_t addr);
void flash_write(uint32_t addr, void *data, uint16_t size);
void flash_read(uint32_t addr, void *data, uint16_t size);

void save_usr_data(void *addr, uint16_t size);
void read_usr_data(void *addr, uint16_t size);

#endif