#include <nrf_drv_twi.h>
#include <nrf_delay.h>
#include <nrf_gpio.h>
#include <nrf_drv_gpiote.h>
#include "common.h"
#include "drv_sc7a20h.h"
#include "app_error.h"
#include "lsh.h"


#define SC7A20H_ADDR            0x18                                            /**< I2C地址 */
#define IIC_SCL	                NRF_GPIO_PIN_MAP(1, 9)
#define IIC_SDA	                NRF_GPIO_PIN_MAP(0, 8)
#define PIN_ISR                 NRF_GPIO_PIN_MAP(0, 6)

enum _sc7a20h_reg {
    REG_WHO_AM_I        = 0x0F,                                                     /**< 默认0x11 */
    REG_CTRL_REG0       = 0x1F,
    REG_CTRL_REG1       = 0x20,
    REG_CTRL_REG2       = 0x21,
    REG_CTRL_REG3       = 0x22,
    REG_CTRL_REG4       = 0x23,
    REG_CTRL_REG5       = 0x24,
    REG_CTRL_REG6       = 0x25,
    REG_DRDY_STATUS_REG = 0x27,
    REG_OUT_X_L         = 0x28,
    REG_OUT_X_H         = 0x29,
    REG_OUT_Y_L         = 0x2A,
    REG_OUT_Y_H         = 0x2B,
    REG_OUT_Z_L         = 0x2C,
    REG_OUT_Z_H         = 0x2D,
    REG_AOI1_CFG        = 0x30,
    REG_AOI1_SRC        = 0x31,
    REG_AOI1_THS        = 0x32,
    REG_AOI1_DURATION   = 0x33,
    REG_AOI2_CFG        = 0x34,
    REG_AOI2_SRC        = 0x35,
    REG_AOI2_THS        = 0x36,
    REG_AOI2_DURATION   = 0x37,
};

static nrf_drv_twi_t twi = NRF_DRV_TWI_INSTANCE(0);                             /**# twi实例 */
static accel_cb_t accel_callback;

/**************************************************************************************************
 * @brief  : 中断处理函数
 * @param  : None
 * @return : None
**************************************************************************************************/
static void sc7a20h_isr_handler(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
    accel_callback();
}

/**************************************************************************************************
 * @brief  : 写寄存器
 * @param  : None
 * @return : None
**************************************************************************************************/
static void sc7a20h_write_reg(uint8_t reg, uint8_t value)
{
    uint8_t data[2];

    data[0] = reg;
    data[1] = value;

    nrf_drv_twi_tx(&twi, SC7A20H_ADDR, data, 2, false);
}

/**************************************************************************************************
 * @brief  : 读取寄存器
 * @param  : None
 * @return : None
**************************************************************************************************/
static uint8_t sc7a20h_read_reg(uint8_t reg)
{
    uint8_t value;

    nrf_drv_twi_tx(&twi, SC7A20H_ADDR, &reg, 1, true);
    nrf_drv_twi_rx(&twi, SC7A20H_ADDR, &value, 1);

    return value;
}


/**************************************************************************************************
 * @brief  : i2c初始化
 * @param  : None
 * @return : None
**************************************************************************************************/
static void i2c0_init(void)
{
    nrf_drv_twi_config_t config;

    /* i2c配置 */
    config.scl = IIC_SCL;
    config.sda = IIC_SDA;
    config.frequency = NRF_DRV_TWI_FREQ_100K;
    config.interrupt_priority = APP_IRQ_PRIORITY_LOWEST;
    
    /* i2c初始化 */
    nrf_drv_twi_init(&twi, &config, NULL, NULL);
    nrf_drv_twi_enable(&twi); 
}


/**************************************************************************************************
 * @brief  : sc7a20 reg初始化
 * @param  : None
 * @return : None
**************************************************************************************************/
static void sc7a20h_reg_init(void)
{
    sc7a20h_write_reg(REG_CTRL_REG1, 0x57);             // 设定工作模式100Hz, 同时使能x、y、z三轴测量
    sc7a20h_write_reg(REG_CTRL_REG3, 0x40);             // 将事件路由到INT1物理引脚输出
    sc7a20h_write_reg(REG_CTRL_REG4, 0x88);             // 设置量程为2G
    sc7a20h_write_reg(REG_AOI1_CFG, 0x95);              // 使能x、y、z轴方向检测
    sc7a20h_write_reg(REG_AOI1_THS, 0x30);              // 0x30 = 756, 10 = 256
    sc7a20h_write_reg(REG_AOI1_DURATION, 0x00);         // 设定最短持续事件, 目前一旦满足, 立刻触发中断
}


/**************************************************************************************************
 * @brief  : sc7a20中断初始化
 * @param  : None
 * @return : None
**************************************************************************************************/
static void sc7a20h_isr_init(void)
{
    ret_code_t err_code;
    nrf_drv_gpiote_in_config_t config = GPIOTE_CONFIG_IN_SENSE_HITOLO(true);

    /* 中断引脚配置 */
    nrf_drv_gpiote_in_init(PIN_ISR, &config, sc7a20h_isr_handler);
    nrf_drv_gpiote_in_event_enable(PIN_ISR, true);
}

/**************************************************************************************************
 * @brief  : sc7a20初始化
 * @param  : None
 * @return : None
**************************************************************************************************/
void sc7a20h_init(accel_cb_t cb)
{
    i2c0_init();
    sc7a20h_isr_init();
    sc7a20h_reg_init();

    accel_callback = cb;
}

/*================================================ I2C0控制 ================================================*/

/**************************************************************************************************
 * @brief  : 打印用法
 * @param  : None
 * @return : None
**************************************************************************************************/
static void print_usage(void)
{
    LOG_I("Usage:");
    LOG_I("  i2c0 -r <addr> <reg>");
    LOG_I("  i2c0 -w <addr> <reg> <val>");
}


/**************************************************************************************************
 * @brief  : i2c read
 * @param  : None
 * @return : None
**************************************************************************************************/
static uint8_t i2c_read(uint8_t addr, uint8_t reg)
{
	uint8_t val;
	
    nrf_drv_twi_tx(&twi, addr, &reg, 1, true);
    nrf_drv_twi_rx(&twi, addr, &val, 1);
	
	return val;
}

/**************************************************************************************************
 * @brief  : i2c read
 * @param  : None
 * @return : None
**************************************************************************************************/
static void i2c_write(uint8_t addr, uint8_t reg, uint8_t val)
{
    uint8_t data[2];

    data[0] = reg;
    data[1] = val;

    nrf_drv_twi_tx(&twi, addr, data, 2, false);
}

/**************************************************************************************************
 * @brief  : 执行系统命令
 * @param  : None
 * @return : None
**************************************************************************************************/
static void i2c0_read(int argc, char const *argv[])
{
    unused(argc);
    uint8_t val, len;
    uint8_t addr = strtol(argv[2], NULL, 0);
    uint8_t reg = strtol(argv[3], NULL, 0);

    if(argc == 4) {
        val = i2c_read(addr, reg);
        LOG_I("0x%x (%d)", val, val);
        return;
    }  

    /* 计算长度 */
    len = strtol(argv[4], NULL, 0); 
    while(len--) 
    {
        val = i2c_read(addr, reg++);
        printf("%02x ", val);
    }
    printf("\n");
}

/**************************************************************************************************
 * @brief  : 执行系统命令
 * @param  : None
 * @return : None
**************************************************************************************************/
static void i2c0_write(int argc, char const *argv[])
{   
    unused(argc);
    uint8_t addr = strtol(argv[2], NULL, 0);
    uint8_t reg = strtol(argv[3], NULL, 0);
    uint8_t val = strtol(argv[4], NULL, 0);

    i2c_write(addr, reg, val);
}

static cmd_opt_map_t i2c_map_tbl[] = {
    {"-r",  i2c0_read},
    {"-w",  i2c0_write},
    {NULL,  NULL},
};


/**************************************************************************************************
 * @brief  : 快速操作
 * @param  : None
 * @return : None
**************************************************************************************************/
static bool match_quick_cmd(int argc, char const *argv[])
{
    uint8_t addr, reg, val;

    switch(argc)
    {
    case 3:
        addr = strtol(argv[1], NULL, 0);
        reg = strtol(argv[2], NULL, 0);
        val = i2c_read(addr, reg);
        LOG_I("0x%x (%d)", val, val);   
        return true;

    case 4:
        addr = strtol(argv[1], NULL, 0);
        reg = strtol(argv[2], NULL, 0);
        val = strtol(argv[3], NULL, 0);
        i2c_write(addr, reg, val);
        return true;
    }

    return false;
}

/**************************************************************************************************
 * @brief  : i2c控制
 * @param  : None
 * @return : None
**************************************************************************************************/
void lsh_cmd_i2c0(int argc, char *argv[])
{
    if(argc <= 1) {
        print_usage();
        return;
    }

    /* 匹配指令表 */
    if(match_opt_table(i2c_map_tbl, argv[1], argc, argv))
        return;

    /* 快速指令 */
    if(match_quick_cmd(argc, argv))
        return;
}


#if 0
/**************************************************************************************************
 * @brief  : 测试命令
 * @param  : None
 * @return : None
**************************************************************************************************/
void lsh_cmd_tmp(int argc, char *argv[])
{
    uint8_t x_l, x_h;
    uint8_t y_l, y_h;
    uint8_t z_l, z_h;
    int16_t raw_x, raw_y, raw_z;
    float sens;
    float accel_x, accel_y, accel_z;

    float range_g = 2.0f;

    for(;;) 
    {
        x_l = sc7a20h_read_reg(REG_OUT_X_L);
        x_h = sc7a20h_read_reg(REG_OUT_X_H);
        y_l = sc7a20h_read_reg(REG_OUT_Y_L);
        y_h = sc7a20h_read_reg(REG_OUT_Y_H);
        z_l = sc7a20h_read_reg(REG_OUT_Z_L);
        z_h = sc7a20h_read_reg(REG_OUT_Z_H);

        raw_x = (int16_t)((x_h << 8) | x_l) >> 4;
        raw_y = (int16_t)((y_h << 8) | y_l) >> 4;
        raw_z = (int16_t)((z_h << 8) | z_l) >> 4;

        if(raw_x & 0x0800)
            raw_x = -(~(raw_x & 0x07FF) + 1);

        if(raw_y & 0x0800)
            raw_y = -(~(raw_y & 0x07FF) + 1);

        if(raw_z & 0x0800)
            raw_z = -(~(raw_z & 0x07FF) + 1);

        sens = 2.0f / 4096.0f;

        accel_x = raw_x * sens;
        accel_y = raw_y * sens;
        accel_z = raw_z * sens;

        printf("[%f] [%f] [%f]\n", accel_x, accel_y, accel_z);

        // LOG_I("[0x%2x 0x%2x] [0x%2x 0x%2x] [0x%2x 0x%2x] ", x_l, x_h, y_l, y_h, z_l, z_h);
        nrf_delay_ms(50);
    }
}

#endif