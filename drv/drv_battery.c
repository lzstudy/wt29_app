#include "nrf_gpio.h"
#include "drv_battery.h"
#include "common.h"
#include "drv_analog_device.h"

#define PIN_BAT_CHARGE 				NRF_GPIO_PIN_MAP(0, 10)
#define PIN_BAT_STDBY				NRF_GPIO_PIN_MAP(0, 9)


#define MCU_USB_STA_				NRF_GPIO_PIN_MAP(1, 10)
#define BAT_POW						NRF_GPIO_PIN_MAP(0, 26)

/**************************************************************************************************
 * @brief  : 电池电源设置
 * @param  : None
 * @return : None
**************************************************************************************************/
void bat_pow_init(void)
{
	/* 电池供电配置为输出 */
	nrf_gpio_cfg_output(BAT_POW);

	/* 电源检测配置为输入 */
	nrf_gpio_cfg_input(PIN_BAT_CHARGE,  GPIO_PIN_CNF_PULL_Disabled);
	nrf_gpio_cfg_input(PIN_BAT_STDBY, GPIO_PIN_CNF_PULL_Disabled);

	/* 设置电池供电为高 */
	nrf_gpio_pin_write(BAT_POW, 1);
}

/**************************************************************************************************
 * @brief  : 电池电源设置
 * @param  : None
 * @return : None
**************************************************************************************************/
void set_bat_power_state(uint8_t state)
{
	nrf_gpio_pin_write(BAT_POW, state);
}

/**************************************************************************************************
 * @brief  : 电池电源设置
 * @param  : None
 * @return : None
**************************************************************************************************/
bat_state_t get_bat_state(void)
{
	int pin_chrg, pin_stdby;

	pin_chrg = nrf_gpio_pin_read(PIN_BAT_CHARGE);
	pin_stdby = nrf_gpio_pin_read(PIN_BAT_STDBY);

	/* 都为0表示没有充电 */
	if(pin_chrg == 0 && pin_stdby == 0)
		return BAT_STAT_UNCHARGE;

	if(pin_chrg == 1 && pin_stdby == 0)
		return BAT_STAT_CHARGING;

	/* 表示充满 */
	if(pin_chrg == 0 && pin_stdby == 1)
		return BAT_STAT_FULLY;

	return BAT_STAT_UNCHARGE;
}

/**************************************************************************************************
 * @brief  : 读取电池电量
 * @param  : None
 * @return : None
**************************************************************************************************/
void lsh_dbg_bat_stat(int argc, char *argv[])
{
	int power;
	int pin_chrg, pin_stdby;

	pin_chrg = nrf_gpio_pin_read(PIN_BAT_CHARGE);
	pin_stdby = nrf_gpio_pin_read(PIN_BAT_STDBY);
	power = get_analog_device_value(ANA_DEV_BAT);

	LOG_I("charge %d, stdby %d, power = %d", pin_chrg, pin_stdby, power);

	if(pin_chrg == 0 && pin_stdby == 0) {
		LOG_I("battery unchagre");
		return;
	}

	if(pin_chrg == 1 && pin_stdby == 0) {
		LOG_I("battery charging");
		return;
	}

	/* 表示充满 */
	if(pin_chrg == 0 && pin_stdby == 1) {
		LOG_I("battery fully");
		return;
	}
}
