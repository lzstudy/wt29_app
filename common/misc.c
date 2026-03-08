#include <stdio.h>  
#include <stdint.h>

/**************************************************************************************************
 * @brief  : 打印hex数据
 * @param  : 提示字符
 * @param  : 数据
 * @param  : 数据长度
 * @return : 串口句柄, NULL - 失败
 **************************************************************************************************/
void print_hex_value(const char *prompt, void *data, int datalen)
{
    uint8_t *val = data;

    printf("%s(%d bytes): ", prompt, datalen);
    for(uint8_t i = 0 ; i < datalen ; i++)
    {
        printf("0x%02x ", val[i]);
    }
    printf("\n");
}
