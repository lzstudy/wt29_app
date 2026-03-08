#include "common.h"
#include "drv_beep.h"
#include "nrfx_pwm.h"
#include "nrf_drv_pwm.h"
#include "nrfx.h"
#include "app_error.h"
#include "nrf_gpio.h"
#include <stdint.h>
#include "nrf_delay.h"

#define PWM_DEV             3                                                   /**# 用的PWM 3 */
#define PIN_BEEP            NRF_GPIO_PIN_MAP(1, 15)                             /**# 蜂鸣器使用的引脚 */

/* 16M / 4000 = 4000 (4KHZ) */
#define PWM_TOP_VALUE       3999                                                /**# 4K频率 */

typedef struct _beep_device {
    uint8_t volume;                                                             /**@ 音量设置 */
    nrf_drv_pwm_t pwm;                                                          /**@ pwm控制器 */
    nrfx_pwm_config_t cfg;                                                      /**@ pwm配置 */
    nrf_pwm_sequence_t seq;                                                     /**@ 控制序列 */
    nrf_pwm_values_individual_t pvi;                                            /**@ 通道设置 */
}beep_device_t;

static beep_device_t beep_device;                                               /**# 蜂鸣器设备 */
static nrf_pwm_values_individual_t m_seq_values;                                /**# PWM值存储 */

/**************************************************************************************************
 * @brief  : pwm配置
 * @param  : None
 * @return : None
**************************************************************************************************/
static void pwm_config(beep_device_t *dev)
{
    nrfx_pwm_config_t *cfg = &dev->cfg;

    /* 设置PWM控制器 */
    dev->pwm.p_registers  = NRFX_CONCAT_2(NRF_PWM, PWM_DEV);
    dev->pwm.drv_inst_idx = NRFX_CONCAT_3(NRFX_PWM, PWM_DEV, _INST_IDX);

    /* 设置pwm通道, 默认有4路通道, 目前只用第一路 */
    cfg->output_pins[0] = PIN_BEEP;
    cfg->output_pins[1] = NRFX_PWM_PIN_NOT_USED;
    cfg->output_pins[2] = NRFX_PWM_PIN_NOT_USED;
    cfg->output_pins[3] = NRFX_PWM_PIN_NOT_USED;

    /* 硬件属性设置 */
    cfg->irq_priority = NRFX_PWM_DEFAULT_CONFIG_IRQ_PRIORITY,
    cfg->count_mode   = NRF_PWM_MODE_UP;
    cfg->load_mode    = NRF_PWM_LOAD_INDIVIDUAL,
    cfg->step_mode    = NRF_PWM_STEP_AUTO,

    /* 周期和占空比相关 */
    cfg->base_clock   = NRF_PWM_CLK_16MHz;               // 1M HZ时钟
    cfg->top_value    = PWM_TOP_VALUE;                  // 决定占空比

    /* pwm初始化 */
    nrfx_pwm_init(&dev->pwm, cfg, NULL);
}

/**************************************************************************************************
 * @brief  : pwm序列初始化
 * @param  : None
 * @return : None
**************************************************************************************************/
static void pwm_seq_init(beep_device_t *dev)
{
    nrf_pwm_sequence_t *seq = &dev->seq;

    seq->values.p_individual = &m_seq_values;
    seq->length = NRF_PWM_VALUES_LENGTH(m_seq_values);
    seq->repeats = 0;
    seq->end_delay = 0;
}

/**************************************************************************************************
 * @brief  : 设置音量, 此蜂鸣器的谐振频率为4K
 * @param  : None
 * @return : None
**************************************************************************************************/
uint8_t beep_get_volume(void)
{
    return beep_device.volume;
}

/**************************************************************************************************
 * @brief  : 设置音量, 此蜂鸣器的谐振频率为4K
 * @param  : None
 * @return : None
**************************************************************************************************/
void beep_set_volume(uint8_t volume)
{
    beep_device.volume = volume;
}

/**************************************************************************************************
 * @brief  : 蜂鸣器启动
 * @param  : None
 * @return : None
**************************************************************************************************/
void beep_start(void)
{
    uint16_t max_duty_cycle, duty_cycle;
    beep_device_t *dev = &beep_device;

    /* 计算占空比(决定音量), 50%的占空比最大音量 */
    max_duty_cycle = (PWM_TOP_VALUE * 50) / 100;
    duty_cycle = (max_duty_cycle * dev->volume) / 100;

    /* 更新占空比到对应通道  */
    m_seq_values.channel_0 = duty_cycle;

    /* 更新pwm */
    nrfx_pwm_complex_playback(&dev->pwm, &dev->seq, &dev->seq, 0, NRFX_PWM_FLAG_LOOP);
}

/**************************************************************************************************
 * @brief  : 蜂鸣器停止
 * @param  : None
 * @return : None
**************************************************************************************************/
void beep_stop(void)
{
    nrfx_pwm_stop(&beep_device.pwm, true);
}

/**************************************************************************************************
 * @brief  : 蜂鸣器初始化
 * @param  : None
 * @return : None
**************************************************************************************************/
void beep_init(void)
{
    pwm_config(&beep_device);
    pwm_seq_init(&beep_device);

    beep_device.volume = 28;
}

/**************************************************************************************************
 * @brief  : 单次play
 * @param  : None
 * @return : None
**************************************************************************************************/
void beep_single_play(uint8_t time)
{
    beep_start();
    nrf_delay_ms(time);
    beep_stop();
    nrf_delay_ms(time);
}



/**************************************************************************************************
 * @brief  : 注册变量
 * @param  : None
 * @return : None
**************************************************************************************************/
void lsh_dbg_volume(int argc, char const *argv[])
{
    uint8_t volume = atoi(argv[1]);

    if(argc == 1) {
        LOG_I("volume = %d", beep_device.volume);
        return;
    }

    beep_set_volume(volume);
}