#include <string.h>
#include "common.h"
#include "nrf_delay.h"
#include "nrf_gpio.h"
#include "nrf_drv_spi.h"
#include "boards.h"
#include "app_error.h"
#include "drv_st7789.h"

/* 引脚相关 */     
enum _lcd_st7789_pin {
	PIN_RST    = NRF_GPIO_PIN_MAP(1, 00),
	PIN_DC     = NRF_GPIO_PIN_MAP(0, 22),
	PIN_CLK    = NRF_GPIO_PIN_MAP(0, 19),
	PIN_SDI    = NRF_GPIO_PIN_MAP(0, 20),
	PIN_CS     = NRF_GPIO_PIN_MAP(0, 17),
	PIN_POWER  = NRF_GPIO_PIN_MAP(0, 01),
	PIN_BL_PWM = NRF_GPIO_PIN_MAP(0, 04),
};

/* SPI实例 */
static const nrfx_spim_t spim = NRFX_SPIM_INSTANCE(3);


/**************************************************************************************************
 * @brief  : lcd gpio初始化
 * @param  : None
 * @return : None
**************************************************************************************************/
static void st7789_gpio_init(void) 
{
	/* 配置DC、RST、CS引脚为输出模式 */
	nrf_gpio_cfg_output(PIN_DC);
	nrf_gpio_cfg_output(PIN_RST);
	nrf_gpio_cfg_output(PIN_CS);

	/* 配置电源、背光 */
	nrf_gpio_cfg_output(PIN_POWER);
	nrf_gpio_cfg_output(PIN_BL_PWM);

	/* 使能CS引脚 */
	nrf_gpio_pin_clear(PIN_CS);
}

/**************************************************************************************************
 * @brief  : lcd spi初始化
 * @param  : None
 * @return : None
**************************************************************************************************/
static void st7789_spi_init(void) 
{
	nrfx_spim_config_t config = NRFX_SPIM_DEFAULT_CONFIG;
	config.mode      = NRF_SPIM_MODE_0;
	config.frequency = NRF_SPIM_FREQ_32M;
	config.mosi_pin  = PIN_SDI ;
	config.sck_pin   = PIN_CLK ;
	config.ss_pin    = PIN_CS ;
	APP_ERROR_CHECK(nrfx_spim_init(&spim, &config, NULL, NULL));
}

/**************************************************************************************************
 * @brief  : lcd 设备复位
 * @param  : None
 * @return : None
**************************************************************************************************/
static void st7789_dev_reset(void)
{
	/* 硬件复位 */
    nrf_gpio_pin_write(PIN_RST, 0);
    nrf_delay_ms(10);
    nrf_gpio_pin_write(PIN_RST, 1);
    nrf_delay_ms(120);
}

/**************************************************************************************************
 * @brief  : lcd st7789 写命令
 * @param  : None
 * @return : None
**************************************************************************************************/
void st7789_write_command(uint8_t cmd) 
{
    nrf_gpio_pin_write(PIN_DC, 0);
    nrfx_spim_xfer_desc_t desc = NRFX_SPIM_XFER_TX(&cmd, 1);
    nrfx_spim_xfer(&spim, &desc, 0);
}

/**************************************************************************************************
 * @brief  : lcd st7789 写数据
 * @param  : None
 * @return : None
**************************************************************************************************/
void st7789_write_data(uint8_t data)
{
    nrf_gpio_pin_write(PIN_DC, 1);
    nrfx_spim_xfer_desc_t desc = NRFX_SPIM_XFER_TX(&data, 1);
    nrfx_spim_xfer(&spim, &desc, 0);
}


/**************************************************************************************************
 * @brief  : lcd 设备初始化
 * @param  : None
 * @return : None
**************************************************************************************************/
static void st7789_reg_init(void)
{
	st7789_write_command(0x11);     
	nrf_delay_ms(120);                

	st7789_write_command(0x36);     
	st7789_write_data(0x00);

	st7789_write_command(0x3A);     
	st7789_write_data(0x05);   

	st7789_write_command(0xB2);     
	st7789_write_data(0x0B);   
	st7789_write_data(0x0B);   
	st7789_write_data(0x00);   
	st7789_write_data(0x33);   
	st7789_write_data(0x33);   

	st7789_write_command(0xB7);     
	st7789_write_data(0x75);   

	st7789_write_command(0xBB);     
	st7789_write_data(0x28);   

	st7789_write_command(0xC0);     
	st7789_write_data(0x2C);   

	st7789_write_command(0xC2);     
	st7789_write_data(0x01);   

	st7789_write_command(0xC3);     
	st7789_write_data(0x1F);    

	st7789_write_command(0xC6);     
	st7789_write_data(0x13);    

	st7789_write_command(0xD0);     
	st7789_write_data(0xA7);    

	st7789_write_command(0xD0);     
	st7789_write_data(0xA4);   
	st7789_write_data(0xA1);   

	st7789_write_command(0xD6);     
	st7789_write_data(0xA1);  

	st7789_write_command(0xE0);     
	st7789_write_data(0xF0);
	st7789_write_data(0x05);
	st7789_write_data(0x0A);
	st7789_write_data(0x06);
	st7789_write_data(0x06);
	st7789_write_data(0x03);
	st7789_write_data(0x2B);
	st7789_write_data(0x32);
	st7789_write_data(0x43);
	st7789_write_data(0x36);
	st7789_write_data(0x11);
	st7789_write_data(0x10);
	st7789_write_data(0x2B);
	st7789_write_data(0x32);

	st7789_write_command(0xE1);
	st7789_write_data(0xF0);
	st7789_write_data(0x08);
	st7789_write_data(0x0C);
	st7789_write_data(0x0B);
	st7789_write_data(0x09);
	st7789_write_data(0x24);
	st7789_write_data(0x2B);
	st7789_write_data(0x22);
	st7789_write_data(0x43);
	st7789_write_data(0x38);
	st7789_write_data(0x15);
	st7789_write_data(0x16);
	st7789_write_data(0x2F);
	st7789_write_data(0x37);
	st7789_write_command(0x21);     
	st7789_write_command(0x29);		// 开启显示
	st7789_write_command(0x2C);
	nrf_delay_ms(120);
}


/**************************************************************************************************
 * @brief  : lcd 初始化
 * @param  : None
 * @return : None
**************************************************************************************************/
void st7789_init(void)
{
	st7789_gpio_init();				// gpio初始化
	st7789_spi_init();				// spi初始化
	st7789_dev_reset();				// 设备复位
	st7789_reg_init();				// 写入初始寄存器序列
}

/**************************************************************************************************
 * @brief  : lcd st7789 设置显示区域(全屏)
 * @param  : None
 * @return : None
**************************************************************************************************/
void st7780_set_window(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2) 
{
	st7789_write_command(0x2A);
	st7789_write_data((x1 >> 8) & 0xFF);
	st7789_write_data((x1+0x23) & 0xFF);
	st7789_write_data((x2 >> 8) & 0xFF);
	st7789_write_data((x2+0x23) & 0xFF);

	st7789_write_command(0x2B);
	st7789_write_data((y1 >> 8) & 0xFF);
	st7789_write_data(y1 & 0xFF);
	st7789_write_data((y2 >> 8) & 0xFF);
	st7789_write_data(y2 & 0xFF);
	st7789_write_command(0x2C);
}

/**************************************************************************************************
 * @brief  : st7789矩形填充
 * @param  : None
 * @return : None
**************************************************************************************************/
void st7789_fill_area_screen(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t* color)
{
	uint8_t lo, hi;
	uint16_t row, column;
	uint8_t buf[340 * 80] = (0);

	/* 设置要填充的区域 */
	st7780_set_window(x1, y1, x2, y2);
	st7789_write_command(0x2C);
	nrf_gpio_pin_write(PIN_DC, 1);

	/* 填充像素 */
	row = (x2 - x1 + 1);
	column =(y2 - y1 + 1);
	
	for(int y = 0, j = 0 ; y < column ; y++) 
	{
		for(int i = 0 ; i < row ; i++)
		{
			buf[j] = (uint8_t)((color[y * row + i] >> 8) & 0xFF);
			buf[j + 1] = (uint8_t)(color[y * row + i] & 0xFF);
			j += 2;
		}
	}

	nrfx_spim_xfer_desc_t desc = NRFX_SPIM_XFER_TX(buf, row * 2 * column);
	nrfx_spim_xfer(&spim, &desc, 0);
}

/**************************************************************************************************
 * @brief  : 背光控制
 * @param  : None
 * @return : None
**************************************************************************************************/
void st7789_bl_ctrl(uint8_t state)
{
	nrf_gpio_pin_write(PIN_BL_PWM, state);
}

/**************************************************************************************************
 * @brief  : 设置LCD电源状态
 * @param  : None
 * @return : None
**************************************************************************************************/
void set_lcd_power_state(uint8_t state)
{
	nrf_gpio_pin_write(PIN_POWER, !state);
}
