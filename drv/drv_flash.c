#include "common.h"
#include "nrf_nvmc.h"

#define ADDR_USR_DATA           0x000F9000

/**************************************************************************************************
 * @brief  : flash擦除一页
 * @param  : None
 * @return : None
**************************************************************************************************/
void flash_earse_page(uint32_t addr)
{
    nrf_nvmc_page_erase(addr);
}

/**************************************************************************************************
 * @brief  : flash写入
 * @param  : None
 * @return : None
**************************************************************************************************/
void flash_write(uint32_t addr, void *data, uint16_t size)
{
    nrf_nvmc_write_bytes(addr, data, size);

}

/**************************************************************************************************
 * @brief  : flash读取
 * @param  : None
 * @return : None
**************************************************************************************************/
void flash_read(uint32_t addr, void *data, uint16_t size)
{
    uint8_t *src = (uint8_t *)addr;
    memcpy(data, src, size);
}

/**************************************************************************************************
 * @brief  : 保存用户数据
 * @param  : None
 * @return : None
**************************************************************************************************/
void save_usr_data(void *src, uint16_t size)
{
    nrf_nvmc_page_erase(ADDR_USR_DATA);
    flash_write(ADDR_USR_DATA, src, size);
}

/**************************************************************************************************
 * @brief  : 读取用户数据
 * @param  : None
 * @return : None
**************************************************************************************************/
void read_usr_data(void *dst, uint16_t size)
{
    flash_read(ADDR_USR_DATA, dst, size);
}
