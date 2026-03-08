#include "nrfx_saadc.h"
#include "app_error.h"
#include "common.h"
#include "drv_analog_device.h"

nrf_saadc_channel_config_t ch0_cfg = NRFX_SAADC_DEFAULT_CHANNEL_CONFIG_SE(NRF_SAADC_INPUT_AIN0);
nrf_saadc_channel_config_t ch1_cfg = NRFX_SAADC_DEFAULT_CHANNEL_CONFIG_SE(NRF_SAADC_INPUT_AIN5);
nrf_saadc_channel_config_t ch2_cfg = NRFX_SAADC_DEFAULT_CHANNEL_CONFIG_SE(NRF_SAADC_INPUT_AIN7);


#include "nrf_gpio.h"
/**************************************************************************************************
 * @brief  : adc采集句柄
 * @param  : None
 * @return : None
**************************************************************************************************/
static void saadc_event_handler(nrfx_saadc_evt_t const * p_event)
{
    LOG_I("saadc event handler");
}

/**************************************************************************************************
 * @brief  : ms2351初始化：红外、电池、检波器
 * @param  : None
 * @return : None
**************************************************************************************************/
void analog_device_init(void)
{
    ret_code_t err_code;

	/* adc初始化 */
    nrfx_saadc_config_t config = NRFX_SAADC_DEFAULT_CONFIG;
    config.oversample = NRF_SAADC_OVERSAMPLE_DISABLED;
    config.resolution = NRF_SAADC_RESOLUTION_12BIT;
    nrfx_saadc_init(&config, saadc_event_handler);

    /* 红外配置 */
    ch0_cfg.gain = NRF_SAADC_GAIN1_6;
    ch0_cfg.pin_p = NRF_SAADC_INPUT_AIN0;                   // 模拟输入引脚0
    ch0_cfg.reference = NRF_SAADC_REFERENCE_INTERNAL;       // 0.6V参考
    err_code = nrfx_saadc_channel_init(0, &ch0_cfg);
    APP_ERROR_CHECK(err_code);

    /* 电池电量检测 */
	ch1_cfg.gain = NRF_SAADC_GAIN1_6;
	ch1_cfg.pin_p = NRF_SAADC_INPUT_AIN5;                   // 电量检测
	ch1_cfg.reference = NRF_SAADC_REFERENCE_INTERNAL;       // 0.6V参考
	err_code = nrfx_saadc_channel_init(1, &ch1_cfg); 
	APP_ERROR_CHECK(err_code);

    /* 检波器配置 */
	ch2_cfg.gain = NRF_SAADC_GAIN1_6;
	ch2_cfg.pin_p = NRF_SAADC_INPUT_AIN7;                   // 检波器检测
	ch2_cfg.reference = NRF_SAADC_REFERENCE_INTERNAL;       // 0.6V参考
    ch2_cfg.resistor_p = NRF_SAADC_RESISTOR_PULLDOWN;
    ch2_cfg.resistor_n = NRF_SAADC_RESISTOR_PULLDOWN;
	err_code = nrfx_saadc_channel_init(2, &ch2_cfg); 
	APP_ERROR_CHECK(err_code);
}

/**************************************************************************************************
 * @brief  : 获取模拟设备值
 * @param  : None
 * @return : None
**************************************************************************************************/
int16_t get_analog_device_value(ana_device_type_t type)
{
    uint8_t channel;
    ret_code_t err_code;
    nrf_saadc_value_t adc_value;

    switch(type)
    {
    case ANA_DEV_IR:
        channel = 0;
        break;

    case ANA_DEV_BAT:
        channel = 1;
        break;

    case ANA_DEV_RECTIFIER:
        channel = 2;
        break;
    }

    err_code = nrfx_saadc_sample_convert(channel, &adc_value);
    if(err_code == NRF_SUCCESS)
        return adc_value;

    return -1;
}

#if 0
void lsh_cmd_tmp(int argc, char *argv[])
{
    int i = 0;
    int16_t val[200];

    for(;;)
    {
        val[i] = get_analog_device_value(ANA_DEV_RECTIFIER);
        i++;

        if(i == 100)
        {
            for(int j = 0 ; j < 100 ; j++)
            {
                printf("%d ", val[j]);
            }
            printf("\n");
            i = 0;
            nrf_delay_ms(1000);

        } 

        // vol = (float)value / (float)4095;
    
        // vol = vol * 3.6;

        // LOG_I("value = %d, vol = %fV", value, vol);
        // nrf_delay_ms(500);
    }

}
#endif