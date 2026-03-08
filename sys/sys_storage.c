#include <stdlib.h>
#include <string.h>
#include "nrf_fstorage.h"
#include "nrf_fstorage_sd.h"
#include "sys_storage.h"
#include "app_error.h"
#include "nrf_delay.h"
#include "common.h"


#define ADDR_FSTORAGE_START         (0x000F8000)                    // (0x000F8000)
#define ADDR_FSTORAGE_SN_START      (0x000F9000)                    // SN起始地址
#define ADDR_FSTORAGE_END           (0x000FA000 - 1)                // (0x000F9999)
#define MAGIC_SYS_INFO              0x5A


/**************************************************************************************************
 * @brief  : fstorrage 事件 handler
 * @param  : None
 * @return : None
**************************************************************************************************/
static void fstorage_evt_handler(nrf_fstorage_evt_t * p_evt)
{
    LOG_I("%s", __func__);
}

NRF_FSTORAGE_DEF(nrf_fstorage_t fstorage) = {
    .evt_handler = fstorage_evt_handler,
    .start_addr = ADDR_FSTORAGE_START,
    .end_addr = ADDR_FSTORAGE_END,
};

/**************************************************************************************************
 * @brief  : 系统设置初始化
 * @param  : None
 * @return : None
**************************************************************************************************/
static void sys_setting_first_init(system_setting_info_t *ssi)
{
    memset(ssi, 0, sizeof(*ssi));
    ssi->magic = MAGIC_SYS_INFO;
    ssi->flag_volume_key = 1;
    ssi->flag_volume_alarm = 1;
    ssi->flag_volume_alarm_type = 0;
    ssi->flag_shake = 1;

    nrf_delay_ms(10);

    sys_fstorage_set(ssi);
}

/**************************************************************************************************
 * @brief  : 初始化fstorage
 * @param  : None
 * @return : None
**************************************************************************************************/
void sys_fstorage_init(system_setting_info_t *ssi)
{
	ret_code_t ret;
    nrf_fstorage_api_t * p_fs_api = &nrf_fstorage_sd;

    /* 初始化fstorage */
    ret = nrf_fstorage_init(&fstorage, p_fs_api, NULL);
    APP_ERROR_CHECK(ret);

    /* 读取配置 */
    sys_fstorage_get(ssi);

    /* 如果之前没有写入过, 那么写入新数据 */
    if(ssi->magic != MAGIC_SYS_INFO)
        sys_setting_first_init(ssi);
}

/**************************************************************************************************
 * @brief  : 擦除系统信息
 * @param  : None
 * @return : None
**************************************************************************************************/
void sys_fstorage_earse(void)
{
    ret_code_t ret;

    /* 擦除地址 */
    ret = nrf_fstorage_erase(&fstorage, ADDR_FSTORAGE_START, 1, NULL);
    APP_ERROR_CHECK(ret);
}

/**************************************************************************************************
 * @brief  : 设置系统信息
 * @param  : None
 * @return : None
**************************************************************************************************/
void sys_fstorage_set(system_setting_info_t *ssi)
{
    ret_code_t ret;

    /* 擦除地址 */
    ret = nrf_fstorage_erase(&fstorage, ADDR_FSTORAGE_START, 1, NULL);
    APP_ERROR_CHECK(ret);

    /* 等待擦完 */
    nrf_delay_ms(10);

    /* 写入数据 */
	ret  = nrf_fstorage_write(&fstorage, ADDR_FSTORAGE_START, ssi, sizeof(*ssi), NULL);
	APP_ERROR_CHECK(ret);

    /* 等待擦完 */
    nrf_delay_ms(10);
}

/**************************************************************************************************
 * @brief  : 获取系统信息
 * @param  : None
 * @return : None
**************************************************************************************************/
void sys_fstorage_get(system_setting_info_t *info)
{
    memset(info, 0, sizeof(*info));
    nrf_fstorage_read(&fstorage, ADDR_FSTORAGE_START, info, sizeof(*info));
}

/**************************************************************************************************
 * @brief  : 擦除SN
 * @param  : None
 * @return : None
**************************************************************************************************/
void sys_fstorage_sn_earse(void)
{
    ret_code_t ret;

    /* 擦除地址 */
    ret = nrf_fstorage_erase(&fstorage, ADDR_FSTORAGE_SN_START, 1, NULL);
    APP_ERROR_CHECK(ret);

    /* 等待擦完 */
    nrf_delay_ms(10);
}

/**************************************************************************************************
 * @brief  : 设置SN
 * @param  : None
 * @return : None
**************************************************************************************************/
void sys_fstorage_sn_set(uint64_t sn)
{
    ret_code_t ret;

    /* 擦除地址 */
    ret = nrf_fstorage_erase(&fstorage, ADDR_FSTORAGE_SN_START, 1, NULL);
    APP_ERROR_CHECK(ret);

    /* 等待擦完 */
    nrf_delay_ms(10);

    /* 写入数据 */
	ret  = nrf_fstorage_write(&fstorage, ADDR_FSTORAGE_SN_START, &sn, sizeof(sn), NULL);
	APP_ERROR_CHECK(ret);

    /* 等待擦完 */
    nrf_delay_ms(10);
}

/**************************************************************************************************
 * @brief  : 获取SN
 * @param  : None
 * @return : None
**************************************************************************************************/
uint64_t sys_fstorage_sn_get(void)
{
	uint64_t sn;
    nrf_fstorage_read(&fstorage, ADDR_FSTORAGE_SN_START, &sn, sizeof(sn));
	return sn;
}



/*===================================== 调试接口 ========================================*/
static void print_sys_setting(system_setting_info_t *ssi)
{
    LOG_I("========================================");
    LOG_I("magic      = %x", ssi->magic);
    LOG_I("key        = %d", ssi->flag_volume_key);
    LOG_I("alarm      = %d", ssi->flag_volume_alarm);
    LOG_I("alarm type = %d", ssi->flag_volume_alarm_type);
    LOG_I("shake      = %d", ssi->flag_shake);
    LOG_I("test       = %d", ssi->test);
    LOG_I("========================================\n");
}

void lsh_dbg_sys_setting(int argc, char *argv[])
{
    uint64_t sn;
    system_setting_info_t ssi = {0};
    int index = atoi(argv[1]);

    switch(index)
    {
    case 1:
        sys_fstorage_get(&ssi);
        print_sys_setting(&ssi);
        break;

    case 2:
        sys_fstorage_get(&ssi);
        ssi.test = atoi(argv[2]);
        sys_fstorage_set(&ssi);
        break;

    case 3:
        sys_fstorage_earse();
        LOG_I("test erase !");
        break;

    case 4:
        sys_fstorage_sn_earse();
        break;

    case 5:
        sn = sys_fstorage_sn_get();
        LOG_I("SN = %llx", sn);
        break;

    case 6:
        sn = atoi(argv[3]);
        sys_fstorage_sn_set(sn);
        break;
    }

    // mm -r 0xF8000 0x100
}
