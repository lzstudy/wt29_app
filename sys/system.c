#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#include "system.h"
#include "ckret.h"
#include "drv_beep.h"
#include "drv_sc7a20h.h"
#include "drv_misc.h"
#include "drv_analog_device.h"
#include "msgbus.h"
#include "msgbus_register_table.h"
#include "sys_storage.h"
#include "beep_manager.h"
#include "beep_key_manager.h"
#include "motor_manager.h"
#include "accel_manager.h"
#include "alarm_manager.h"
#include "timer_manager.h"
#include "magentic_manager.h"
#include "nrf_drv_gpiote.h"
#include "drv_qmc5883p.h"


extern void debug_uart_init(void);                                  /**# 调试串口初始化 */
extern void ble_init(void);                                         /**# BLE初始化 */

static system_info_t system_info;

/**************************************************************************************************
 * @brief  : 日志初始化
 * @param  : None
 * @return : None
**************************************************************************************************/
static void log_init(void)
{
    ret_code_t err_code = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(err_code);
    NRF_LOG_DEFAULT_BACKENDS_INIT();
}

/**************************************************************************************************
 * @brief  : 时钟初始化
 * @param  : None
 * @return : None
**************************************************************************************************/
static void clock_init(void)
{
    ret_code_t err_code = nrf_drv_clock_init();
    APP_ERROR_CHECK(err_code);
		
	/* 时钟配置 */
    nrf_drv_clock_hfclk_request(NULL);
    while (!nrf_drv_clock_hfclk_is_running());

    LOG_I("clock init success.");
}

/**************************************************************************************************
 * @brief  : 电源模式初始化
 * @param  : None
 * @return : None
**************************************************************************************************/
static void power_mode_init(void)
{
    SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;

    LOG_I("power init success.");
}

/**************************************************************************************************
 * @brief  : 系统设置初始化
 * @param  : None
 * @return : None
**************************************************************************************************/
static void sys_setting_init(void)
{
    system_setting_info_t *ssi = &system_info.setting;

    /* sys初始化 */
    sys_fstorage_init(ssi);

    /* 设置声音状态 */
    beep_key_manager()->enable(ssi->flag_volume_key);

    /* 电机manager使能 */
    motor_manager()->enable(ssi->flag_shake);

    /* 报警manager配置 */
    alarm_manager()->enable(ssi->flag_volume_alarm);
    alarm_manager()->set(ssi->flag_volume_alarm_type);

    /* 定时器manger初始化 */
    timer_manager()->init();
}

/**************************************************************************************************
 * @brief  : 设备基础初始化
 * @param  : None
 * @return : None
**************************************************************************************************/
static void device_base_init(void)
{
    ret_code_t err_code;    
    err_code = nrf_drv_gpiote_init();
    APP_ERROR_CHECK(err_code);
}

/**************************************************************************************************
 * @brief  : 设备初始化
 * @param  : None
 * @return : None
**************************************************************************************************/
static void device_init(void)
{
    device_base_init();
    ir_init();                  // 红外初始化
    analog_device_init();       // 模拟设备初始化：检波器、电池、红外
}

/**************************************************************************************************
 * @brief  : 这侧topic
 * @param  : None
 * @return : None
**************************************************************************************************/
static void manager_init(void)
{
    beep_manager()->init();     // beep初始化
    motor_manager()->init();    // 震动电机初始化
    accel_manager()->init();    // 加速计初始化
    magentic_manager()->init(); // 磁力计初始化
}

/**************************************************************************************************
 * @brief  : 这侧topic
 * @param  : None
 * @return : None
**************************************************************************************************/
static void msgbus_service_init(void)
{
    msgbus()->init();
    msgbus()->register_topic("misc");
    msgbus()->register_topic("ble");
    msgbus()->register_topic("accel");
    msgbus()->register_topic("timer");
}

/**************************************************************************************************
 * @brief  : 服务初始化
 * @param  : None
 * @return : None
**************************************************************************************************/
static void service_init(void)
{
    msgbus_service_init();
}

/**************************************************************************************************
 * @brief  : 系统初始化
 * @param  : None
 * @return : None
**************************************************************************************************/
void system_init(void)
{
    bat_pow_init();            // 设置电池电源
    log_init();                 // 日志初始化
    debug_uart_init();          // 调试串口初始化
	clock_init();               // 时钟初始化
    power_mode_init();          // 初始化电源模式
    device_init();              // 设备初始化
    manager_init();             // manager初始化
    ble_init();                 // BLE初始化
    service_init();             // 服务初始化
    sys_setting_init();         // 系统配置初始化
    task_init();                // 任务初始化
}

/**************************************************************************************************
 * @brief  : 系统初始化
 * @param  : None
 * @return : None
**************************************************************************************************/
system_setting_info_t *get_system_setting_info(void)
{
    return &system_info.setting;
}

/**************************************************************************************************
 * @brief  : 保存系统设置
 * @param  : None
 * @return : None
**************************************************************************************************/
void save_system_setting_info(void)
{
    sys_fstorage_set(&system_info.setting);
}

/**************************************************************************************************
 * @brief  : 擦除SN信息
 * @param  : None
 * @return : None
**************************************************************************************************/
void earse_sn_info(void)
{
    sys_fstorage_sn_earse();
}

/**************************************************************************************************
 * @brief  : 设置SN信息
 * @param  : None
 * @return : None
**************************************************************************************************/
void set_sn_info(uint64_t sn)
{
    sys_fstorage_sn_set(sn);
}

/**************************************************************************************************
 * @brief  : 获取SN信息
 * @param  : None
 * @return : None
**************************************************************************************************/
uint64_t get_sn_info(void)
{
    return sys_fstorage_sn_get();
}

extern void check_sn_info(void);
/**************************************************************************************************
 * @brief  : 校验SN
 * @param  : None
 * @return : None
**************************************************************************************************/
void check_sn(void)
{
    check_sn_info();
}

