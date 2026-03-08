#ifndef __MSGBUS_TYPE_H__
#define __MSGBUS_TYPE_H__
#include <stdint.h>
#include "slist.h"

typedef void (*msgbus_cb_t)(void *data, uint16_t len, void *priv);              /**# 消息回调 */

typedef enum _msgbus_type {
    MSGBUS_TYPE_NORMAL,                                                         /**< 00' 普通消息 */
    MSGBUS_TYPE_PRIORITY,                                                       /**< 01' 优先消息 */
    MSGBUS_TYPE_URGENT,                                                         /**< 02' 紧急消息 */
}msgbus_type_t;

typedef struct _msgbus_topic {
    char name[32];                                                              /**@ topic名字 */
    struct list_head list;                                                      /**@ node节点 */
    struct list_head head_client;                                               /**@ 客户端表头 */
}msgbus_topic_t;

typedef struct _msgbus_client {
    msgbus_topic_t *topic;                                                      /**@ 绑定主题 */
    struct list_head list;                                                      /**@ node节点 */
    msgbus_cb_t cb;                                                             /**@ 回调 */
    const char *name;                                                           /**@ client名称 */
    void *priv;                                                                 /**@ 私有数据 */
}msgbus_client_t;


typedef struct _msgbus_msg {
    msgbus_topic_t *topic;                                                      /**@ topic */
    uint16_t len;                                                               /**@ 数据长度 */
    uint8_t data[64];                                                           /**@ 数据 */
}msgbus_msg_t;

typedef enum _msgbus_client_attr {
    MBC_ATTR_NAME,                                                              /**@ 设置名字属性 */
}msgbus_client_attr_t;

#endif

