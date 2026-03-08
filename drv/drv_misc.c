#include "nrf_gpio.h"
#include "drv_misc.h"
#include "common.h"

/* 红外 */
#define PIN_IR_2          NRF_GPIO_PIN_MAP(0, 24)
#define PIN_IR_3          NRF_GPIO_PIN_MAP(0, 15)

/* 电机 */
#define PIN_MOTOR		  NRF_GPIO_PIN_MAP(1, 13)

/**************************************************************************************************
 * @brief  : 电机初始化
 * @param  : None
 * @return : None
**************************************************************************************************/
void motor_init(void)
{
    nrf_gpio_cfg_output(PIN_MOTOR);
}

/**************************************************************************************************
 * @brief  : motor start
 * @param  : None
 * @return : None
**************************************************************************************************/
void motor_start(void)
{
    nrf_gpio_pin_write(PIN_MOTOR, 1);
}

/**************************************************************************************************
 * @brief  : 电机 停止
 * @param  : None
 * @return : None
**************************************************************************************************/
void motor_stop(void)
{
    nrf_gpio_pin_write(PIN_MOTOR, 0);
}


/**************************************************************************************************
 * @brief  : ir初始化
 * @param  : None
 * @return : None
**************************************************************************************************/
void ir_init(void)
{
    nrf_gpio_cfg_output(PIN_IR_2);
    nrf_gpio_cfg_output(PIN_IR_3);
}

/**************************************************************************************************
 * @brief  : 红外设置
 * @param  : None
 * @return : None
**************************************************************************************************/
void ir_set(uint8_t val)
{
    if(val ==0)
    {
        nrf_gpio_pin_write(PIN_IR_2, 0);
        nrf_gpio_pin_write(PIN_IR_3, 0);
    }
    else
    {
        nrf_gpio_pin_write(PIN_IR_2, 1);
        nrf_gpio_pin_write(PIN_IR_3, 1);
    }
}

/**************************************************************************************************
 * @brief  : ir翻转
 * @param  : None
 * @return : None
**************************************************************************************************/
void ir_troggle(void)
{
    static bool state = 0;

    state = !state;
    ir_set(state);
}


/**************************************************************************************************
 * @brief  : ir控制
 * @param  : None
 * @return : None
**************************************************************************************************/
void lsh_cmd_ir(int argc, char *argv[])
{
    unused(argc);
    unused(argv);
    
    uint8_t val = atoi(argv[1]);
	ir_set(val);
}

/**************************************************************************************************
 * @brief  : ir控制
 * @param  : None
 * @return : None
**************************************************************************************************/
void lsh_cmd_motor(int argc, char *argv[])
{
    unused(argc);
    unused(argv);

    int time = atoi(argv[1]);

    for(int i = 0 ; i < 10 ; i++)
    {
        motor_start();
        nrf_delay_ms(time);
        motor_stop();
        nrf_delay_ms(time);
    }
}