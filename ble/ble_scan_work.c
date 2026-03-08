#include "common.h"
#include "ble_common.h"
#include "topic.h"
#include "msgbus.h"

NRF_BLE_SCAN_DEF(m_scan);                                                           /**# 扫描实例 */

static ble_gap_scan_params_t m_scan_param =                 /**< Scan parameters requested for scanning and connection. */
{
    .active        = 0x01,
    .interval      = 10,  // NRF_BLE_SCAN_SCAN_INTERVAL,
    .window        = 10, // NRF_BLE_SCAN_SCAN_WINDOW,
    .filter_policy = BLE_GAP_SCAN_FP_ACCEPT_ALL,
    .timeout       = 0,//1000,   //NRF_BLE_SCAN_SCAN_DURATION  10ms  1000
    .scan_phys     = BLE_GAP_PHY_1MBPS,
    .extended      = true,
};

static uint8_t wd0[6] = {0x2b, 0xaf, 0xab, 0xd5, 0x3d, 0xc5};

/**************************************************************************************************
 * @brief  : 扫描事件回调
 * @param  : None
 * @return : None
**************************************************************************************************/
void ble_scan_evt_handler(scan_evt_t const * p_scan_evt)
{
    const uint8_t *addr = p_scan_evt->params.p_not_found->peer_addr.addr;
    int rssi = p_scan_evt->params.p_not_found->rssi;

    if(memcmp(addr, wd0, 6) == 0) {
        msgbus()->publish_topic("ble", &rssi, sizeof(rssi));
    }

}

/**************************************************************************************************
 * @brief  : ble扫描开始
 * @param  : None
 * @return : None
**************************************************************************************************/
void ble_scan_start(void)
{
    ret_code_t err_code;

    m_scan_param.timeout = 0;
    nrf_ble_scan_params_set(&m_scan, &m_scan_param);
    err_code = nrf_ble_scan_start(&m_scan);
    APP_ERROR_CHECK(err_code);
}

/**************************************************************************************************
 * @brief  : 扫描初始化
 * @param  : None
 * @return : None
**************************************************************************************************/
void ble_scan_init(void)
{
    ret_code_t          err_code;
    nrf_ble_scan_init_t init_scan;

    /* 设置扫描参数 */
    memset(&init_scan, 0, sizeof(init_scan));
    init_scan.p_scan_param = &m_scan_param;

    /* 扫描初始化 */
    err_code = nrf_ble_scan_init(&m_scan, &init_scan, ble_scan_evt_handler);
    APP_ERROR_CHECK(err_code);
}
