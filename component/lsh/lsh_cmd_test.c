#include <stdio.h>
#include <string.h>
#include "lsh.h"
#include "common.h"

#include "lvgl.h"
#include "text.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#include "nrf_drv_saadc.h"
#include <nrf_drv_twi.h>
#include <nrf_gpio.h>
#include <nrf_drv_gpiote.h>

//磁力传感器 QMC5883P
#define QMC5883P_ADDR   0x2C  // I2C 地址（SA0=0）
#define QMC5883P_REG_CHIP_ID	0x00 //0x0 // 芯片ID寄存器
// QMC5883P 寄存器定义
#define QMC5883P_REG_DATA_OUT_X_LSB   0x01
#define QMC5883P_REG_DATA_OUT_X_MSB   0x02
#define QMC5883P_REG_DATA_OUT_Y_LSB   0x03
#define QMC5883P_REG_DATA_OUT_Y_MSB   0x04
#define QMC5883P_REG_DATA_OUT_Z_LSB   0x05
#define QMC5883P_REG_DATA_OUT_Z_MSB   0x06
#define QMC5883P_REG_STATUS           0x06
//#define QMC5883P_REG_TEMP_LSB         0x07
//#define QMC5883P_REG_TEMP_MSB         0x08
//#define QMC5883P_REG_CONTROL_1        0x09
//#define QMC5883P_REG_CONTROL_2        0x0A
//#define QMC5883P_REG_PERIOD           0x0B
//#define QMC5883P_REG_CHIP_ID          0x0D // 芯片ID寄存器

// 控制寄存器1配置
#define QMC5883P_MODE_STANDBY         0x00


#define QMC5883P_MODE_CONTINUOUS      0x03
#define QMC5883P_ODR_10HZ             0x00
#define QMC5883P_ODR_50HZ             0x04
#define QMC5883P_ODR_100HZ            0x08
#define QMC5883P_ODR_200HZ            0x0C
#define QMC5883P_RNG_2G               0x00
#define QMC5883P_RNG_8G               0x10
#define QMC5883P_OSR_512              0x00
#define QMC5883P_OSR_256              0x40
#define QMC5883P_OSR_128              0x80
#define QMC5883P_OSR_64               0xC0

// I2C 实例
#define QMC_IIC_SCL		NRF_GPIO_PIN_MAP(0,11)
#define QMC_IIC_SDA		NRF_GPIO_PIN_MAP(0,12)
//全局TWI实例
nrf_drv_twi_t m_twi = NRF_DRV_TWI_INSTANCE(1);
//初始化 I2C
void QMC_i2c_init(void) {
    nrf_drv_twi_config_t config = {
        .scl = QMC_IIC_SCL, 
        .sda = QMC_IIC_SDA,
        .frequency = NRF_DRV_TWI_FREQ_100K,
        .interrupt_priority = APP_IRQ_PRIORITY_LOWEST,
				//.hold_bus_uninit =1
    };
    nrf_drv_twi_init(&m_twi, &config, NULL, NULL);
    nrf_drv_twi_enable(&m_twi);
}

//向QMC5883P的寄存器写入一个字节
ret_code_t qmc5883p_write_byte(uint8_t reg, uint8_t data) {
    ret_code_t err_code;
    uint8_t buffer[2] = {reg, data};
    err_code = nrf_drv_twi_tx(&m_twi, QMC5883P_ADDR, buffer, 2, false);
    return err_code;
}

//从QMC5883P的寄存器读取一个字节
ret_code_t qmc5883p_read_byte(uint8_t reg, uint8_t *data) {
    ret_code_t err_code;
    //先写入要读取的寄存器地址
    err_code = nrf_drv_twi_tx(&m_twi, QMC5883P_ADDR, &reg, 1, false);
    if (err_code != NRF_SUCCESS) {
        return err_code;
    }
    //然后读取该寄存器的值
    err_code = nrf_drv_twi_rx(&m_twi, QMC5883P_ADDR, data, 1);
    return err_code;
}


// 从QMC5883P读取多个字节 (用于读取传感器数据)
ret_code_t qmc5883p_read_bytes(uint8_t start_reg, uint8_t *data, uint32_t length) {
    ret_code_t err_code;
    // 先写入要起始寄存器地址
    err_code = nrf_drv_twi_tx(&m_twi, QMC5883P_ADDR, &start_reg, 1, true);
    if (err_code != NRF_SUCCESS) {
        return err_code;
    }
    // 然后连续读取多个字节
    err_code = nrf_drv_twi_rx(&m_twi, QMC5883P_ADDR, data, length);
    return err_code;
}

// 读取并验证QMC5883P的芯片ID (版本信息)
void qmc5883p_get_chip_id(uint8_t *chip_id_t) {
    qmc5883p_read_byte(QMC5883P_REG_CHIP_ID, chip_id_t);
}

//初始化QMC5883P传感器
int test_qmc5883p_init(void) {

//		nrf_gpio_cfg_output(QMC_IIC_SCL);
//		nrf_gpio_cfg_output(QMC_IIC_SDA);
//		nrf_gpio_pin_write(QMC_IIC_SCL, 0);
//		nrf_gpio_pin_write(QMC_IIC_SDA, 1);
    ret_code_t err_code;
		
		QMC_i2c_init();
		
		#if 1
    //软件复位 (设置控制寄存器2的SOFT_RST位)
    err_code = qmc5883p_write_byte(0x0b, 0x00); //0x04);//0x0C);
    if (err_code != NRF_SUCCESS) {
        return err_code;
    }
    nrf_delay_ms(10); // 等待复位完成
    // 配置周期寄存器 (建议值0x01)
//    err_code = qmc5883p_write_byte(QMC5883P_REG_PERIOD, 0x01);
//    if (err_code != NRF_SUCCESS) {
//        return err_code;
//    }


    //配置控制寄存器1 (连续测量模式, 10Hz, 2G量程, OSR=256)
    uint8_t ctrl1_config = QMC5883P_MODE_CONTINUOUS | QMC5883P_ODR_10HZ;
    err_code = qmc5883p_write_byte(0x0a, ctrl1_config);
		
    return err_code;
		
		#endif

}
uint8_t qmc5883p_data[6];
int16_t test_data[3];

// 读取磁力计原始数据 (X, Y, Z)
ret_code_t qmc5883p_read_mag_data(int16_t *mag_x, int16_t *mag_y, int16_t *mag_z){
    ret_code_t err_code;
    memset(qmc5883p_data, 0, sizeof(qmc5883p_data));
    // 读取6个数据寄存器 (XLSB, XMSB, YLSB, YMSB, ZLSB, ZMSB)
    err_code = qmc5883p_read_bytes(QMC5883P_REG_DATA_OUT_X_LSB, qmc5883p_data, sizeof(qmc5883p_data));
    if (err_code != NRF_SUCCESS){
        return err_code;
    }
    // 组合数据 (注意: QMC5883P通常是LSB在前，MSB在后)
    *mag_x = (int16_t)((qmc5883p_data[1] << 8) | qmc5883p_data[0]);
    *mag_y = (int16_t)((qmc5883p_data[3] << 8) | qmc5883p_data[2]);
    *mag_z = (int16_t)((qmc5883p_data[5] << 8) | qmc5883p_data[4]);

    LOG_I("x = %x, y = %x, z = %x", *mag_x, *mag_y, *mag_z);
    return NRF_SUCCESS;
}


/**************************************************************************************************
 * @brief  : i2c控制
 * @param  : None
 * @return : None
**************************************************************************************************/
void lsh_cmd_test(int argc, char *argv[])
{
    uint8_t id;
    int16_t x, y, z;
    test_qmc5883p_init();

    qmc5883p_get_chip_id(&id);
    LOG_I("id = 0x%x", id);

#if 1
    for(;;)
    {
        qmc5883p_read_mag_data(&test_data[0], &test_data[1], &test_data[2]);
        sleep(1);
    }
#endif
    
}