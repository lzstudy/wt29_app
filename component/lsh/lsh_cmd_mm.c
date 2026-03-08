#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "lsh.h"
#include "lsh_type.h"
#include "lsh_utils.h"
#include "common.h"

typedef enum _addr_colum_fmt{
    ADDR_COLUM_16 = 16,
    ADDR_COLUM_32 = 32,
}addr_colum_fmt;

typedef enum _addr_width_fmt {
    ADDR_WIDTH_1 = 1,
    ADDR_WIDTH_2 = 2,
    ADDR_WIDTH_4 = 4,
}addr_width_fmt;

/**************************************************************************************************
 * @brief  : 打印用法
 * @param  : None
 * @return : 0成功, -1失败
**************************************************************************************************/
static void print_mm_info(void)
{

}

/**************************************************************************************************
 * @brief  : 打印用法
 * @param  : None
 * @return : 0成功, -1失败
**************************************************************************************************/
static void print_usage(void)
{
    LOG_I("Usage:");
    LOG_I("  mm -r <addr>");
    LOG_I("  mm -r <addr> <size>");
    LOG_I("  mm -r <addr> <size> -w <1/2/4> -l <16/32>");
    LOG_I("  mm -w <addr> <val>");
    LOG_I("  mm -w <addr> <val> <size>");
}

/**************************************************************************************************
 * @brief  : 打印写入信息
 * @param  : None
 * @return : None
**************************************************************************************************/
static void print_write_info(char *fmt, void *addr, uint32_t val)
{
    LOG_I("write [%d] to [%p]", val, addr);
}

/**************************************************************************************************
 * @brief  : 读取内存
 * @param  : None
 * @return : 0成功, -1失败
**************************************************************************************************/
static inline void mm_read_u8(void *addr)
{
    uint8_t val = *(uint8_t *)addr;
    printf("%02x ", val);
}

/**************************************************************************************************
 * @brief  : 读取内存
 * @param  : None
 * @return : 0成功, -1失败
**************************************************************************************************/
static inline void mm_read_u16( void *addr)
{
    uint16_t val = *(uint16_t *)addr;
    printf("%04x ", val);
}

/**************************************************************************************************
 * @brief  : 读取内存
 * @param  : None
 * @return : 0成功, -1失败
**************************************************************************************************/
static inline void mm_read_u32(void *addr)
{
    uint32_t val = *(uint32_t *)addr;
    printf("%08x ", val);
}

/**************************************************************************************************
 * @brief  : 读取内存
 * @param  : None
 * @return : None
**************************************************************************************************/
static void mm_read_cell(void *addr, uint8_t width)
{
    switch(width)
    {
    case 1:
        mm_read_u8(addr);
        break;

    case 2:
        mm_read_u16(addr);
        break;

    case 4:
        mm_read_u32(addr);
        break;

    default:
        break;
    }
}

/**************************************************************************************************
 * @brief  : dump内存
 * @param  : None
 * @return : None
**************************************************************************************************/
static void lsh_mm_dump(void *start, uint32_t size, uint8_t width, uint8_t colum)
{
    uint8_t *addr, *begin;
    uint8_t pos = 0;

    begin = (uint8_t *)start;

    printf("\n");

    for(addr = begin ; addr < begin + size ; addr += width)
    {
        /* 位置为0， 打印地址*/
        if(pos == 0)
            printf("%lx : ", addr);

        mm_read_cell(addr, width);

        /* 如果到末尾了, 换行打印 */
        if(++pos == colum / width) {
            printf("\n");
            pos = 0;
        }
    }

    printf("\n");
}

/**************************************************************************************************
 * @brief  : 简单的读取
 * @param  : None
 * @return : None
**************************************************************************************************/
static void lsh_mm_read_simple(int argc, char const *argv[])
{
    size_t val;
    void *addr = (void *)strtoul(argv[2], NULL, 0);

    val = *(size_t *)addr;
    LOG_I("0x%x (%d)", val, val);
}

/**************************************************************************************************
 * @brief  : 突发读取
 * @param  : None
 * @return : None
**************************************************************************************************/
static void lsh_mm_read_burst(int argc, char const *argv[])
{
    void *addr;
    uint16_t size;
    
    addr = (void *)strtoul(argv[2], NULL, 0);
    size = strtoul(argv[3], NULL, 0);

    /* dump内存 */
    lsh_mm_dump(addr, size, ADDR_WIDTH_1, ADDR_COLUM_16);
}

/**************************************************************************************************
 * @brief  : 标准读取
 * @param  : None
 * @return : None
**************************************************************************************************/
static void lsh_mm_read_stand(int argc, char const *argv[])
{
    void *addr;
    uint16_t size;
    uint8_t width, colum;
    
    addr  = (void *)strtoul(argv[2], NULL, 0);
    size  = strtoul(argv[3], NULL, 0);
    width = strtoul(argv[5], NULL, 0);
    colum = strtoul(argv[7], NULL, 0);

    /* 不符合条件的直接退出*/
    if(width != ADDR_WIDTH_1 && width != ADDR_WIDTH_2 && width != ADDR_WIDTH_4) {
        LOG_I("Invaild width %d", width);
        return;
    }

    if(colum != ADDR_COLUM_16 && colum != ADDR_COLUM_32) {
        LOG_I("Invaild colum %d", colum);
        return;
    }

    /* dump内存 */
    lsh_mm_dump(addr, size, width, colum);
}

/**************************************************************************************************
 * @brief  : 执行系统命令
 * @param  : None
 * @return : None
**************************************************************************************************/
static void lsh_mm_read(int argc, char const *argv[])
{
    /* 此处根据参数数量决定操作 */
    switch(argc)
    {
    case 3:
        lsh_mm_read_simple(argc, argv);
        break;

    case 4:
        lsh_mm_read_burst(argc, argv);
        break;

    case 8:
        lsh_mm_read_stand(argc, argv);
        break;

    default:

        break;
    }
}

/**************************************************************************************************
 * @brief  : 简单的读取
 * @param  : None
 * @return : None
**************************************************************************************************/
static void lsh_mm_write_simple(int argc, char const *argv[])
{
    void *addr = (void *)strtoul(argv[2], NULL, 0);
    size_t val = strtoul(argv[3], NULL, 0);

    *(uint32_t *)addr = val;

    print_write_info("0x%lx (%ld)", addr, val);
}

/**************************************************************************************************
 * @brief  : 简单的读取
 * @param  : None
 * @return : None
**************************************************************************************************/
static void lsh_mm_write_stand(int argc, char const *argv[])
{
    uint8_t *addr = (uint8_t *)strtoul(argv[2], NULL, 0);
    uint8_t value = strtoul(argv[3], NULL, 0);
    uint16_t size = strtoul(argv[4], NULL, 0);

    memset(addr, value, size);

    LOG_I("write [ 0x%x - 0x%x ] val [ 0x%02x (%d)%s ]", addr, addr + size, value, value);
}

/**************************************************************************************************
 * @brief  : 执行系统命令
 * @param  : None
 * @return : None
**************************************************************************************************/
static void lsh_mm_write(int argc, char const *argv[])
{
    switch(argc)
    {
    case 4:
        lsh_mm_write_simple(argc, argv);
        break;

    case 5:
        lsh_mm_write_stand(argc, argv);
        break;
    }
}

static cmd_opt_map_t mm_map_tbl[] = {
    {"-r",  lsh_mm_read},
    {"-w",  lsh_mm_write},
};

/**************************************************************************************************
 * @brief  : 快速读取
 * @param  : None
 * @return : None
**************************************************************************************************/
static void mm_quick_read(int argc, char const *argv[])
{
    void *addr = (void *)strtoul(argv[1], NULL, 0);
    uint32_t val = *(uint32_t *)addr;

    LOG_I("0x%lx (%ld)", val, val);
}

/**************************************************************************************************
 * @brief  : 快速读取
 * @param  : None
 * @return : None
**************************************************************************************************/
static void mm_quick_write(int argc, char const *argv[])
{
    void *addr = (void *)strtoul(argv[1], NULL, 0);
    size_t val = strtoul(argv[2], NULL, 0);

    *(uint32_t *)addr = val;

    print_write_info("0x%lx (%ld)", addr, val);
}

/**************************************************************************************************
 * @brief  : 快速操作
 * @param  : None
 * @return : None
**************************************************************************************************/
static bool mm_quick_cmd(int argc, char const *argv[])
{
    switch(argc)
    {
    case 2:
        mm_quick_read(argc, argv);
        return true;

    case 3:
        mm_quick_write(argc, argv);
        return true;
    }

    return false;
}

/**************************************************************************************************
 * @brief  : 注册变量
 * @param  : None
 * @return : None
**************************************************************************************************/
void lsh_cmd_mm(int argc, char const *argv[])
{
    if(argc <= 1) {
        print_mm_info();
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

    /* 快速指令 */
    if(mm_quick_cmd(argc, argv))
        return;
 
    LOG_I("mm unsupport option");
}