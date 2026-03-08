#include <stdint.h>
#include "lsh.h"
#include "common.h"
#include "nrf.h"
#include "system.h"
#include "nrf_nvmc.h"

#define SN_MAGIC_NUMBER         0x1B3E579D2F4A8C61ULL 

/**************************************************************************************************
 * @brief  : 打印用法
 * @param  : None
 * @return : 0成功, -1失败
**************************************************************************************************/
static void print_usage(void)
{
    LOG_I("Usage:");
    LOG_I("  sn -cpuid         - get cpu unique id");
    LOG_I("  sn -gen           - generate sn");
}

/**************************************************************************************************
 * @brief  : 获取芯片ID
 * @param  : None
 * @return : None
**************************************************************************************************/
static uint64_t get_cpuid_info(void) 
{	
    uint32_t cpuid_h = NRF_FICR->DEVICEID[0];
    uint32_t cpuid_l = NRF_FICR->DEVICEID[1];
	
	return ((uint64_t)cpuid_h << 32) | cpuid_l;
}

/**************************************************************************************************
 * @brief  : 计算SN (简单的哈希算法)
 * @param  : None
 * @return : None
**************************************************************************************************/
static uint64_t calculate_sn_info(uint64_t cpuid)
{
    return cpuid ^ SN_MAGIC_NUMBER;

}

/**************************************************************************************************
 * @brief  : 计算index
 * @param  : None
 * @return : None
**************************************************************************************************/
static uint64_t calculate_cpuid_info(uint64_t sn)
{
    return sn ^ SN_MAGIC_NUMBER;
}

/**************************************************************************************************
 * @brief  : 打印SN信息
 * @param  : None
 * @return : None
**************************************************************************************************/
static void print_sn_info(void)
{
    LOG_I("sn: %llx", get_sn_info());
}

/**************************************************************************************************
 * @brief  : 擦除SN信息
 * @param  : None
 * @return : None
**************************************************************************************************/
static void write_sn_info(uint64_t sn)
{
    uint32_t sn_l = (uint32_t)(sn & 0xFFFFFFFF); 
    uint32_t sn_h = (uint32_t)(sn >> 32);

    LOG_I("sn_l = %x, sn_h = %x", sn_l, sn_h);

    sleep(1);

    // 2. 使能NVMC写操作
    NRF_NVMC->CONFIG = NVMC_CONFIG_WEN_Wen;
    while (NRF_NVMC->READY == NVMC_READY_READY_Busy) {}
		

    // 4. 等待写入完成
    while (NRF_NVMC->READY == NVMC_READY_READY_Busy) {}

    // 5. 关闭写使能
    NRF_NVMC->CONFIG = NVMC_CONFIG_WEN_Ren;
    while (NRF_NVMC->READY == NVMC_READY_READY_Busy) {}
}


/**************************************************************************************************
 * @brief  : 打印sn信息
 * @param  : None
 * @return : None
**************************************************************************************************/
static void print_cpu_info(int argc, char const *argv[])
{    
    unused(argc);
    unused(argv);
    LOG_I("cpuid: %llx", get_cpuid_info());
}

/**************************************************************************************************
 * @brief  : 擦除SN信息
 * @param  : None
 * @return : None
**************************************************************************************************/
static void sn_info_earse(int argc, char const *argv[])
{
    unused(argc);
    unused(argv);
    earse_sn_info();
}

/**************************************************************************************************
 * @brief  : 生成SN
 * @param  : None
 * @return : None
**************************************************************************************************/
static void generate_sn(int argc, char const *argv[])
{
	uint64_t cpuid;
    uint64_t sn;

    /* 获取cpuid */
    cpuid = get_cpuid_info();			    // 获取芯片ID
    sn = calculate_sn_info(cpuid);          // 计算SN
    set_sn_info(sn);                        // 写入SN

    LOG_I("SN = %llx", sn);
}

/**************************************************************************************************
 * @brief  : 生成SN
 * @param  : None
 * @return : None
**************************************************************************************************/
static void verify_cpuid(int argc, char const *argv[])
{
    uint64_t sn = get_sn_info();
    uint64_t cpuid = calculate_cpuid_info(sn);

    LOG_I("get: %llx", get_cpuid_info());
    LOG_I("cal: %llx", cpuid);
}



/**************************************************************************************************
 * @brief  : 校验SN
 * @param  : None
 * @return : None
**************************************************************************************************/
void check_sn_info(void)
{
    uint64_t sn = get_sn_info();
    uint64_t cal_cpuid = calculate_cpuid_info(sn);
    uint64_t get_cpuid = get_cpuid_info();
    
    if(cal_cpuid == get_cpuid)
        return;

    /* 如果SN没校验通过 卡在这里 */
    for(;;)
    {
        LOG_I("sn is not valid");
        sleep(1);
    }
}


static cmd_opt_map_t mm_map_tbl[] = {
    {"-cpuid",   print_cpu_info},
    {"-e",       sn_info_earse},
	{"-gen",     generate_sn},
    {"-v",       verify_cpuid},
};

/**************************************************************************************************
 * @brief  : 获取分辨率
 * @param  : None
 * @return : None
**************************************************************************************************/
void lsh_cmd_sn(int argc, char *argv[])
{

    if(argc <= 1) {
        print_sn_info();
        return;
    }

    /* 打印帮助信息 */
    if(!strcmp(argv[1], "-h")) {
        print_usage();
        return;
    }

    /* 匹配指令表 */
    if(match_opt_table(mm_map_tbl, argv[1], argc, argv))
        return;
}