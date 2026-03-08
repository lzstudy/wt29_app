#include "common.h"
#include "ble_common.h"
#include "ble_conf.h"

/* GATT配置 */

/* 实例 - 属性 */
NRF_BLE_GATT_DEF(m_gatt);                                                           /**# 定义属性 */
BLE_ADVERTISING_DEF(m_advertising);                                                 /**# 广播实例 */

/* 实例 - 服务 */
NRF_BLE_QWR_DEF(m_qwr);                                                             /**# 队列 */
BLE_BAS_DEF(m_bas);                                                                 /**# 电池服务 */
BLE_NUS_C_DEF(m_ble_nus_c);                                                         /**# 蓝牙串口实例 */


/* 实例 - 连接句柄 */
static uint16_t m_conn_handle = BLE_CONN_HANDLE_INVALID;

/* 实例 - 连接参数 */


/* 广播内容 */
static ble_uuid_t m_adv_uuids[] =
{
    {BLE_UUID_DEVICE_INFORMATION_SERVICE, BLE_UUID_TYPE_BLE}
};

/* 实例 - GATT队列 */
NRF_BLE_GQ_DEF(m_ble_gatt_queue, NRF_SDH_BLE_CENTRAL_LINK_COUNT, NRF_BLE_GQ_QUEUE_SIZE);

extern void scan_evt_handler(scan_evt_t const * p_scan_evt);

/**************************************************************************************************
 * @brief  : queue write处理函数
 * @param  : None
 * @return : None
**************************************************************************************************/
static void nrf_qwr_error_handler(uint32_t nrf_error)
{
    APP_ERROR_HANDLER(nrf_error);
}

/**************************************************************************************************
 * @brief  : ble事件句柄
 * @param  : None
 * @return : None
**************************************************************************************************/
static void ble_evt_handler(ble_evt_t const * p_ble_evt, void * p_context)
{
    LOG_I("ble_evt_handler");
}

/**************************************************************************************************
 * @brief  : 广播事件回调
 * @param  : None
 * @return : None
**************************************************************************************************/
static void ble_adv_evt_handler(ble_adv_evt_t ble_adv_evt)
{
    LOG_I("ble_adv_evt_handler");
}

/**************************************************************************************************
 * @brief  : 数据库发现句柄
 * @param  : None
 * @return : None
**************************************************************************************************/
static void db_disc_handler(ble_db_discovery_evt_t * p_evt)
{
    ble_nus_c_on_db_disc_evt(&m_ble_nus_c, p_evt);
}

/**************************************************************************************************
 * @brief  : 启动广播
 * @param  : None
 * @return : None
**************************************************************************************************/
void advertising_start(void)
{
    ret_code_t err_code = ble_advertising_start(&m_advertising, BLE_ADV_MODE_FAST);
    APP_ERROR_CHECK(err_code);
}

/**************************************************************************************************
 * @brief  : 安管事件处理
 * @param  : None
 * @return : None
**************************************************************************************************/
static void pm_evt_handler(pm_evt_t const * p_evt)
{
    bool delete_bonds = false;


    pm_handler_on_pm_evt(p_evt);
    pm_handler_flash_clean(p_evt);

    if(p_evt->evt_id == PM_EVT_PEERS_DELETE_SUCCEEDED)
        advertising_start();
}

/**************************************************************************************************
 * @brief  : 蓝牙串口服务初始化
 * @param  : None
 * @return : None
**************************************************************************************************/
static void ble_nus_sevices_init(void)
{
    LOG_I("ble_nus_sevices_init");
}

/**************************************************************************************************
 * @brief  : 连接过程的事件
 * @param  : None
 * @return : None
**************************************************************************************************/
static void on_conn_params_evt(ble_conn_params_evt_t * p_evt)
{
    ret_code_t err_code;
    if (p_evt->evt_type == BLE_CONN_PARAMS_EVT_FAILED)
    {
        err_code = sd_ble_gap_disconnect(m_conn_handle, BLE_HCI_CONN_INTERVAL_UNACCEPTABLE);
        APP_ERROR_CHECK(err_code);
    }
}

/**************************************************************************************************
 * @brief  : 连接出错处理
 * @param  : None
 * @return : None
**************************************************************************************************/
static void conn_params_error_handler(uint32_t nrf_error)
{
    APP_ERROR_HANDLER(nrf_error);
}

/**************************************************************************************************
 * @brief  : ble协议栈初始化
 * @param  : None
 * @return : None
**************************************************************************************************/
static void ble_stack_init(void)
{
    uint32_t ram_start = 0;
    ret_code_t err_code;

    /* 使能soft device */
    err_code = nrf_sdh_enable_request();
    APP_ERROR_CHECK(err_code);

    /* 设置默认配置 */
    err_code = nrf_sdh_ble_default_cfg_set(APP_BLE_CONN_CFG_TAG, &ram_start);
    APP_ERROR_CHECK(err_code);

    /* 使能ble协议栈 */
    err_code = nrf_sdh_ble_enable(&ram_start);
    APP_ERROR_CHECK(err_code);

    /* 注册回调, 用于处理ble事件 */
    NRF_SDH_BLE_OBSERVER(m_ble_observer, APP_BLE_OBSERVER_PRIO, ble_evt_handler, NULL);
}

/**************************************************************************************************
 * @brief  : GAP层参数初始化
 * @param  : None
 * @return : None
**************************************************************************************************/
static void gap_params_init(void)
{
    ret_code_t              err_code;
    ble_gap_conn_params_t   gap_conn_params;
    ble_gap_conn_sec_mode_t sec_mode;

    /* 设置连接模式 */
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&sec_mode);
    err_code = sd_ble_gap_device_name_set(&sec_mode, (const uint8_t *)DEVICE_NAME, strlen(DEVICE_NAME));
    APP_ERROR_CHECK(err_code);

    /* 设置 */
    err_code = sd_ble_gap_appearance_set(BLE_APPEARANCE_HEART_RATE_SENSOR_HEART_RATE_BELT);
    APP_ERROR_CHECK(err_code);

    /* 设置连接参数 */
    memset(&gap_conn_params, 0, sizeof(gap_conn_params));
    gap_conn_params.min_conn_interval = MIN_CONN_INTERVAL;
    gap_conn_params.max_conn_interval = MAX_CONN_INTERVAL;
    gap_conn_params.slave_latency     = SLAVE_LATENCY;
    gap_conn_params.conn_sup_timeout  = CONN_SUP_TIMEOUT;
    err_code = sd_ble_gap_ppcp_set(&gap_conn_params);
    APP_ERROR_CHECK(err_code);
}

/**************************************************************************************************
 * @brief  : 属性协议初始化
 * @param  : None
 * @return : None
**************************************************************************************************/
static void gatt_init(void)
{
    ret_code_t err_code = nrf_ble_gatt_init(&m_gatt, NULL);
    APP_ERROR_CHECK(err_code);
}


/**************************************************************************************************
 * @brief  : 广播初始化
 * @param  : None
 * @return : None
**************************************************************************************************/
static void advertising_init(void)
{
    ret_code_t err_code;
    ble_advertising_init_t init;

    /* 设置广播参数 */
    memset(&init, 0, sizeof(init));
    init.advdata.name_type               = BLE_ADVDATA_FULL_NAME;
    init.advdata.include_appearance      = true;
    init.advdata.flags                   = BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE;
    init.advdata.uuids_complete.uuid_cnt = sizeof(m_adv_uuids) / sizeof(m_adv_uuids[0]);
    init.advdata.uuids_complete.p_uuids  = m_adv_uuids;
    init.config.ble_adv_fast_enabled     = true;
    init.config.ble_adv_fast_interval    = APP_ADV_INTERVAL;
    init.config.ble_adv_fast_timeout     = APP_ADV_DURATION;
    init.config.ble_adv_slow_enabled     = true;
    init.config.ble_adv_slow_interval    = APP_ADV_INTERVAL;
    init.config.ble_adv_slow_timeout     = 0;
    init.evt_handler                     = ble_adv_evt_handler;

    /* 广播初始化 */
    err_code = ble_advertising_init(&m_advertising, &init);
    APP_ERROR_CHECK(err_code);

    /* 广播连接tag设置 */
    ble_advertising_conn_cfg_tag_set(&m_advertising, APP_BLE_CONN_CFG_TAG);
}

/**************************************************************************************************
 * @brief  : 服务初始化
 * @param  : None
 * @return : None
**************************************************************************************************/
static void services_init(void)
{
    ret_code_t         err_code;
    ble_bas_init_t     bas_init;
    ble_dis_init_t     dis_init;
    nrf_ble_qwr_init_t qwr_init = {0};

    /* 初始化队列模块 */
    qwr_init.error_handler = nrf_qwr_error_handler;
    err_code = nrf_ble_qwr_init(&m_qwr, &qwr_init);
    APP_ERROR_CHECK(err_code);

    /* 初始化电池服务 */
    memset(&bas_init, 0, sizeof(bas_init));
    bas_init.bl_rd_sec            = SEC_OPEN;
    bas_init.bl_cccd_wr_sec       = SEC_OPEN;
    bas_init.bl_report_rd_sec     = SEC_OPEN;
    bas_init.evt_handler          = NULL;
    bas_init.support_notification = true;
    bas_init.p_report_ref         = NULL;
    bas_init.initial_batt_level   = 100;
    err_code = ble_bas_init(&m_bas, &bas_init);
    APP_ERROR_CHECK(err_code);

    /* 初始化设备信息服务 */
    memset(&dis_init, 0, sizeof(dis_init));
    dis_init.dis_char_rd_sec = SEC_OPEN;
    ble_srv_ascii_to_utf8(&dis_init.manufact_name_str, (char *)MANUFACTURER_NAME);
    ble_srv_ascii_to_utf8(&dis_init.fw_rev_str, FW_VERSION);
    ble_srv_ascii_to_utf8(&dis_init.hw_rev_str, HW_VERSION);
    ble_srv_ascii_to_utf8(&dis_init.sw_rev_str, SW_VERSION);
    err_code = ble_dis_init(&dis_init);
    APP_ERROR_CHECK(err_code);

    /* 蓝牙串口初始化 */
    ble_nus_sevices_init();
}

/**************************************************************************************************
 * @brief  : 连接参数初始化
 * @param  : None
 * @return : None
**************************************************************************************************/
static void conn_params_init(void)
{
    ret_code_t             err_code;
    ble_conn_params_init_t cp_init;

    /* 设置连接参数 */
    memset(&cp_init, 0, sizeof(cp_init));
    cp_init.p_conn_params                  = NULL;
    cp_init.first_conn_params_update_delay = FIRST_CONN_PARAMS_UPDATE_DELAY;
    cp_init.next_conn_params_update_delay  = NEXT_CONN_PARAMS_UPDATE_DELAY;
    cp_init.max_conn_params_update_count   = MAX_CONN_PARAMS_UPDATE_COUNT;
    cp_init.disconnect_on_fail             = false;
    cp_init.evt_handler                    = on_conn_params_evt;
    cp_init.error_handler                  = conn_params_error_handler;

    /* 连接参数初始化 */
    err_code = ble_conn_params_init(&cp_init);
    APP_ERROR_CHECK(err_code);
}

/**************************************************************************************************
 * @brief  : 加密配置
 * @param  : None
 * @return : None
**************************************************************************************************/
static void peer_manager_init(void)
{
    ble_gap_sec_params_t sec_param;
    ret_code_t           err_code;

    /* 安管初始化 */
    err_code = pm_init();
    APP_ERROR_CHECK(err_code);
    memset(&sec_param, 0, sizeof(ble_gap_sec_params_t));
    
    /* 设置绑定参数 */
    sec_param.bond           = SEC_PARAM_BOND;
    sec_param.mitm           = SEC_PARAM_MITM;
    sec_param.lesc           = SEC_PARAM_LESC;
    sec_param.keypress       = SEC_PARAM_KEYPRESS;
    sec_param.io_caps        = SEC_PARAM_IO_CAPABILITIES;
    sec_param.oob            = SEC_PARAM_OOB;
    sec_param.min_key_size   = SEC_PARAM_MIN_KEY_SIZE;
    sec_param.max_key_size   = SEC_PARAM_MAX_KEY_SIZE;
    sec_param.kdist_own.enc  = 1;
    sec_param.kdist_own.id   = 1;
    sec_param.kdist_peer.enc = 1;
    sec_param.kdist_peer.id  = 1;

    /* 设置安管参数 */
    err_code = pm_sec_params_set(&sec_param);
    APP_ERROR_CHECK(err_code);

    /* 注册安管模块 */
    err_code = pm_register(pm_evt_handler);
    APP_ERROR_CHECK(err_code);
}

/**************************************************************************************************
 * @brief  : 数据搜索模块 
 * @param  : None
 * @return : None
**************************************************************************************************/
static void db_discovery_init(void)
{
    ble_db_discovery_init_t db_disc_init;

    /* 设置数据库发现参数 */
    db_disc_init.evt_handler  = db_disc_handler;
    db_disc_init.p_gatt_queue = &m_ble_gatt_queue;

    /* 初始化db搜索功能 */
    ret_code_t ret = ble_db_discovery_init(&db_disc_init);
    APP_ERROR_CHECK(ret);
}

/**************************************************************************************************
 * @brief  : 蓝牙串口事件
 * @param  : None
 * @return : None
**************************************************************************************************/
static void ble_nus_c_evt_handler(ble_nus_c_t * p_ble_nus_c, ble_nus_c_evt_t const * p_ble_nus_evt)
{
    ret_code_t err_code;

    switch (p_ble_nus_evt->evt_type)
    {
    case BLE_NUS_C_EVT_DISCOVERY_COMPLETE:
        err_code = ble_nus_c_handles_assign(p_ble_nus_c, p_ble_nus_evt->conn_handle, &p_ble_nus_evt->handles);
        APP_ERROR_CHECK(err_code);
        err_code = ble_nus_c_tx_notif_enable(p_ble_nus_c);
        APP_ERROR_CHECK(err_code);
        err_code = ble_nus_c_ecg_tx_notif_enable(p_ble_nus_c);
        APP_ERROR_CHECK(err_code);
        err_code = ble_nus_c_mix_tx_notif_enable(p_ble_nus_c);
        APP_ERROR_CHECK(err_code);
        break;
    case BLE_NUS_C_EVT_NUS_TX_EVT:
        break;
    case BLE_NUS_C_EVT_NUS_ECG_TX_EVT:
        break;
    case BLE_NUS_C_EVT_NUS_MIX_TX_EVT:
        break;
    case BLE_NUS_C_EVT_DISCONNECTED:
        break;
    }
}

/**************************************************************************************************
 * @brief  : 蓝牙串口初始化 
 * @param  : None
 * @return : None
**************************************************************************************************/
static void nus_c_init(void)
{
    ret_code_t       err_code;
    ble_nus_c_init_t   init;

    /* 蓝牙串口初始化 */
    init.evt_handler = ble_nus_c_evt_handler;
    err_code = ble_nus_c_init(&m_ble_nus_c, &init);
    APP_ERROR_CHECK(err_code);
}



/**************************************************************************************************
 * @brief  : ble初始化
 * @param  : None
 * @return : None
**************************************************************************************************/
void ble_init(void)
{
    bool erase_bonds = 0;

    ble_stack_init();
    gap_params_init();
    gatt_init();
    advertising_init();
    services_init();                    // 服务初始化
    conn_params_init();                 // 连接参数配置
    peer_manager_init();                // 安全管理配置
    db_discovery_init();                // 数据库配置
    nus_c_init();                       // 蓝牙串口初始化
    ble_scan_init();                    // 扫描初始化

    /* ble 线程初始化 */
    nrf_sdh_freertos_init(NULL, &erase_bonds);
}

