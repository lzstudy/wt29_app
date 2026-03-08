#ifndef __CKERR_H__
#define __CKERR_H__

#include <stdio.h>
#include <string.h>

#define PRINT printf
#define FILE_NAME(x)                    (strrchr(x, '/') ? strrchr(x, '/') + 1 : x)

/* 标准打印 */
#define LOG_I(fmt, ...)                 printf(fmt"\n", ##__VA_ARGS__)
#define DBG_I(fmt, ...)

/* 打印行 */
#define LOG_LINE \
    LOG_I("[%s] [%s] [line:%u]", FILE_NAME(__FILE__), __FUNCTION__, __LINE__);

/* 提示信息 */
#define CHECK_RET(func, cond, ret)                                                                                               \
    if(cond) {                                                                                                          \
        PRINT("[%s:%s:%u - %s] [fail:%s]\n", FILE_NAME(__FILE__), __FUNCTION__, __LINE__, #func, #cond);                 \
        return ret;                                                                                                      \
    }

/* 提示信息 */
#define CP_RET(func, cond)                                                                                               \
    if(!cond) {                                                                                                          \
        PRINT("[%s:%s:%u - %s] [fail:%s (null)]\n", FILE_NAME(__FILE__), __FUNCTION__, __LINE__, #func, #cond);          \
        return NULL;                                                                                                     \
    }

#define CI_RET(func, cond, ret)                                                                                          \
    if(cond) {                                                                                                           \
        PRINT("[%s:%s:%u - %s] [fail:%s] [ret:%d] \n", FILE_NAME(__FILE__), __FUNCTION__, __LINE__, #func, #cond, ret);  \
        return ret;                                                                                                      \
    }

#define _CI_RET(func, cond)                                                                               \
    if(cond) {                                                                                            \
        PRINT("[%s:%s:%u - %s] [fail:%s]\n", FILE_NAME(__FILE__), __FUNCTION__, __LINE__, #func, #cond);  \
        return;                                                                                           \
    }

#define CI_GOTO(func, cond, label)                                                                        \
    if(cond) {                                                                                            \
        PRINT("[%s:%s:%u - %s] [fail:%s]\n", FILE_NAME(__FILE__), __FUNCTION__, __LINE__, #func, #cond);  \
        goto label;                                                                                       \
    }

#define CI_CUSTOM(func, cond, custom)                                                                     \
    if(cond) {                                                                                            \
        PRINT("[%s:%s:%u - %s] [fail:%s]\n", FILE_NAME(__FILE__), __FUNCTION__, __LINE__, #func, #cond);  \
        custom;                                                                                           \
    }



/* 无提示信息 */
#define CK_RET(cond, ret)               do {if(ret) {return ret;}} while(0)
#define _CK_RET(cond)                   do {if(cond) {return;}} while(0)
#define CK_GOTO(cond, label)            do {if(cond) {goto label;}} while(0)
#define CK_CUSTOM(cond, custom)         do {if(cond) {custom;}} while(0)

/* 检测 - 参数 */
#define CI_ARG  CI_RET
#define CK_ARG  CK_RET

/* 简化使用 */
#define CIU_RET(func, int)              CI_RET(func, int < 0, int)              /**< 判断整数, 返回值 < 0报错 */
#define CIP_RET(func, pointer)          CI_RET(func, !pointer, -1)              /**< 判断指针 */
#define CKU_RET(int)                    CK_RET(int < 0, int)                    /**< 判断整数 - 无提示*/
#define CKP_RET(pointer)                CK_RET(!pointer, -1)                    /**< 判断指针 - 无提示*/

#define _CIU_RET(func, int)             _CI_RET(func, int < 0)                  /**< 判断整数, 返回值 < 0 报错 */
#define _CIP_RET(func, pointer)         _CI_RET(func, !pointer)                 /**< 判断指针 */
#define _CKU_RET(int)                   _CK_RET(int < 0)                        /**< 判断整数 */
#define _CKP_RET(pointer)               _CK_RET(!pointer)                       /**< 判断指针 */

#define CIU_GOTO(func, int, label)      CI_GOTO(func, int < 0, label)           /**< 跳转到某个标签 */
#endif
