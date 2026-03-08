#include "lsh.h"
#include "common.h"
#include "nrf_gpio.h"

/**************************************************************************************************
 * @brief  : 打印用法
 * @param  : None
 * @return : 0成功, -1失败
**************************************************************************************************/
static void print_usage(void)
{
    LOG_I("Usage:");
    LOG_I("  pin <pin> <val>");
    LOG_I("  pin <pin>");
}

/**************************************************************************************************
 * @brief  : 快速操作
 * @param  : None
 * @return : None
**************************************************************************************************/
static void pin_quick_cmd(int argc, char const *argv[])
{
    uint16_t pin = atoi(argv[1]);
    uint8_t val = atoi(argv[2]);
    
    switch(argc)
    {
    case 2:
        val = nrf_gpio_pin_read(pin);
        LOG_I("%d",val);
        break;

    case 3:
        nrf_gpio_pin_write(pin, val);
        break;

    default:
        break;
    }
}

/**************************************************************************************************
 * @brief  : pin命令
 * @param  : None
 * @return : None
**************************************************************************************************/
void lsh_cmd_pin(int argc, char const *argv[])
{
    if(argc <= 1) {
        print_usage();
        return;
    }

    /* 打印帮助信息 */
    if(!strcmp(argv[1], "-h")) {
        print_usage();
        return;
    }

    pin_quick_cmd(argc, argv);
}

/**************************************************************************************************
 * @brief  : pin命令
 * @param  : None
 * @return : None
**************************************************************************************************/
void lsh_cmd_pinmap(int argc, char const *argv[])
{
    uint32_t port;
    uint32_t pin;
    uint32_t result;

    if(argc < 2) {
        LOG_I("usage : pinmap <port> <pin>");
        return;
    }

    port = atoi(argv[1]);
    pin = atoi(argv[2]);

    /* 计算结果 */
    result = (port << 5) | (pin & 0x1F);
    LOG_I("%d", result);
}