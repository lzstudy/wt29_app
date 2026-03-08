#include <nrf_drv_twi.h>
#include <nrf_delay.h>
#include <nrf_gpio.h>
#include <nrf_drv_gpiote.h>
#include "common.h"
#include "app_error.h"
#include "drv_qmc5883p.h"
#include "lsh.h"

#define QMC5883P_ADDR   0x2C                                                    /**< 器件地址 */
#define IIC_SCL		    NRF_GPIO_PIN_MAP(0, 11)
#define IIC_SDA		    NRF_GPIO_PIN_MAP(0, 12)

typedef enum _qmc5883p_reg {
    REG_CHIP_ID      = 0x00,                                                    /**< 芯片ID寄存器 */
    REG_DATA_X_LSB   = 0x01,                                                    /**< X轴数据低字节 */
    REG_DATA_X_MSB   = 0x02,                                                    /**< X轴数据高字节 */
    REG_DATA_Y_LSB   = 0x03,                                                    /**< Y轴数据低字节 */
    REG_DATA_Y_MSB   = 0x04,                                                    /**< Y轴数据高字节 */
    REG_DATA_Z_LSB   = 0x05,                                                    /**< Z轴数据低字节 */
    REG_DATA_Z_MSB   = 0x06,                                                    /**< Z轴数据高字节 */
    REG_STATUS       = 0x09,                                                    /**< 状态寄存器 */
    REG_CONTROL_1    = 0x0A,                                                    /**< 控制寄存器1 */
    REG_CONTROL_2    = 0x0B,                                                    /**< 控制寄存器2 */
    
};

/* 工作模式 (MODE) - 控制寄存器1 bit[1:0] */
typedef enum {
    QMC5883P_MODE_SUSPEND     = 0x00,                                           /**< 挂起模式 (低功耗) */ 
    QMC5883P_MODE_NORMAL      = 0x01,                                           /**< 正常模式 */
    QMC5883P_MODE_SINGLE      = 0x02,                                           /**< 单次测量模式 */
    QMC5883P_MODE_CONTINUOUS  = 0x03,                                           /**< 连续模式 */
} qmc5883p_mode_t;

/* 输出数据率 (ODR) - 控制寄存器1 bit[3:2] */
typedef enum {
    QMC5883P_ODR_10HZ         = (0x00 << 2),                                    /**< 10 Hz */
    QMC5883P_ODR_50HZ         = (0x01 << 2),                                    /**< 50 Hz */
    QMC5883P_ODR_100HZ        = (0x02 << 2),                                    /**< 100 Hz */
    QMC5883P_ODR_200HZ        = (0x03 << 2),                                    /**< 200 Hz */
} qmc5883p_odr_t;

/* Set/Reset 模式 (SR_MODE) - 控制寄存器2 bit[1:0] */
typedef enum {
    QMC5883P_SR_MODE_SET_RESET_ON = 0x00,                                       /**< 置位/复位开启 (推荐，自动偏移抵消) */
    QMC5883P_SR_MODE_SET_ONLY     = 0x01,                                       /**< 仅置位开启 */
    QMC5883P_SR_MODE_OFF          = 0x02,                                       /**< 置位/复位关闭 */
} qmc5883p_sr_mode_t;

/* 量程 (RNG) - 控制寄存器2 bit[3:2] */
typedef enum {
    QMC5883P_RNG_2G           = (0x00 << 2),                                    /**< ±2 Gauss  (最高灵敏度: 15000 LSB/G) */
    QMC5883P_RNG_8G           = (0x01 << 2),                                    /**< ±8 Gauss  (灵敏度: 3750 LSB/G) */
    QMC5883P_RNG_12G          = (0x02 << 2),                                    /**< ±12 Gauss (灵敏度: 2500 LSB/G) */
    QMC5883P_RNG_30G          = (0x03 << 2),                                    /**< ±30 Gauss (灵敏度: 1000 LSB/G) */
} qmc5883p_range_t;

static nrf_drv_twi_t m_twi = NRF_DRV_TWI_INSTANCE(1);

/**************************************************************************************************
 * @brief  : 写寄存器
 * @param  : None
 * @return : None
**************************************************************************************************/
static void qmc5883p_write_byte(uint8_t reg, uint8_t value)
{
    uint8_t data[2];

    data[0] = reg;
    data[1] = value;

    nrf_drv_twi_tx(&m_twi, QMC5883P_ADDR, data, 2, false);
}

/**************************************************************************************************
 * @brief  : 读取一个字节
 * @param  : None
 * @return : None
**************************************************************************************************/
static uint8_t qmc5883p_read_byte(uint8_t reg) 
{
    uint8_t value;

    nrf_drv_twi_tx(&m_twi, QMC5883P_ADDR, &reg, 1, true);
    nrf_drv_twi_rx(&m_twi, QMC5883P_ADDR, &value, 1);

    return value;
}

/**************************************************************************************************
 * @brief  : 调试命令
 * @param  : None
 * @return : None
**************************************************************************************************/
static void i2c1_init(void)
{
    nrf_drv_twi_config_t config;

    /* i2c配置 */
    config.scl = IIC_SCL;
    config.sda = IIC_SDA;
    config.frequency = NRF_DRV_TWI_FREQ_100K;
    config.interrupt_priority = APP_IRQ_PRIORITY_LOWEST;
    
    /* i2c初始化 */
    nrf_drv_twi_init(&m_twi, &config, NULL, NULL);
    nrf_drv_twi_enable(&m_twi); 
}

/**************************************************************************************************
 * @brief  : qmc5883p 初始化
 * @param  : None
 * @return : None
**************************************************************************************************/
void qmc5883p_init(void)
{
    i2c1_init(); 

    qmc5883p_write_byte(REG_CONTROL_2, 0x00);    //0x04);//0x0C);
    nrf_delay_ms(10);                            // 等待复位完成

    /* 配置采样频率 */
    qmc5883p_write_byte(REG_CONTROL_1, QMC5883P_MODE_CONTINUOUS | QMC5883P_ODR_10HZ);
}

/**************************************************************************************************
 * @brief  : 读取x
 * @param  : None
 * @return : None
**************************************************************************************************/
int16_t qmc5883p_read_x(void)
{
    uint8_t val_l = qmc5883p_read_byte(REG_DATA_X_LSB);
    uint8_t val_h = qmc5883p_read_byte(REG_DATA_X_MSB);

    return (int16_t)((val_l << 8) | val_h);
}

/**************************************************************************************************
 * @brief  : 读取y
 * @param  : None
 * @return : None
**************************************************************************************************/
int16_t qmc5883p_read_y(void)
{
    uint8_t val_l = qmc5883p_read_byte(REG_DATA_Y_LSB);
    uint8_t val_h = qmc5883p_read_byte(REG_DATA_Y_MSB);

    return (int16_t)((val_l << 8) | val_h);
}

/**************************************************************************************************
 * @brief  : 读取z
 * @param  : None
 * @return : None
**************************************************************************************************/
int16_t qmc5883p_read_z(void)
{
    uint8_t val_l = qmc5883p_read_byte(REG_DATA_Z_LSB);
    uint8_t val_h = qmc5883p_read_byte(REG_DATA_Z_MSB);

    return (int16_t)((val_l << 8) | val_h);
}

/*===================================================== 调试 ===============================================*/
/**************************************************************************************************
 * @brief  : 调试命令
 * @param  : None
 * @return : None
**************************************************************************************************/
void lsh_dbg_qmc5883p(int argc, char *argv[])
{
    uint8_t value;
    uint8_t index = atoi(argv[1]);

    switch(index)
    {
    case 1:
        value = qmc5883p_read_byte(REG_CHIP_ID);
        LOG_I("chip id = %x", value);
        break;

    }
}


/*================================================ I2C1控制 ================================================*/


/**************************************************************************************************
 * @brief  : 打印用法
 * @param  : None
 * @return : None
**************************************************************************************************/
static void print_usage(void)
{
    LOG_I("Usage:");
    LOG_I("  i2c1 -r <addr> <reg>");
    LOG_I("  i2c1 -w <addr> <reg> <val>");
}

/**************************************************************************************************
 * @brief  : i2c read
 * @param  : None
 * @return : None
**************************************************************************************************/
static uint8_t i2c_read(uint8_t addr, uint8_t reg)
{
    uint8_t val;

    nrf_drv_twi_tx(&m_twi, addr, &reg, 1, true);
    nrf_drv_twi_rx(&m_twi, addr, &val, 1);

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

    nrf_drv_twi_tx(&m_twi, addr, data, 2, false);
}

/**************************************************************************************************
 * @brief  : 执行系统命令
 * @param  : None
 * @return : None
**************************************************************************************************/
static void i2c1_read(int argc, char const *argv[])
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
static void i2c1_write(int argc, char const *argv[])
{
    unused(argc);

    uint8_t addr = strtol(argv[2], NULL, 0);
    uint8_t reg = strtol(argv[3], NULL, 0);
    uint8_t val = strtol(argv[4], NULL, 0);

    i2c_write(addr, reg, val);
}

static cmd_opt_map_t i2c_map_tbl[] = {
    {"-r",  i2c1_read},
    {"-w",  i2c1_write},
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
void lsh_cmd_i2c1(int argc, char *argv[])
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
