#ifndef __MSGBUS_H__
#define __MSGBUS_H__
#include "msgbus_type.h"

typedef struct _msgbus_ops {
    void (*init)(void);                                                                       /**@ 初始化 */
    void (*register_topic)(const char *name);                                                 /**@ 注册主题 */
    void (*unregister_topic)(const char *name);                                               /**@ 取消注册 */
    int (*publish_topic)(const char *name, void *data, uint16_t len);                         /**@ 发布消息 */
    msgbus_client_t *(*subscribe_topic)(const char *name, msgbus_cb_t cb, void *pirv);        /**@ 订阅消息 */
    int (*unsubscribe_topic)(msgbus_client_t *client);                                        /**@ 注销订阅 */
    void (*set_client_attr)(msgbus_client_t *client, msgbus_client_attr_t attr, void *data);  /**@ 设置属性 */
}msgbus_ops_t;

const msgbus_ops_t *msgbus(void);                                               /**# 消息总线 */


#endif
